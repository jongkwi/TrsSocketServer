# 작업 로그

## [2026-10-14] DB 생성 수동화 + 복원 스크립트를 실 서버 구성(PG15 39999, nasacti)에 맞춤

### 작업 내용
- 사용자가 Rocky 서버(onipx)의 기존 PostgreSQL 15(포트 39999)에 `createdb -p 39999 -O nasacti -D pg_default -E UTF8 nasa_license` 로 DB 를 수동 생성 → "DB 는 무조건 수동 생성, 나머지 복원 부분 수정" 요청
- `Source/onlic/db/00_create_db.sql` 삭제(git rm, `_backup/` 보관). 롤 `onlic` 신설 계획 폐기 → 기존 롤 `nasacti` 사용
- `scripts/db_export_mssql_to_pg.py`
  - `write_apply_scripts`: 생성되는 `apply.sh` 가 DB 를 만들지 않고 **접속 확인 + 인코딩(UTF8/EUC_KR) 확인** 후 실패 시 `createdb` 안내하고 종료. 기본 접속 `127.0.0.1:39999 nasacti nasa_license` (ini `[output] pg_host/pg_port/pg_user/pg_db`, 환경변수 PGHOST/PGPORT/PGUSER/PGDATABASE 로 덮어쓰기). `apply_commands.txt` 도 `-h -p -U -d` 전체 표기
  - `copy_schema_and_apply` 분리: 추출 폴더에 `01_nasa_license_schema.sql` 만 복사(00 복사 제거)
  - 신규 옵션 `--regen-apply <폴더>`: 추출 없이 기존 폴더의 `apply.sh`/`apply_commands.txt`/01 사본만 재생성 → `Source/onlic/db/data/20261013_165211`, `20261013_171713` 두 폴더에 적용(구버전 추출이라 apply.sh 가 없었음). `bash -n` 통과
  - 패치 중 heredoc 이스케이프 실수로 `io.open(... newline=)` 줄이 깨져 SyntaxError 1회 → Edit 로 복구, `py_compile` 통과
- `scripts/db_export.ini.sample` + 로컬 `db_export.ini`: `pg_host=127.0.0.1 pg_port=39999 pg_user=nasacti` 추가
- `02a/02b *.load`: INTO `postgresql://nasacti:PASSWORD@127.0.0.1:39999/nasa_license`
- `01_nasa_license_schema.sql` 헤더 적용 절차 수정(수동 createdb 전제)
- `Source/onlic/db/README.md`: §0 "DB 생성은 수동" 신설(명령·인코딩 대안·재생성·접속 덮어쓰기), 파일표·적용 절차·pgloader 절차에서 00 제거 및 `nasacti@39999` 로 통일, `--regen-apply` 안내
- `docs/작업요약_2026-10-13.md` §8-2 갱신

### 서버 적용 결과 (추가, 같은 날)
- 사용자가 오늘 재추출(`scripts/20261014_150341`, 44 IPCS_LOG 326,341행)한 폴더를 서버 onipx 에 복사해 `./apply.sh`(B 역할) 실행 → `== done ==`. 최종 건수: lic_access_list 859, nasa_lic 698, ipcs_log **328,319**, callcenter_link 2,050, site_sms_user 25,649, site_sms_basic_user 28, site_email_user 8, site_smsexcludetime 4
- ipcs_log 328,319 = 326,341 + 1,986 − **8**: 7 의 IPCS_LOG 중 44 와 5컬럼이 완전히 같은 행 8건을 append 모드가 건너뜀(로컬 SQL 파일 대조로 확인). 정상
- **문제**: `12_data_7.sql` 의 IPCS_LOG 병합 문장(`NOT EXISTS ... IS NOT DISTINCT FROM` 5컬럼)이 인덱스를 못 타 1,986 × 326k 중첩 비교 → 사용자가 멈춘 것으로 오인할 만큼 오래 걸림(수 분). 완료는 됨
- **수정**: 생성기 `convert()` 의 병합을 `INSERT ... SELECT ... FROM (SELECT ... FROM _stage EXCEPT ALL SELECT ... FROM 본테이블) d` 로 교체(해시 집합 연산, NULL 동등 취급, 의미 동일). 이미 추출된 로컬 폴더 5개(`Source/onlic/db/data/2026101*`, `scripts/2026101*`)의 `12_data_7.sql` 도 같은 문장으로 치환. 서버에 올린 사본은 이미 적용 완료라 재전송 불필요

### 서버 DB 원격 검증 (추가, 같은 날) — PC 의 psql 16 으로 203.239.159.43:39999 `nasacti` 접속 (비밀번호는 로그·문서에 기록 안 함)
- PostgreSQL 15.17, `nasa_license` UTF8 / en_US.utf8, 58 MB. 테이블 19개 전부 존재, 소유자 nasacti, PK 13 + FK 1, 인덱스 22
- 건수: 19개 테이블 전부 `scripts/20261014_150341/_report.txt` 와 일치 (ipcs_log 는 326,341+1,986−8 중복 = 328,319)
- identity 시퀀스 16개 모두 `MAX+1, is_called=false` 로 보정됨 → 다음 INSERT 충돌 없음
- 앱 쿼리 확인: `lic_access_list` 190.7 IP 조회 1건, `CallCenter_Link cc_use='Y' AND cc_link_error='N'` 381, cc_use Y 607/N 1,443, sms_use T 11,647/X 13,999/Y 3. `cc_update_time` 은 `YYYYMMDDHHMMSS` 문자열 그대로. IPCS_LOG 최신 2026-10-14 15:00
- **이슈(인코딩)**: 문자 컬럼 159개를 `convert_to(col,'EUC_KR')` 전수 검사 → 2개 실패. `callcenter_link_reference.comment` 에 '튵'(U+D2B5, KS X 1001 밖의 CP949 확장 한글) / `ipcs_log.log_ip` 에 U+FFFD(추출 시 해석 불가 스캐너 바이트 치환). **PostgreSQL 의 `EUC_KR` 은 완성형 2,350자만 지원** → CP949 확장 한글이 들어오면 onlic 이 `client_encoding=EUCKR` 로 읽을 때 변환 오류. `convert_to(col,'UHC')`(CP949) 로 재검사하면 `ipcs_log.log_ip` 의 U+FFFD 만 실패(onlic 은 log_ip 를 SELECT 하지 않고 INSERT 만 하므로 영향 없음, 비 IP 값 12,394행은 스캐너 쓰레기)
- **권고**: onlic `CLIENT_ENCODING` 을 `EUCKR` → **`UHC`** 로 (PG 클라이언트 전용 인코딩, CP949 = Windows-949 와 동일). 바꿀 곳: `Source/onlic/cfg/onlic.ini.sample:31`, `ConfigLoad.cpp:85` 기본값, `docs/리눅스참조구조.md:138`, `docs/DB스키마.md:39`, `db/README.md` §0. 사용자 확인 후 적용
- 검사 중 psql.exe(Windows) 가 `-c` 의 한글·`SET client_encoding` 이후 쿼리를 콘솔 코드페이지로 보내 3회 오류 → 데이터 문제 아님(ASCII 구문으로 재검사)

### 쓰레기 데이터 정리 (추가, 같은 날)
- 사용자 "필요없으면 빼게" → 정체 확인: (1) `callcenter_link_reference` 59행 중 55행이 사이트 22184(123stepupCTI) 의 OnAlarm 웹 테스트 입력(키보드 난타), 그중 3행(ccr 16·24·52)이 CP949 확장 한글(튵·퓽·녿·슽) (2) `ipcs_log.log_ip` U+FFFD 11,626행 = IVR(주로 218.238.95.246, 2017-02~2024-09)이 보낸 깨진 IP 문자열, 행 자체(log_real_ip·시각·타입)는 정상. 그 외 IP 아닌 값 768행(`??`, `pbx2.synaptory.com` 등)은 변환 문제 없어 유지
- `Source/onlic/db/04_cleanup.sql` 신규(재실행 가능): 22184 참조 메모 DELETE + U+FFFD 행의 `log_ip=''` UPDATE(행 유지) + 검증 SELECT. 생성기 `copy_schema_and_apply` 가 01 과 함께 복사, `apply.sh` 마지막 단계로 실행. `scripts/20261014_150341` 에 `--regen-apply` 반영
- 서버에 직접 DELETE/UPDATE 실행은 Claude Code 권한 분류기(Mass Delete)에 막혀 미적용 → 사용자가 재추출·재복원(또는 `04_cleanup.sql` 단독 실행)으로 적용하기로
- **원복(사용자 요청 16:02)**: `04_cleanup.sql` 작성 취소. 파일은 `_backup/04_cleanup.sql.bak_20261014_160223` 로 이동, 생성기 복사·`apply.sh` 마지막 단계·README 표기 제거, `scripts/20261014_150341` apply.sh 재생성. 쓰레기 데이터(참조 메모 55행, log_ip U+FFFD 11,626행)는 **정리하지 않고 그대로 둠**. 자세한 정체: 참조 메모는 190.7 OnAlarm 웹(2025-05-20 생성 테이블)의 테스트 입력, log_ip 는 IVR(218.238.95.246)이 `IP:` 뒤에 보낸 초기화 안 된 버퍼를 서버가 그대로 기록한 것(판정은 log_real_ip 사용)

### Rocky 9 첫 빌드 (추가, 같은 날) — 사용자가 서버 onipx 에서 `cd linux && make`
- 라이브러리 5개(`liblist_object` `liblog_object` `liburl_en_de` `libshare_lib` `libtimerobject`) 빌드 성공. 앱 링크 단계에서 오류 1 + 경고 1종
- **오류** `HttpWaitStack.cpp:138: 'struct ALARM_OBJ' has no member named 'alert_type'` → Windows `AlramWeb.h:30` 의 `int alert_type`(저장만, 미사용)을 `struct_def.h` ALARM_OBJ 로 옮길 때 누락. `struct_def.h` 에 `int alert_type;` 추가 (원본 동일)
- **경고** `SocketThread.h:9-10 F_READING/F_DONE redefined`(onmw `shareheader.h:218/220` 과 충돌) → `F_CREATE/F_READING/F_DONE` 3줄은 어디서도 미사용이라 제거
- 두 파일 EUC-KR+CRLF 유지, `_backup/*.bak_20261014_154453` 백업. 서버 재빌드 대기(다음 오류·링크 오류 가능)
- 2차 빌드: 헤더 2개 미반영 상태로 빌드돼 경고 재현(scp 안내). 3차 빌드: **컴파일 전부 통과**, 링크에서 `cannot open output file ../bin/onlic`(서버에 `bin/` 폴더 없음) → `linux/Makefile_64bit` 의 `all:`·`onlic:` 타깃 첫 줄에 `mkdir -p ../bin ../lib ../log` 추가(LF 유지)
- 4차 빌드: 링크 `undefined reference to Malloc(unsigned long)`(TimerObject.cpp:238·407). `Malloc` 은 `libshare_lib.a`(WinMalloc.cpp) 에 있는데 `-lshare_lib` 가 `-ltimerobject` 앞이라 미해소. onmw 는 `*.o` 를 같이 링크해 가려졌던 문제 → 앱 링크 줄 2곳을 `-Wl,--start-group -llist_object -llog_object -ltimerobject -lurl_en_de -lshare_lib -Wl,--end-group` 으로 수정

### 원본 MSSQL 재확인 (추가, 사용자 요청 "7번 DB 에서 깨진 데이터 확인", 읽기 조회)
- `IPCS_LOG.LOG_IP` 쓰레기는 **원본에 그대로 존재**(이관 산물 아님). 190.7: 1,986행 중 비IP 109·비ASCII 88행(예: `80 3F 01`, `E0 BD 41 01`, 0x01 제어문자 포함, 송신지 14곳: 175.208.11.77 50행(2017-12-15) 등). 190.44: 326,345행 중 비IP 12,285·비ASCII 8,357행(218.238.95.246 7,722행 2023-04~2024-09, 203.234.226.33 167행 등, 최신 2026-03-04 211.33.155.24). PATINDEX 집계라 PG 측 U+FFFD 11,626행과 집계 방식 차이 있음
- `CallCenter_Link_Reference`(190.7): 15:30 조회 때 59행(사이트 22184 테스트 55행) → **16시 재조회 시 3행만 남음**(ccr 59 사이트 21654 실메모 2025-11-18, ccr 60·61 사이트 22184 `test` 2026-10-14 15:42:02 신규). 테스트 메모 56행이 원본에서 오늘 삭제됨(OnAlarm 웹 쪽 작업 추정, 사용자 확인 필요). 재추출하면 PG 도 동일해짐

### 43 서버 재복원 검증 (추가, 사용자가 `scripts/20261014_162559` 로 재추출·재복원)
- 19개 테이블 건수 원본과 전부 일치(ipcs_log 328,323 = 326,345 + 1,986 − 8). `callcenter_link_reference` 3행(원본 정리 반영). 시퀀스 보정 정상(ipcs_log 646135, ccr 62)
- 전 테이블 전컬럼 동일 행 중복 검사: 이관으로 생긴 중복 0. `site_sms_user` 225행 중복은 **원본 MSSQL 에도 동일**(25,649 / distinct 25,424, PK 없음) → 그대로 이관된 것
- `ipcs_log`: `lic_index` 644157~644160 이 2행씩 겹침(PK 는 (lic_index, lic_insert_date) 라 오류 없음). 원인: 12_data_7 의 append INSERT 가 13_after_insert 의 setval **이전**에 실행되어 시퀀스가 지난 복원 값(644157) 에 머문 채 새 값을 부여 → 44 본체의 기존 lic_index 와 충돌. 생성기 `convert()` 수정: append INSERT 직전에 identity 컬럼 setval(MAX+1) 추가. 다음 복원부터 해소
- `ipcs_log` 5컬럼 동일 행 203,107 은 44 원본의 반복 기록(같은 분에 같은 사이트 15회 등) → 정상. append 의 EXCEPT ALL 이 건너뛴 8행은 lic·lic2 양쪽에 같은 분에 기록된 정상 이벤트일 가능성 있으나 **사용자: ipcs_log 는 중요 데이터 아님, 깨진 데이터 실패 무방** → 추가 조치 없음
- EUC_KR 변환: 159 컬럼 중 `ipcs_log.log_ip` 1개만 실패(U+FFFD) → 위 결정으로 무시

### 기동 배너·실행 스크립트 (추가, 사용자 요청 "onmw 처럼 띄워 달라")
- `onlicmain.cpp` `OnlicIntro()` 를 onmw `NcmwIntro()` 와 같은 61칸 상자형 배너로 교체(제품명·Release Version·컴파일 정의·포팅 원본·Copyright). 기동 완료 메시지 `onlic Server Start!!` → `License/Alarm Relay Server Start!!`. EUC-KR+CRLF 유지, `_backup/onlicmain.cpp.bak_20261014_164715`
- `bin/runonlic` 신규(onmw `runmw` 대응, ASCII): `./onlic -d -c ../cfg/onlic.ini`. `doc/` 의 runmw·mwcron·ps_mw.chk.sh 는 아직 onmw 원본 그대로(acromw 이름·경로) → 사용자 확인 후 onlic 경로로 수정 예정
- 확인: 실행 옵션은 onmw 와 동일(-d/-c/-p/-v/-h), `-n ip4/ip6`·`-s`·`-i` 없음, `-l`(포그라운드 로그) 추가. `MY_IP` 빈값 = INADDR_ANY(권장). ini 파서가 `;` 주석과 값 끝 공백을 제거하므로 샘플 인라인 주석 사용 가능. `GetPrivateProfile.cpp:42-63`
- 발견: Bash 도구 heredoc 안에서 `\\n` 이 한 단계 풀려 Python 에 `\n` 으로 전달됨 → 앞서 io.open 줄 깨짐·패턴 불일치의 원인. 백슬래시가 든 패치는 Write 로 .py 파일을 만든 뒤 실행하는 방식으로 전환

### onlic 소스 최종 검증 (추가, 사용자 요청) — 검토 에이전트 3개 병렬(소켓·프레이밍 / 업무로직·DB / 기동·워커·HTTP) + 핵심 지적 직접 재확인
**높음 (수정 필수)**
- S1 `SocketThread.cpp:256-267`→`SockProcLic.cpp:43-46`: 6321 응답을 워커가 fd 번호 사본으로 send. 클라이언트 5초 타임아웃·유휴 정리로 세션이 닫힌 뒤 같은 fd 가 새 접속/ODBC/로그파일에 재사용되면 엉뚱한 곳에 "Run" 전송(미허가 장비 Run 가능). Windows 는 단일 스레드라 없던 문제. 수정: SOCK_RECV_WAIT 에 sock_seq, 전송 전 리스트 뮤텍스 아래 세션 일치 확인
- S2 `onlicDefine.h:9-11` FD_SETSIZE 2048 재정의는 glibc 에서 무효(1024 고정). fd≥1024 면 FD_SET 스택 밖 쓰기·select EINVAL 무한 루프, EMFILE 시 accept 실패 반복으로 CPU 100%. 수정: accept 후 fd≥FD_SETSIZE 즉시 close, EMFILE 시 100ms 대기, setrlimit, 운영 idle timeout 사용(장기: poll)
- S3 `SocketThread.cpp:229,276` 뮤텍스 보유 중 블로킹 send(논블로킹·SNDTIMEO 없음). 뷰어 PC 절전으로 송신버퍼 차면 이벤트 워커가 alarm_socklist_mutex 쥔 채 영구 블록 → UDP 처리·ACK 전면 정지. 수정: MSG_DONTWAIT 추가, EAGAIN 은 실패 집계(Windows CAsyncSocket 과 동일)
- L1 `ShareLib/GetPrivateProfile.cpp:34,67` ini 값·기본값을 printf 포맷으로 nprintf 에 전달(onmw 원본 동일). [ALARM_MSG] 값의 %s 가 varargs 로 해석되어 기동 시 크래시/쓰레기, 비밀번호·URL 의 % 도 깨짐. 수정: "%s" 포맷 경유 (2줄)
- L2 `ShareLib/nprintf.cpp:12-19` 는 무제한 vsprintf 후 검사, err_quit 는 출력만(종료 없음) → 초과 시 조용한 오버플로. 도달 경로: `HttpWaitStack.cpp:301-331` email_title[201] = cc_name(≤256)+ip+proc, `AlarmProc.cpp:335-497` szMsg[1024] 는 [ALARM_MSG] 511자 문구 시 초과, `ConfigLoad.cpp:75-85` DSN/UID/PWD 51 초과값. 수정: onlic 쪽 nprintf 를 vsnprintf 절단으로(라이브러리 사본 수정) + 사이트명 상한
- B1 `AlarmProc.cpp:272` cc_nasupload_fail 비교값이 'YYYY-MM-DD'(DbLic_TodayHyphen). **Windows 원본 `TrsSocketServer.cpp:812` 는 YYYYMMDD** (하이픈 :1560 은 로그 파일명 전용) → 확인결정사항 B12 의 전제가 오독, 운영값 20240822 가 현 소스와 일치. 영향: 절체 후 전 사이트 첫 303/304 가 긴급 1회 오발, 이후 형식 불일치. 수정: DbLic_Today8 사용, B12 정정
- B2 `DirectODBC.cpp:90`·`ConfigLoad.cpp:85` client_encoding EUCKR → CP949 확장 한글 INSERT/SELECT 실패. 수정: 기본값·샘플 UHC (오늘 DB 검증과 동일 결론)
**중간**
- B3 `DirectODBC.cpp:111` m_nConn==TRUE 일 때만 SQLDisconnect → 08xxx 단절 후 재접속마다 DBC+ENV+드라이버 소켓 누수. 수정: 무조건 SQLDisconnect
- B4 `DirectODBC.cpp:310` SqlEscape 가 개행을 공백으로 → TXT_TABLE_CHECK/TXT_NEW_SITE 알림톡 문구 줄바꿈 소실(원본은 유지). 수정: CR/LF/TAB 통과
- B5 `DirectODBC.cpp:118-124,219-226` 모든 실패에 재접속(데이터 오류 포함), DB 불통 시 쿼리당 LOGIN_TIMEOUT 30초×2. 수정: 08xxx 일 때만 재접속, LIC 로그인 타임아웃 3~5초
- B6 `AlarmProc.cpp:606-626` vs `:423-430` 타이머·워커 경합(21분 경계 LINK_CHECK 직후 타이머가 'Y' 전이 → 가짜 장애/복구). 수정: SetLinkError 에 cc_update_time 조건 + 영향행 0 이면 알람 생략
- S4 `SocketThread.cpp:349,364` 유휴 정리가 응답 대기 중 세션을 닫음(S1 과 결합). 수정: pending 카운터
- L3 `ConfigLoad.cpp:71` LOG_PATH 255 허용 vs LogObject m_log_path[100]/szFullFilename[101] → 78자 초과 시 로그 스레드 오버플로. 수정: 70자 상한 검사
- L4 `onlicmain.cpp:265-276` quit_func 가 syslog·OnlicLog(뮤텍스) 호출 후 플래그 설정 → 로그 스레드에 시그널 오면 자기 교착, TERM 무시. 수정: 핸들러는 플래그만
- L5 종료 시 스레드 join 없이 3초/1초 대기 후 delete(`HttpWaitStack.cpp:112`, `RecvWaitStack.cpp:211-217`, `MwTimer.cpp:82-89`), curl 타임아웃 10초·DB 30초보다 짧음. Pthread_kill(SIGINT) 는 스레드를 죽이지 못함. 수정: pthread_join(상한 대기)
- L6 `onlicmain.cpp:78→87` 중복 실행 검사가 StartSystem 뒤 → 2번째 인스턴스가 DbLic_UpdateTimeAll 로 전 사이트 cc_update_time 초기화(실 장애 21분 은폐). 수정: PID 검사를 StartSystem 앞으로
- L7 `onlicmain.cpp:82,90` 실패 시 return 0 + StopOnlicSystem 미호출, 포트 점유 오류가 stdout 에 안 나옴. 수정: return 1, SOCK_ERROR 는 PRINT, Stop 호출
- L8 `SystemLoad.cpp:360-364` `-l` 옵션이 로그 객체 생성 후 무효. 수정: m_nLogView 면 stdout 병행
**낮음**: S5 UDP ETX<STX 순서 datagram 을 빈 이벤트로 처리 / S6 recv -1 EINTR·EAGAIN / S7 종료 순서(소켓 close 후 워커 send, S1 로 해소) / S8 framing_sim.py 500 경계 벡터가 499바이트(테스트 보강) / B7 NAS_CON_FAIL 읽기 실패 시 -1→0 vs 0→1 / B8 [ALARM_MSG] 검증이 % 개수만 / B9 MSSQL 핸들에도 set client_encoding 전송(sqlerr 1건) / L9 로그 줄 4200 초과 시 링 슬롯 침범 ≤23B / L10 curl_global_init 가 HTTP 스레드 기동 뒤 / L11 daemon_init 이 0/1/2 미리다이렉트 / L12 ini 값의 `;` 절단, CLIENT_ENCODING 미검증 / 비원자 s_sock_seq++
**이상 없음 확인**: Framing 바이트 규칙 1:1(strlen 절단·마지막 STX~첫 ETX·500/1000), 6321 raw Run/Close·미응답 조건, UDP ACK 5경로, 6770 푸시·하트비트 프레임, ServerUdpProc nType 전부 패리티(문구·템플릿·긴급·cc_pause·제외시간·신규1·NAS 10/999·21분 전이), SQL 전부 DDL 과 컬럼명·개수 일치(SITE_EMAIL_USER 16=16), ESC 전수 적용, new[]/delete[] 쌍, 워커 큐 mutex/cond, 타이머 재진입 없음, HTTP 버퍼 크기, 배너 61칸

### 최종 검증 결함 수정 (1~4단계, 사용자 승인 "poll 방식으로 변경하고 모두 수정")
백업 `_backup/onlic_20261014_173621/`(32개). 전부 EUC-KR+CRLF 유지(Write 로 스크래치에 작성 → cp949 변환 / Python 패치). 버전 `1.0.0.2 / 2026-10-14`
**1단계 소켓 (`SocketThread.cpp` 전면 재작성)**
- S2 `select`→**`poll()`**: `BuildPollFds()` 가 리스트에서 pollfd 배열 구성(리슨 [0] + 세션), fd 1024 제한 제거. `onlicDefine.h` 의 무효한 `FD_SETSIZE 2048` 삭제. `Tcp_Accept` EMFILE/ENFILE 시 100ms 대기(CPU 폭주 방지), `onlicmain.cpp RaiseFdLimit()` 로 RLIMIT_NOFILE soft=hard
- S1 `Lic_SendResponse(sock_seq, handle, data, len)` 신설: 워커(`SockProcLic.cpp:43`)는 fd 사본으로 직접 send 하지 않고 리스트 뮤텍스 아래 (handle, sock_seq) 일치 세션에만 송신. `SOCK_RECV_WAIT.sock_seq` 추가, `s_sock_seq` 는 `__sync_add_and_fetch`
- S3 모든 TCP send 에 `MSG_DONTWAIT` (뮤텍스 보유 중 블로킹 금지, EAGAIN 은 실패 집계·세션 유지 = Windows CAsyncSocket 동작)
- S4 `ONLIC_SOCKET.pending`(워커 적재 수) 추가, 유휴 정리는 `pending<=0` 세션만. S6 recv EINTR/EAGAIN 유지. S7 종료 순서는 S1 로 해소
- S5 `Framing_UdpExtract` 프레임 없음 = **-1**(빈 프레임 STX ETX 는 0), UDP 루프의 strchr 보정 삭제. `framing_test.cpp` 는 -1 → "none"
**2단계 DB·라이브러리·설정**
- B1 `AlarmProc.cpp:272` `DbLic_Today8`(YYYYMMDD) 로, `DbLic_TodayHyphen` 삭제. 문서 B12 정정
- B2 `ConfigLoad.cpp` 기본 `UHC`, `onlic.ini.sample CLIENT_ENCODING=UHC`, `DirectODBC::SetSessionProc` 값 검증([A-Za-z0-9_]). 리눅스참조구조·DB스키마 반영
- B3 `DisConnectODBC` 는 `m_nConn` 과 무관하게 `SQLDisconnect` (08xxx 뒤 DBC/ENV 누수 차단)
- B4 `SqlEscape` CR/LF/TAB 통과 (TXT_TABLE_CHECK·TXT_NEW_SITE 알림톡 줄바꿈 보존)
- B5 `OpenTable/ExecSql` 재접속은 `m_nConn==FALSE`(08xxx·핸들 오류)일 때만. `DATABASEINFO.lLoginTimeout` 신설: LIC 5초, ATALK/SMS 30초(`SystemLoad.cpp:137`)
- B6 `DbLic_SetLinkError(..., szLocalDay14)` 가 `AND cc_link_error='N' AND cc_update_time<='기준'` 조건으로 갱신 행 수 반환(신설 `CDirectODBC::ExecSqlRows/ExecLastSqlRows`, `ExecSql` 은 이를 래핑). `LinkCheck_OnTimer` 는 'Y' 전이를 먼저 하고 0행이면 알람·알림톡·메일 생략(타이머·UDP 워커 경합 제거). DB 오류(-1) 는 원본대로 알람
- B7 NAS_CON_FAIL 읽기 실패 -1 유지(원본 -1→++→0). B8 `[ALARM_MSG]` 검증을 변환 지정자 순서 비교(`AlarmText_FmtSpec`)로. B9 `DATABASEINFO.nDbType` + ini `SMS_DB_TYPE`(기본 1=MSSQL): PG 세션 설정은 PG 핸들에만
- L1 `ShareLib/GetPrivateProfile.cpp:34,67` ini 값·기본값을 `"%s"` 로 전달(포맷 문자열 주입 차단). L2 `ShareLib/nprintf.cpp` `vsnprintf(buf, size+2)` 로 절단(호출 관례 sizeof-1 유지). 두 파일 머리에 onmw 와의 차이 주석. `HttpWaitStack.cpp` 메일 제목은 사이트명 100바이트 절단(`Framing_HangulStrCut`)
**3단계 기동·종료**
- L4 `quit_func/pipe_func` 는 `volatile sig_atomic_t` 플래그만, 로그·syslog 는 메인 루프. L6 PID 중복 검사를 `StartSystem` **앞**으로(ini PID_FILE 선독). L7 실패 시 `return 1` + `StopOnlicSystem()`, SOCK_ERROR 는 stdout 에도. L8 `-l` 이면 로그 객체와 무관하게 stdout 병행. L9 `OnlicLogf` `MAX_LOG_CHAR-32`. L10 `curl_global_init` 을 StartSystem 앞으로. L11 데몬은 기동 메시지 출력 후 `DetachStdio()` 로 0/1/2 → /dev/null (배너는 터미널에 보임). L3 `LOG_PATH_MAX_LEN 70` 초과 시 CONFIG 오류
- L5 join: `StopOnlicSystem` 소켓 스레드 `JoinThreadVar`(5초), `CRecvWait::Kill` 45초 대기 후 `pthread_join`(미종료 시 DB 핸들 해제 생략), `CHttpWait::Kill` 15초, `CMwTimer::EndUserTimer` 45초(미종료 시 delete 생략). `Pthread_kill(SIGINT)` 전부 제거
**4단계 테스트·문서**
- S8 `framing_sim.py`: residual_overflow 주석 정정, `frame_split_near_500`(498+1)·`frame_over_500_lost`(500B 잔여 폐기 규칙) 추가, `udp_none` 벡터(ETX 가 STX 앞 → none). `framing_vectors.txt` 재생성. `확인결정사항.md` B12 정정, `리눅스참조구조.md` §4·ini, `DB스키마.md` §1.2·§1.3, `Source/onlic/README.md`
**낮음 중 미반영**: L12 ini `;` 절단은 파서 사양으로 샘플 머리에 명시만. B9 는 ini 스위치로 대체. 그 외 낮음 항목은 모두 반영

### 신규 기능: 재시작 직후 링크 장애 알림 억제 (사용자 요청)
- 요청: 재시작 후 21분 타이머가 "링크데이터가 수신되고 있지 않습니다" 를 묶음 발송 → 재시작 이후 첫 판정은 무시
- 원인: 기동 시 정상 사이트의 cc_update_time 을 현재로 초기화 → 21분 뒤 그 사이 LINK_CHECK 가 없던(재시작 전부터 죽어 있던) 사이트 전부 판정. 에이전트 주기 5분이므로 창 = 21+5 분
- 구현: `[LINK_CHECK] STARTUP_SILENT_MIN=26`(0=끔). `AlarmProc.cpp` `LinkCheck_OnTimer` 에 기동 시각(s_tStart) 기록, 창 안 판정은 `DbLic_SetLinkError` 로 Y 전이만 하고 `SilentSite_Add` 후 알림 전부 생략(로그 `LinkCheck startup silent`). `SEND_TYPE_LINK_CHECK` 수신 시 `SilentSite_Remove` 가 TRUE 면 복구 알림(TXT_LINK_RETURN·NET_SUCC 메일) 생략. 목록은 2048 개, 뮤텍스 보호(타이머 스레드 Add / UDP 워커 Remove)
- 변경: `struct_def.h` startup_silent_min, `ConfigLoad.h/.cpp` CFG_STARTUP_SILENT_MIN(기본 "26"), `SystemLoad.cpp` 로드, `cfg/onlic.ini.sample`, `docs/기능목록.md` N-01, `docs/리눅스참조구조.md` ini. EUC-KR+CRLF 유지
- 보강(사용자 "26분 안전한가?"): 시간 창만으로는 재시작 직후 한 번 보내고 죽은 사이트도 무알림 처리됨 → 조건을 **창 안 AND `cc_update_time`==기동 초기화 시각**(재시작 후 수신 0회)으로 좁힘. `DbLic_SiteCountUpdateTime` 신규, `AlarmProc.cpp` s_szInitDate 보관. 창 길이와 무관하게 안전

### ODBC 드라이버 확인 (추가) — 사용자 실측으로 정정
- 43 서버 `odbcinst.ini` 는 PGDG `/usr/pgsql-15/lib/psqlodbc.so`(Unicode 빌드). isql 조회 결과의 깨진 패턴(`▒▒▒Ǿ▒▒׽▒Ʈ167`)을 바이트로 풀면 CP949 `BF C2 C7 C7 BE C6 …` 를 UTF-8 터미널이 그린 것 → **isql 이 받은 바이트가 이미 CP949**. 사용자가 수정 전 onlic 빌드로 "링크데이터가 수신되고 있지 않습니다" 알림톡을 한글 정상 수신한 것과 일치
- 제가 "Unicode 드라이버라 ANSI(psqlodbca.so) 로 바꿔야 한다" 고 안내했던 것은 **오판** → 취소. 사용자가 추가했던 ANSI 항목 제거. 드라이버·DSN 은 기존 그대로, `CLIENT_ENCODING=UHC` 유지. `docs/DB스키마.md` §1.2·§1.3 정정

### 결정 사항
- **ipcs_log 는 중요 데이터 아님**(사용자 2026-10-14): 깨진 log_ip·8행 누락·lic_index 겹침은 운영에 무관, 정리하지 않음
- ~~쓰레기 데이터 최소 정리~~ → **정리 안 함**(사용자 원복 요청). onlic 동작에 영향 없음(log_ip 는 INSERT 만, 참조 테이블은 미사용). 인코딩 문제는 `CLIENT_ENCODING=UHC` 로 대응
- DB 생성(및 재생성 `dropdb`)은 항상 운영자가 postgres 계정으로 수동 수행. 스크립트는 생성·삭제 권한을 갖지 않음 (실수로 운영 DB 를 덮는 경로 차단)
- 포트 39999·롤 `nasacti` 는 서버에 이미 있는 PG 인스턴스(onpia_check 와 같은 클러스터)를 그대로 쓰는 구성. 별도 PG 설치·`onlic` 롤 생성 계획 폐기
- 인코딩은 UTF8 유지(클라이언트 EUCKR). CP949 확장 문자 변환 오류가 실제로 나면 EUC_KR DB 로 재생성(수동)

### 다음 할 일
- 서버·PC 타임스탬프 폴더 삭제(개인정보). 절체 직전 재추출·재복원 1회 더
- **CLIENT_ENCODING=UHC 전환**(위 권고, 사용자 확인 후): ini 샘플·ConfigLoad 기본값·문서 4곳
- ~~최종 검증 결함 수정~~ 완료(위). **서버 반영**: `Source/onlic` 전체(source/onlic, include/onlic, source/ShareLib 2개, cfg 샘플, test) scp → `make -f Makefile_64bit`(라이브러리 포함 전체 재빌드: nprintf/GetPrivateProfile 변경) → `cd test && make && ./framing_test` → `bin/onlic -l` 기동 → 시뮬레이터로 6321/5363/6770 확인
- 확인결정사항 B12 정정(cc_nasupload_fail 은 YYYYMMDD 가 원본)
- `odbc.ini` DSN `ipcs_log`(localhost:39999 nasa_license) / `onpia_check` 작성 → `cfg/onlic.ini` → `cd linux && make` 빌드 → `bin/onlic -l` 기동
- `test/framing_test` 실행, 시뮬레이터로 6321/5363/6770 확인

---

## [2026-10-13] 소스 완전 분석 + Linux 포팅 기준선 작성

### 작업 내용
- 목표 확정: **기존 클라이언트 무변경으로 Linux 포팅**. 이번 단계는 분석·기능 리스트까지 (포팅 코드 없음)
- Explore 에이전트 3개(소켓/앱로직/DB·HTTP)로 병렬 탐색 후 핵심 함수 직접 재확인(iconv cp949)
- 산출물 (`docs/`)
  - `기능목록.md` — F-01~F-90 기능 기준선, X-01~X-14 죽은 코드, B-01~B-16/S-01~S-03 결함·보안, Z 검증 체크리스트
  - `프로토콜명세.md` — 6321/6770/5363 바이트 레벨 프레임, 재조립 규칙(500/1000/strlen), nType 30종 표, ACK 무응답 5경로, 외부 HTTP/알림톡 포맷
  - `포팅영향분석.md` — Windows API→Linux 대체 매트릭스, DB 방언, 설정 외부화 목록, 인코딩 전략, 결정 필요 사항 §8
  - `TrsSocketServer_architecture_diagram.html` — 5탭 SVG 다이어그램 (직접 작성)
  - deep-analyze-c 자동 리포트 3종 (`*_심층분석_*.html/.md`, `*_분석데이터_*.json`) — 단, C++ 클래스 메서드 파싱이 약해 함수 12개/스레드 0개로 집계됨. 참고용
- `scripts/check_refs.py` — 문서의 `파일:라인` 참조를 소스와 대조. 결과 OK 221 / WARN 38 / ERR 0 (WARN은 Linux 대체 API명 등 소스에 없는 게 정상인 심볼)
- `CLAUDE.md`, `README.md`, `docs/설계문서.md` 전면 정정 (아래 결정 사항 참조)

### 결정 사항
- **초기화 때 쓴 "CTI 호분배 미들웨어" 설명은 오류** — `MyDefine.h`의 CTI 정의는 참조 0건. 실제는 콜센터 사이트 감시·알람 중계 서버(IPCS 라이선스 체크 + UDP 장애 이벤트 → 6770 푸시/웹푸시/알림톡/이메일 + 21분 링크 감시)
- 죽은 코드는 별도 섹션(X)으로 분리하고 포팅 제외 권고 (사용자 선택)
- 산출물은 MD + deep-analyze-c 리포트 (사용자 선택)
- 문서 내 DB 비밀번호·알림톡 sender key는 마스킹
- 문서의 소스 참조는 파일명을 항상 명시 (`:NNN` 단독 표기는 앞 항목 파일로 오해됨 — 검증에서 ERR 6건 발견 후 수정)

### 분석에서 확인된 핵심 사실
- 포트 3개 하드코딩(`StdAfx.h:27-29`), 6321 응답은 프레이밍 없는 raw `Run`/`Close`, UDP는 payload echo ACK(LINK_CHECK 등 5경로 무응답)
- DB 3개: `ipcs_log`=MSSQL 2012, `onpia_check`=PostgreSQL(알림톡), `onpia_sms`=제품 불명. 계정 하드코딩(`Dlg.cpp:216-224`)
- 동시성: 단일 UI 스레드 + 알림 워커 1개. DB는 메인 스레드 전용
- 결함: SQL 인젝션 가능, `AlarmWebTh_Delete` 역논리, `this==NULL`, `new[]`→`delete`, 컬럼 수 불일치 등 (기능목록 Y 섹션)
- Release 구성이 v100 툴셋으로 Debug(v142)와 불일치

### 추가 (같은 날) — 클라이언트 라이선스 체크 소스 확보
- 사용자가 IVR 클라이언트 ShareLib의 `WriteServerLog`/`CheckLicense`/`WriteLicense`를 제공 → `docs/reference/client_license_check.cpp`로 보관
- 서버 분석과 대조하여 확정: 요청 프레임 `STX "IVR" 0xFF <MAC> "IP:" <ip> ETX` (index 3은 0xFF), 응답은 `strcmp` 완전 일치(raw `Run`/`Close`), **5초 이내 응답 필수**, 접속 대상 `lic.onpia.co.kr`/`lic2.onpia.co.kr`:6321
- 라인 수·만료 검증은 클라이언트 로컬 `.lic` 파일(MD5 digest)에서 수행. 서버는 IP 허용 여부만 판단 — 서버에 수량 개념 없음 재확인
- 반영: 프로토콜명세 §3.1/§3.3/§3.5, 기능목록 F-07·Z, 포팅영향분석 §1(응답 지연·DNS 행 추가)
- 녹취 서버 `ncir-2.10.22` 소스로 호출 흐름 확정: 기동 시 1회 `CheckLicense`(로컬 .lic) 실패 때만 `WriteServerLog`(서버). 설정 `[CALLCENTER_OPTION] USE_CHECK_LICENSE` 기본 1. 타입 `VRS`. `_NO_CHECK_LIC`는 Makefile에만 있는 죽은 플래그. 프로토콜명세 §3.6, reference/README.md에 기록
- IVR(`ms_260424` 앱 + `ncmsivr` SipStack)도 동일 방식 확인: `LoadTrunkProc` 기동 1회 → `SipMsgLoadLibrary` → `CheckLicense` 실패 && `check_lic==1`일 때만 `WriteServerLog`. 타입 `IVR`, 5초 타임아웃, FreeBSD 빌드만 `220.73.223.*` 우회. 프로토콜명세 §3.7
- 녹취 백업 `bk_source`(ncbk)에서 UDP 5363 송신 확인: `lic2.onpia.co.kr:5363`, 프레임 `STX %03d 0xFF %03d 0xFF ip 0xFF msg ETX`, nType 301~304·306~308 (305 미송신), **fire-and-forget(ACK 안 읽음)**, 하루 1회 억제, DB `ncr_set.vms='C'`일 때만. 프로토콜명세 §5.9
- 감시 데몬 `OnDemon`(AppMonitor)이 UDP 5363 **주 송신자**로 확인: `lic2.onpia.co.kr:5363`, 로컬 UDP 20001 상시 바인드, LINK_CHECK 300초 고정, 0~7·10~13·15 이벤트 송신, **echo ACK로 송신 큐 제거**(재전송 타이머는 미등록), 23~00시 억제. 서버 ACK 유지 필수 근거 확보. 프로토콜명세 §5.10

### 결정 사항 (추가, 사용자 확정)
- 인코딩 CP949 유지 / 결함 Y 표 **수정** / DB **PostgreSQL + unixODBC**(MSSQL 이관) / 죽은 코드 X-01~X-14 제외 / TCP DB 게이트웨이(CDbProxy) **미사용**, ODBC 직접 / 구조는 **onmw_3.10.79 라이브러리·패턴 동일** 적용
- 미확정: `onpia_sms` DBMS와 운영 DDL → 포팅영향분석 §8.1 "추가 필요 정보" 6항목 정리

### onmw 구조 참조 (추가)
- `onmw_3.10.79` 공용 라이브러리 5개 API와 앱 패턴(기동·설정·DB·소켓 스레드·WaitStack·타이머·로그·빌드) 분석, `F:\AI_Pro\_proc\onmw\docs\analysis` 기존 문서 반영
- `docs/리눅스참조구조.md` 작성: 모듈 매핑 표, 스레드/DB 핸들 설계(소켓 3 + 워커 2 + HTTP + 타이머, DB 핸들 7개), ini 설계, 폴더/빌드, 결함 수정 호환 조건, onmw 결함 8종 제외 목록
- onmw `struct_def.h`는 포함 금지(`CALL_DIV_CIRCLE` 값 충돌). `-D_UTF8` 미정의(CP949)

### DB 스키마·ODBC 정리 (추가)
- `docs/DB스키마.md` 작성: SQL 51건 전수 + 2017 MSSQL 백업(`DB/NASA_LICENSE_20170705.bak`) 카탈로그 문자열 추출 + 개발 PC ODBC 레지스트리 대조
- 테이블 9개 컬럼·용도·버퍼 길이·값 형식(`cc_update_time`은 `YYYYMMDDHHMMSS` 문자열, `cc_insert_time`만 datetime), ODBC 호출 방식(2.x API, 로그인 타임아웃 30초, 오토커밋 기본 + 매 문장 `COMMIT`, 재접속 1회), PostgreSQL DDL 초안(추정)
- **발견**: `onpia_sms` DSN은 개발 PC 32-bit 레지스트리에 존재 → **MSSQL `tcssdk`@203.239.190.7** (`sqlsrv32.dll`). `_ATALK` 빌드에서 `SDK_SMS_SEND` 실사용 경로는 IVR 200/201뿐
- **발견**: 2017 백업에 `lic_access_list`, `SITE_SmsExcludeTime`, `cc_nas_failcnt`, `cc_nasupload_fail` 없음(이후 신설). 백업의 `nasa_lic`/`nasa_lic_log`/`nasa_security`/`SITE_AUTH`는 서버 코드 미참조
- **정정**: `MyDefine.h:410-467`의 DB 레코드 구조체 5종은 살아 있는 코드 → 포팅 대상 (CLAUDE.md, 리눅스참조구조 §3 보정)
- `SITE_EMAIL_USER` 컬럼 밀림 결함(16 SELECT vs 15 GetData, `email_url` 누락) 위치를 컬럼 표에 명시
- 포팅영향분석 §3 DB 행·§8 3번 갱신
- **운영 서버 2대 확인**(사용자 DSN 목록 + `server/netstat44.txt`·`netstat7.txt`): 실운영은 190.7(lic2: 로컬 MSSQL 1433 `NASA_License`, 외부 PG 203.239.159.134:39999 `onpia_check`, `tcssdk`는 미사용 추정). 190.44(lic)는 MSSQL이 34333 포트로 떠 있고 앱이 재접속 반복 흔적(TIME_WAIT 46건), 클라이언트 접속 0건. 190.7 6321에 외부 스캐너 세션 13건 누적(서버가 세션을 안 닫음) → Linux 판 유휴 타임아웃 권고. `DB스키마.md` §1.4

### 운영 MSSQL 읽기 조회 (추가, 사용자 승인: 스키마·데이터 확인만)
- `sqlcmd`(SQL Server 2008 도구)로 203.239.190.7 `lic` 계정 접속, `INFORMATION_SCHEMA`·`sys.*`·`SELECT`만 실행. 원본 `server/db/00~07_*.txt`(전화번호 마스킹, `server/`는 .gitignore 추가)
- 인스턴스: SQL Server 2012 SP4 Express, `Korean_Wansung_CI_AS`. `lic` 로그인은 기본 DB master, `NASA_License` db_owner, sysadmin 아님
- `docs/DB스키마.md` 전면 갱신: 6개 테이블 운영 DDL(타입·길이·기본값·PK·identity) 확정, 데이터 분포, PG DDL을 1:1 변환으로 교체
- **최우선 이슈**: `lic_access_list`가 접근 가능한 모든 DB에 없음(직접 SELECT → 오류 208). 그런데 `IPCS_LOG`는 2026-08까지 기록 중 → 현재 소스대로면 전부 `Close` 응답이므로 **배포 exe가 소스와 다를 가능성**. `cc_nasupload_fail` 운영값이 `20240822`(소스는 `YYYY-MM-DD`)인 것도 같은 근거. DB스키마 §1.5
- `onpia_sms`=`tcssdk` 미사용 확정(SDK_SMS_SEND 0행, 보관 테이블 2019-04까지) → 포팅 제외 제안
- 운영 분포: CallCenter_Link 2,049행(cc_use Y 606, 신규1 897), SITE_SMS_USER 25,640행(sms_use X 13,999 / T 11,638 / **Y 3**, 'ALL' 0건), SITE_SMS_BASIC_USER 28(Y 0, T 8, S 11, N 9), SITE_EMAIL_USER 8(사이트 1곳), SITE_SmsExcludeTime 4(sm_msg에 공백 삽입으로 사실상 비활성)
- 코드 버퍼 < 운영 길이: `cc_name`·`sms_site_name` 51<256, `cc_nasupload_fail` 21<128. `cc_update_time`은 varchar(256), `cc_pause` varchar(50), `sm_use` char(2)
- `NASA_License`에는 OnAlarm 웹(2025)·NcLicense용 테이블 13개가 더 있음(`enterprise`, `project`, `server`, `on_alarm_user`, `nasa_lic` 등) → 이관 범위 결정 필요

### 확인·결정 사항 정리 (추가)
- `docs/확인결정사항.md` 작성: 확인 필요 A1~A10, 결정 필요 B1~B16(권고 포함), 확정 사항 C, 착수 순서 D. 우선순위 1 = 구현 착수 전 필수(A1 배포 exe 일치, A2 44 역할, A3 PG 접속, A4 OnAlarm 웹 DB, A5 빌드 환경 / B1 6321 판정 기준, B2 DB 장애 응답, B3 이관 범위, B4 PG 배치)

### 사용자 답변 반영 (추가)
- 배포 버전: 190.44 = 1.0.0.01(2018-07-30, 라이선스 1차), 190.7 = 1.0.0.7(2025-12-08, 알람+라이선스 2차). 소스 `TrsSocketServer.rc:115` FILEVERSION 1,0,0,7 → **190.7은 현재 소스와 동일** → 190.7 도달 요청은 `Close`를 받고 있다고 판단(IPCS_LOG 반복 패턴이 근거). 44의 2018 빌드 소스는 없음
- 결정: MSSQL `NASA_License` 전체 백업 → 신규 Linux PG 복원(B3·B4 확정) / `onpia_check` PG는 기존 서버 유지, ODBC 연결 / `onpia_sms`는 **제외하지 않고** 신규 서버에 SMS Agent 설치해 유지
- 새 쟁점 A7: `_ATALK` 빌드에서 `SDK_SMS_SEND`에 쓰는 경로는 IVR 200/201뿐 → 알람 SMS를 원하면 기능 추가 필요. SMS Agent의 PG 호환 여부도 확인 필요
- 반영: 확인결정사항 A1·A3·A4·A7·B3·B4·B5, DB스키마 §1.4·§1.5·§4·§5, CLAUDE.md

### 190.44 MSSQL 읽기 조회 (추가, 사용자 승인)
- `203.239.190.44,34333` `lic2` 계정(sysadmin) 접속. SQL Server 2012 SP1 Developer x86 / Windows Server 2008 SP2. 원본 `server/db/44_0x_*.txt`
- **`lic_access_list`는 44의 `Nasa_License`에 존재** (859행, 전부 Y, 복합 PK (lic_ip_index, nasa_con_ip), 중복 IP 36, 2026-09-07까지 추가). `IPCS_LOG` 326,235행, 최신 2026-10-13 14:00 = 실시간 → **44가 실제 라이선스 1차 서버**. `nasa_lic` 698행, NcLicense 발급 2026-10-02까지(rocky9 사이트). 44에는 `CallCenter_Link`·`SITE_*` 없음
- 결론: 두 서버는 DB 이름만 같고 역할이 다름(44 라이선스 / 7 알람). 7은 소스와 같지만 `lic_access_list`가 없어 폴백 요청에 `Close`. "배포 exe가 소스와 다르다" 가설 철회
- 반영: DB스키마 §1.1·§1.4·§1.5(재작성)·§2.2·§2.3(실 DDL)·§5·§6, 확인결정사항 A1 해소·A2·A11 신규·B1 (a) 권고·B17 병합 신규, CLAUDE.md, 메모리

### 서버 2대 구성 결정 (추가)
- 사용자 결정: 이중화는 추후. **서버 2대에 각각 로컬 PG 설치(통합 스키마 동일), 앱은 로컬 DB만 사용**
- 설계(리눅스참조구조 §8 신설, §4 `[ROLE]` 추가): A=`lic` 라이선스만(`ALARM=0 LINK_CHECK=0`), B=`lic2` 라이선스+알람. 초기 데이터 A←44, B←44 `lic_access_list` + 7 알람 테이블. `lic_access_list` 양쪽 동기화 필수(권고 cron `\copy`, 대안 논리 복제). A에서 알람 역할을 켜면 UDP를 못 받는 A가 전 사이트를 장애 판정하므로 금지. 추후 이중화는 §8.4에 확장 경로만 기록
- 반영: 확인결정사항 B17 확정·B18~B20 신규·B4·B13 보정, DB스키마 §1.3, CLAUDE.md

### 남은 결정 요약 (추가)
- 사용자 질문(PG 이중화 난이도)에 답변: 스트리밍 복제·논리 복제는 설정 수준, 자동 절체만 어려움. 이중화는 추후로 재확인
- 확인결정사항 §0 신설: 지금 결정 6건(B18·A7+B5·B2·A11·A5·A2), 권고안 진행 8건, 병행 확인 6건, 확정 목록. B12는 소스 형식 유지로 권고 확정

### 결정 사항 1:1 확인 완료 (추가)
- 사용자와 6건을 하나씩 확인: B18 `lic_access_list` 동기화 **안 함**(로컬 DB, 추후 이중화) / A7+B5 SMS 경로 **차후 대비 유지**, 기능 추가 없음(런타임 스위치 권고) / B2 DB 장애 시 **현행 `Close` 유지** / A11 발급 프로그램 **차후 PG용으로 변경, 별도 작업** / A5 **Rocky 9 + onmw와 동일 컴파일 구조**(Makefile, CMake 아님) / A2 **서버 완료 후 도메인 매핑 IP 변경 또는 기존 IP 재사용**으로 전환, 세부 순서는 그때
- 반영: 확인결정사항 §0·A2·A5·A7·A11·B2·B5·B18, 리눅스참조구조 §4(`[ATALK] USE`·`[SMS] USE`)·§8.3·§9(빌드 환경·전환), CLAUDE.md, 메모리
- **구현 착수 전 결정 사항 전부 종료.** 권고안 항목(B1·B6·B8~B16·B19)은 이의 없음으로 적용

### 구현 착수 1단계 — `Source/onlic/` 생성, onmw 라이브러리 복사 (추가)
- 사용자 지시로 `Source/onlic/` 신규 생성: `include/{ShareLib,LogObject,TimerObject,ListObject,UrlEnDe,onlic}`, `source/{동일}`, `linux`, `lib`, `bin`, `cfg`, `log`, `doc`, `test`
- onmw_3.10.79 라이브러리 5개 + `shareheader.h`를 `cp -p`로 **바이트 동일 복사**(`diff -rq` 확인, 60파일). EUC-KR+CRLF 파일 11개는 수정 금지 원칙. 라이브러리 소스에 `_UTF8` 분기 **없음** 확인(`utf8.h` 가드만) → `-D_UTF8` 제외해도 라이브러리 수정 불필요
- `linux/Makefile` 작성: onmw `Makefile_64bit_curl` 구조 유지, 차이는 `-D_UTF8` 제외·앱명 `onlic`·`rm -f *.*` 제거·`libs`/`onlic` 타깃 분리
- `README.md`, `cfg/onlic.ini.sample`(비밀번호 비움, `[ROLE]`·`[ATALK]/[SMS] USE` 포함) 작성. `.gitignore`에 `Source/onlic/{lib/*.a,bin/onlic,linux/*.o,log/*,cfg/*.ini}` 추가
- 이 PC에는 g++/WSL 없음 → 빌드 검증은 Rocky 9 서버(또는 WSL 설치) 필요
- `doc/`에 onmw 운영 스크립트(`runmw`, `ps_mw.chk.sh`, `mwcron`) 참고 사본

### 구현 2단계 — 앱 골격 10개 파일 작성 (추가)
- `Source/onlic/include/onlic/`: `onlicDefine.h`(포트·STX/ETX·nType enum·타이머·역할·로그 채널 매핑·전역 extern·후속 단계 함수 선언), `struct_def.h`(DB 구조체 5종 이식 + `cc_name`/`sms_site_name` 257·`email_url` 읽기용 2049, `DATABASEINFO`, `ONLIC_CONF`, `ONLIC_SOCKET` 세션별 잔여 버퍼 1001, `SOCK_RECV_WAIT`, `ALARM_OBJ`), `ConfigLoad.h`, `MwTimer.h`, `SystemLoad.h`, `onlicmain.h`
- `Source/onlic/source/onlic/`: `ConfigLoad.cpp`(전역 정의 + ini 42키 switch, 기본값 = Windows 하드코딩, 비밀번호 로그 마스킹), `MwTimer.cpp`(onmw 패턴, `Malloc`→`new`, LINK_TIMER 재등록), `SystemLoad.cpp`(기동 순서 8단계, 호환 상수 방어, `OnlicLog/OnlicLogf`), `onlicmain.cpp`(getopt `-d -c -p -l -v -h`, 시그널 항상 등록(SIGPIPE), PID lockf, TZ, curl init, RunOnlicSystem/StopOnlicSystem)
- 모두 EUC-KR + CRLF 로 저장(생성 스크립트 cp949), `file` 확인. 중괄호 균형 확인. **컴파일 미검증**(로컬 g++ 없음)
- onmw 결함 회피: 공유 STX 버퍼 → 세션별 `rdata`, `Malloc` 생성자 미실행 → `new`, 포그라운드 시그널 미등록 → 항상 등록, 설정 경로 51 → 256, daemon_init 반환값 pid 오용 → `getpid()`
- 후속 단계에서 채울 자리(주석으로 명시): `SystemDBConnectProc`(3), `LoadSocketProc`(5), `SmsExcludeLoadProc`(3), `LinkCheck_OnTimer`(5), `*RecvWaitStackInit/Kill`(5), `Run*SocketThread`(5)

### 구현 3단계 — DB 계층 (추가)
- `DirectODBC.h/.cpp`: onmw CDirectODBC 핵심만 이식. ODBC 3.x(`SQLAllocHandle`, `SQL_OV_ODBC3`), 로그인 타임아웃 30초(원본 동일), 접속 직후 `set client_encoding to 'EUCKR'` + `SET statement_timeout`(초→ms), 오토커밋(문장별 COMMIT 제거), `OpenTable/Fetch/GetStr(trim)/GetInt/GetLong/CloseTable`, `ExecSql`(실패 시 재접속+1회 재실행, `SetExecRetry`로 끌 수 있음), `RowCount`(실패 -1 = Windows 동일 → B2 `Close` 유지), `GetValue/GetIntValue`, `SqlEscape`(`'`→`''`, 제어문자 제거), `SQLGetDiagRec` 로 sqlerr 로그(08xxx 면 재접속 표시)
- `DbLic.h/.cpp`: Windows SQL 51건 중 사용 경로 전부를 함수 27개로 이식(주석에 원본 라인). SQL 문자열은 원본과 동일(검증 스크립트로 9개 핵심 문장 일치 확인), 외부 입력은 전부 이스케이프. PG 방언 치환 1건 `DbLic_IsNewSiteExpired`(`DATEDIFF`→`EXTRACT(EPOCH ...)/3600`). `SITE_EMAIL_USER` 16컬럼 전부 읽어 밀림 결함 수정. 알림톡 고정값·SMS 고정값은 ini
- `SystemLoad.cpp`: `SystemDBConnectProc`(라이선스 DB 실패 시 기동 중단, 알림톡/SMS 는 경고 후 사용 시 재접속), `SmsExcludeLoadProc`, `KillSystemMemory` DB 해제. `onlicDefine.h` 에 두 헤더 포함
- 컴파일 미검증(로컬 g++ 없음). 인코딩 EUC-KR+CRLF, 중괄호 균형 확인

### 구현 4단계 — 프레이밍 + 참조 모델 테스트 (추가)
- `include/onlic/Framing.h`, `source/onlic/Framing.cpp`: 원본 `ReadMsgProc/SearchSocketSTX_ETX/CutSocketSTX_ETX/ReadUdpMsgProc/StrSplit/Hangul_StrCut` 을 한 줄씩 대응시켜 이식. 세션별 잔여 버퍼(호출측 보관), strlen 기준·500/1000 규칙·마지막 STX~첫 ETX 그대로. 추가: `StrSplit` 경계 검사(원본 50/1024 오버플로), `Hangul_StrCut` 짝 없는 선행 바이트 종료, `Framing_BuildAlarmPush`(6770 송신 프레임). onlicDefine.h 없이 단독 컴파일 가능
- `test/framing_sim.py`: 원본 알고리즘 Python 참조 모델. 케이스 15 + UDP + 한글 절단 = 벡터 17개 생성(`test/framing_vectors.txt`). `test/framing_test.cpp` + `test/Makefile`: Rocky 9 에서 같은 벡터로 C++ 검증(PASS/FAIL)
- 참조 모델로 확인한 원본 동작(호환 유지): ① STX 는 있고 ETX 가 없는 잔여가 **정확히 500 바이트 이상이면 폐기**(`etx_missing_500`) ② 잔여+신규 ≥1000 이면 잔여 폐기 ③ 청크 안의 NUL 이후 바이트 무시 ④ `STX aaa STX bbb ETX` → `bbb` ⑤ 빈 프레임 `STX ETX` 도 콜백(payload "") → 5단계 ServerProc 에서 원본처럼 처리
- `onlicDefine.h` 에 `Framing.h` 포함

### 구현 5단계 — 소켓·워커·업무 로직 (추가)
- `SocketThread.h/.cpp`(454줄): 리슨(INADDR_ANY, SO_REUSEADDR), 포트별 select 루프(6321/6770 공통 함수, UDP 별도), 세션 리스트 `CListObject` + 뮤텍스, recv(500) 후 `Framing_OnReceive` → 6321 은 워커 큐, 6770 하트비트 `'3'` 는 인라인 ACK, 6321 유휴 타임아웃(B6, 0 이면 원본처럼 유지), `AlarmUser_Send`(1000 바이트 절단·전 세션 푸시·로그)
- `RecvWaitStack.h/.cpp`(218줄): onmw CAgtRecvWait 를 일반화한 `CRecvWait`(mutex+cond+링 2000+vector, 가득 차면 드롭 로그). 라이선스 워커 1·이벤트 워커 1, 워커마다 DB 핸들(LIC/ATALK/SMS)
- `HttpWaitStack.h/.cpp`(344줄): AlramWeb.cpp 큐·50ms 폴링 그대로, 전송만 libcurl(타임아웃 5초, 응답 코드 로그). CP949→UTF-8(iconv)→urlencode(UrlEnDe). `AlarmWebTh_Push/EmailPush`(제목·본문 기본값 8종 원본 동일)
- `SockProcLic.cpp`(52줄): ServerProc 6321 분기 이식. `IP:` 위치>3 → IPCS_LOG INSERT → lic_access_list count → raw Run/Close, 소요시간 로그
- `AlarmProc.h/.cpp`(629줄): ServerUdpProc nType 30종 분기·ACK 규칙·긴급 플래그·템플릿 코드 전부 원본 라인 대조 이식, OnCheckLinkData(타이머 스레드 전용 DB 핸들, 최초 1회 기준시각 초기화), GetUserString/GetUserInfoIdx/IsSmsExclude/PauseCheckProc/IVRSMSSendProc. 알람 문구 29종은 `AlarmText()` 테이블(기본값 = 원본 리터럴, ini `[ALARM_MSG]` 로 교체 가능, %s 개수 불일치 시 무시)
- 패치: `struct_def.h`(SOCK_RECV_WAIT 에 handle/local_port/cmd), `onlicDefine.h`(헤더 포함), `SystemLoad.cpp`(LoadSocketProc 구현, 기동 시 cc_update_time 초기화, AlarmProc_Init), `onlic.ini.sample`([ALARM_MSG])
- 원본과 다른 점(호환 무관): _ATALK/#else 컴파일 분기 → `[ATALK] USE`/`[SMS] USE` 런타임; 신규 사이트 경로 newSite001 수신자를 방금 복사한 기본 수신자로(원본은 이전 사이트 잔존 배열 사용 - 결함); 6321 세션 유휴 타임아웃
- 확인한 원본 세부: _ATALK 빌드의 알람 문자 절단은 **1024** 바이트(80 은 tableCheck 경로와 SMS 경로만) → 프로토콜명세 §7.3 의 "≤80" 표기 정정 필요; ACK 없는 경로 5종 유지
- 전체 `Source/onlic/{include,source}/onlic` 4,544 줄. EUC-KR+CRLF, 중괄호 균형, 라이브러리 API 참조 확인. **컴파일 미검증**

### Makefile_64bit (추가)
- 사용자 요청으로 onmw `linux/Makefile_64bit_curl` 과 같은 형식의 `Source/onlic/linux/Makefile_64bit` 작성(명령 나열식, `all:`/`onlic:`/`clean:`/`distclean:`). 차이: `-D_UTF8` 제외, `-D_CURL -lcurl` 항상, `rm -f *.*` → `rm -f *.o`(Makefile 삭제 방지), 앱 링크 시 소스 뒤에 `-l` 나열(링크 순서). 기존 변수식 `Makefile` 은 `Makefile_64bit` 로 위임하는 래퍼로 교체

### SMS Agent 결정 변경 (추가)
- 사용자: 지금은 SMS Agent 불필요, 차후 별도 설치 후 ODBC 설정으로 사용 → 코드 수정: `[SMS] USE=0` 이면 SMS DB 접속 안 함(`SystemLoad.cpp`, `RecvWaitStack.cpp`), IVR 200/201 은 로그만(`AlarmProc.cpp`). `tcssdk` 이관 제외. 확인결정사항 B5·DB스키마 §4 갱신
- 이관 대상 DB 백업 현황: 최신 백업 없음(2017 .bak 만). 권고 pgloader 직접 이관 또는 bcp CSV. 사용자 결정 대기

### 통합 PostgreSQL 스키마 생성 (추가)
- 44 `Nasa_License` 전체 카탈로그(컬럼·PK·인덱스) 읽기 조회 보완(`server/db/44_05_full_schema.txt`), 7 의 인덱스 키 컬럼·FK(`08_indexes_fk.txt`)
- `gen_pg_schema.py`(스크래치)가 두 덤프를 파싱해 `Source/onlic/db/` 생성: `00_create_db.sql`, `01_nasa_license_schema.sql`(19개 테이블, identity→GENERATED BY DEFAULT, smalldatetime→timestamp(0), getdate()→now(), PK/FK/원본 인덱스 + 권고 인덱스 6개), `02a_pgloader_from_190_7.load`(알람 테이블 + IPCS_LOG staging), `02b_pgloader_from_190_44.load`(라이선스 7개), `03_post_migrate.sql`(IPCS_LOG 병합, setval 16개, 건수 검증), `README.md`
- 두 서버 정의 차이: `nasa_lic.nl_bit`(44 에만) 포함, `nasa_security` 는 44 기준(PK 있음), 나머지 OnAlarm 웹 테이블은 7 기준. 식별자 전부 소문자(코드 SQL 무변경)
- 로컬 PostgreSQL 없음 → DDL 은 Rocky 9 에서 `psql -f` 로 검증

### 데이터 INSERT 파일 생성 (추가)
- 사용자 요청(스키마 생성 쿼리 + 데이터 인서트 쿼리를 파일로): `Source/onlic/db/data/10~13_*.sql` 생성. bcp.exe 로 두 서버 19개 테이블 SELECT 추출(원본 무변경) → UTF-8 multi-row INSERT. 44: 라이선스 6테이블 + IPCS_LOG 326,243 / 7: 알람·OnAlarm 12테이블 + IPCS_LOG 1,986(identity 재부여). 13 은 setval·ANALYZE·건수 검증
- bcp -c 가 빈 문자열을 0x00 으로 내보내는 점(NULL 은 빈 필드)을 처리, 제어문자 제거(PG 는 0x00 불가). `IPCS_LOG.LOG_IP` 의 깨진 바이트 1.2만 행은 U+FFFD — 판정은 LOG_REAL_IP 라 무관
- `Source/onlic/db/data/` 는 .gitignore (전화번호·db_pwd 포함). README 에 psql 적용 순서 추가. 중간 파일 `server/db/export/`(git 제외)

### 절체용 추출 스크립트 + 멱등화 (추가)
- `scripts/db_export_mssql_to_pg.py` 신규: ini(`scripts/db_export.ini`, git 제외, `.sample` 커밋)로 소스 2개 정의, 실행 시점 INFORMATION_SCHEMA 로 컬럼 조회, bcp SELECT 추출 → `Source/onlic/db/data/<타임스탬프>/` 에 INSERT 파일 + `_report.txt`(원본 건수 vs 생성 건수, 불일치 시 종료코드 1). `--dry-run`, `--source`, `--tables`, `--out`
- 사용자 요구 반영(중복 실행 무방): 스키마 `01_*.sql` 은 `CREATE TABLE/INDEX IF NOT EXISTS` + FK 존재 검사(`DO $$`). 데이터 파일은 선두 `TRUNCATE TABLE …`(같은 파일의 테이블을 한 문장에 → FK 안전) 후 INSERT. 7 의 `IPCS_LOG` 는 append 모드(임시 테이블 + `NOT EXISTS`)라 재실행해도 중복 없음
- 실행 확인: 44·7 전 테이블 src=inserted OK (IPCS_LOG 326,243 / 1,986), 29.6초. 이전 수동 추출 파일 4개와 `server/db/export/` 삭제(스크립트 산출물로 대체)
- 버그 수정 2건: ini 경로의 `@`,`` 가 escape 로 깨짐 → `/` 사용, 상대 `dir` 을 CWD 기준으로 풀어 잘못된 위치에 생성 → ini 위치 기준으로 수정. 스키마 생성기가 README 를 덮어쓰던 것도 차단

### 배치 파일 (추가)
- `scripts/db_export.bat`(CP949, CRLF): ini 존재 확인 → py/python 탐색 → `db_export_mssql_to_pg.py` 실행(인자 그대로 전달) → 종료코드별 안내 → pause. `--dry-run` 으로 동작 확인(44 IPCS_LOG 326,249 등 전 테이블 조회 성공)

### 산출물 위치 변경 (추가)
- 사용자 요청: 추출물을 실행 파일 하위 `scripts/<YYYYMMDD_HHMMSS>/` 에 생성. ini `[output] dir = .`(ini 위치 기준), 스크립트 기본값·bat 안내·README 갱신, `.gitignore` 에 `scripts/########_######/` 추가. 기존 `Source/onlic/db/data/20261013_165211/` 은 그대로 둠(필요 없으면 삭제)

### 추출 폴더에 적용 스크립트 생성 (추가)
- 사용자 요청: 추출 폴더 안에 psql 적용 명령 파일 → `apply.sh`(역할 A/B 인자, `set -e`, ON_ERROR_STOP) + `apply_commands.txt` 자동 생성, `00/01` 스키마 파일도 폴더에 복사(폴더만 서버로 옮기면 됨). ini `[output] schema_dir/pg_user/pg_db/alarm_sources` 추가
- 스크립트를 Write 로 전면 재작성(heredoc 으로 넣은 `\n` 이 실제 개행으로 바뀌는 문제 → 이후 백슬래시 포함 파일은 Write 도구 사용). `--tables` 는 각 소스의 테이블 목록과 교집합만 추출(없으면 건너뜀). bat 에서 쉼표 목록은 따옴표 필요
- 소규모 실행으로 폴더 구성 확인 후 테스트 폴더 삭제

### 추출 스크립트 네트워크 순단 대응 (추가)
- 사용자 실행에서 44 번 COUNT 조회 중 `TCP Provider: 응답 없음 / Login timeout` 1회 발생(연결 자체는 정상, 순단). 수정: sqlcmd/bcp 실패 시 5초 간격 3회 재시도(`run_with_retry`), 테이블별로 접속하던 COUNT 를 소스당 1회 UNION ALL 쿼리로 통합(접속 20회→2회), 로그인 타임아웃 15→30초. dry-run 재확인 정상

### 일일 요약·커밋 (추가)
- `docs/작업요약_2026-10-13.md` 작성(초기화→분석→결정→운영 조사→구현 5단계→DB 이관 준비→다음 할 일). deep-analyze HTML 의 DB 비밀번호·sender key 마스킹 후 커밋 대상 전체 비밀 스캔 통과(원본 Windows 소스의 하드코딩은 기존 커밋 그대로). git 커밋·푸시 (사용자 요청)

### VS2010 솔루션 (추가)
- 사용자 요청(onmw 참조): `Source/onlic/win32/onlic/onlic.sln`(Format 11.00) + 라이브러리 5개 vcxproj/filters 는 onmw `win32/NcMiddleVC90` 에서 복사(GUID·상대경로 동일 깊이), `onlic/onlic.vcxproj(.filters)` 신규(소스 12·헤더 14·설정/DB 8, 필터 3개, MultiByte, `_DB_TYPE_POSTGRES;_SOCK_SELECT`, 라이브러리 ProjectReference)
- 검증: 참조 경로 171개 존재, XML 정상, VS2019 MSBuild 로 로드(/pp) 및 ShareLib 빌드 성공(onmw 와 같은 MSB8012 경고). onlic 앱은 Linux 전용 코드라 Windows 빌드 불가(열람·편집용). 빌드 산출물 삭제, .gitignore 추가

### 다음 할 일
- **Rocky 9 에서 빌드·테스트**: `cd linux && make` → 컴파일 오류 수정 → `cd test && make && ./framing_test` → `bin/onlic -l -c ../cfg/onlic.ini` 포그라운드 기동 → 시뮬레이터(라이선스 요청·UDP 이벤트·6770 뷰어)로 동작 확인
- 문서: 프로토콜명세 §7.3 알림톡 문구 길이 1024 정정, 리눅스참조구조 §2 매핑표를 실제 파일명으로 갱신, `docs/기능목록.md` Z 체크리스트로 verify
- 구현 5단계: `SocketThread.h/.cpp`(리슨·select 루프·세션 리스트, onmw Tcp_CreateServerSocket/Udp_CreateServerSocket 패턴, 6321 유휴 타임아웃), `RecvWaitStack.h/.cpp`(라이선스 워커 1·이벤트 워커 1·HTTP 워커 1, onmw WaitStack 패턴), `SockProcLic.cpp`(ServerProc:675-761 이식), `AlarmProc.cpp`(ServerUdpProc:783-1295, OnCheckLinkData:1425-1535, GetUserString, IsSmsExclude, PauseCheckProc, IVRSMSSendProc, AlarmUser_Send), `HttpWaitStack.cpp`(libcurl 웹푸시/메일, UrlEnDe)
- 구현 4단계: `Framing.h/.cpp` — Windows `ReadMsgProc/SearchSocketSTX_ETX/CutSocketSTX_ETX/StrSplit/Hangul_StrCut` 을 세션별 버퍼로 이식 + `test/` Python 시뮬레이터로 바이트 규칙 검증(500/1000 경계, 마지막 STX~첫 ETX, strlen 절단)
- 구현 3단계: `DirectODBC.h/.cpp`(onmw CDirectODBC 핵심만: 3.x API, DSN 연결, `set client_encoding`, OpenTable/Fetch/GetData/CloseTable, ExecSql, RowCount/GetValue/GetUnsignedIntValue, lazy reconnect + 재실행, 바인딩 또는 이스케이프) + `DbLic.h/.cpp`(Windows CDirectODBC 의 GetSmsSiteUsersProc/GetSmsBasicUsersProc/GetCCLinkInfoProc/GetEMailUserProc(16컬럼)/GetSmsExcludeProc 이식) + `SystemDBConnectProc`·`SmsExcludeLoadProc` 구현
- 구현 2단계: `include/onlic/struct_def.h`(DB 구조체 5종, 버퍼 확대) + `onlicDefine.h`(포트·nType enum·상수) + 앱 골격(`onlicmain.cpp` getopt/daemon/signal/PID, `SystemLoad`, `ConfigLoad`)
- 구현 착수: (1) ~~onmw 5개 라이브러리 복사~~ 완료 + Makefile 복제, `_UTF8` 분기 점검 (2) `struct_def.h`(DB 구조체 5종, 버퍼 확대) (3) DbCore(CDirectODBC PG) + 바인딩 (4) Framing(STX/ETX 재조립) + 단위 테스트 (5) 소켓 스레드·WaitStack 워커·타이머 (6) PG DDL 스크립트(DB스키마 §6 + tcssdk `SDK_SMS_SEND` + `nasa_lic*`) 및 MSSQL→PG 이관 스크립트
- 구현 중 병행 확인: A3·A4·A6·A8·A9·A10·B13, A11 IP 등록·NcLicense 도구가 A의 PG를 쓰도록 변경 가능한지, A11 IP 등록·NcLicense 도구의 PG 이전, A2 전환 순서, A7 SMS 범위, A5 빌드 환경, B2 DB 장애 응답
- ~~B1 결정 근거 확보~~ (해소) — 이하 구 항목:
- **B1 결정 근거 확보**: 44·7에 테스트 라이선스 요청 1건씩 보내 `Run`/`Close` 확인(사용자 승인 필요, IPCS_LOG 1행) 또는 서버 `log\SOFT-*.log` 확인
- A7 답변 대기(SMS 범위·Agent PG 호환), A2(44 역할·전환 후 폐기), A5(빌드 환경)
- 사용자 답변 대기: `docs/확인결정사항.md` 나머지 우선순위 1 항목
- **`lic_access_list` 부재 원인 확인**: 190.7 `TrsSocketServer.exe` 파일 날짜/버전 vs 소스, `log\SOFT-*.log`의 6321 응답. 필요 시 테스트 MAC 1건 송신(사용자 승인 필요, IPCS_LOG 1행 남음)
- `onpia_check` PostgreSQL(203.239.159.134:39999, `nasacti`) 접속해 `MTS_ATALK_MSG` DDL 확인 (사용자 승인 시)
- 사용자 검토: `docs/리눅스참조구조.md` (프로젝트명 `onlic` 가칭, 워커 1개 설계, 라이브러리 복사 방식)
- 사용자 결정: `docs/포팅영향분석.md` §8.1 추가 정보 (인코딩 CP949 유지 권고, 결함 호환/수정 여부, `onpia_sms` DBMS, MSSQL 유지, 죽은 코드 제외 확정)
- 결정 후 Linux 포팅 설계: 플랫폼 독립 로직 추출 → epoll 루프/unixODBC/libcurl/iconv 계층 설계 → CMake 빌드
- `NASA_License` 이관 범위 결정(6개 테이블만 vs 전체 19개, OnAlarm 웹 동기화)
- Linux 판 설계 반영: DB 장애 시 6321 `Close` 응답 금지(무응답), 6321 유휴 세션 타임아웃, 6770 heartbeat 미수신 세션 정리
- 운영 PostgreSQL 배치 결정: 159.134 기존 PG(39999)에 이관 vs Linux 로컬 PG
- 포팅 후 `docs/기능목록.md` Z 체크리스트로 `verify-refactor` 수행
- git 커밋은 사용자 요청 시 (현재 미커밋: 분석 산출물 8개 + 정정 4개)

---

## [2026-10-13] 프로젝트 초기화

### 작업 내용
- 폴더 구조 생성: `docs/`, `scripts/`, `tests/`, `logs/`, `_backup/`
- `.gitignore`, `CLAUDE.md`, `README.md`, `docs/설계문서.md`, `docs/work-log.md` 작성
- `scripts/build.ps1` (MSBuild 빌드 스크립트) 작성
- git init + 첫 커밋 `1984b4a` (`core.autocrlf=false`로 CRLF 보존), `main` 브랜치
- GitHub 원격 추가 및 푸시 완료: `https://github.com/jongkwi/TrsSocketServer.git` (origin/main)
- 기존 소스 현황 파악: `Source/TrsSocketServer_Lic_VS2019_Atalk/` — MFC 다이얼로그 앱, cpp 11 / h 14, 약 6,100줄
  - 클래스 9개 (App, Dlg, ListenSocket, ClientSocket, UdpSocket, DirectODBC, AlramWeb, SystemTray, URLEncode)
  - 인코딩: cpp/h 다수 EUC-KR + CRLF (AlramWeb, DirectODBC, TrsSocketServer, TrsSocketServerDlg, MyDefine.h, resource.h, StdAfx.h, UdpSocket.h, AlramWeb.h)

### 결정 사항
- 프로젝트 성격: **기존 C++ 소스 분석/유지보수** (Python 템플릿 구조는 적용하지 않음 → requirements.txt, config.yaml, src/, cli/ 생략)
- 버전 관리: **SVN 유지 + git 병행(GitHub origin)**. `.svn/` git 제외. SVN에는 `Source/`만 등록된 상태(rev 1)
- 첫 커밋 메시지는 사용자 제공 스니펫의 "first commit" 대신 프로젝트 규칙 `[add]` 형식 적용
- EUC-KR 파일 수정 시 Edit/Write 금지, Python cp949 처리 + `_backup/` 백업 필수 (CLAUDE.md에 명시)

### 다음 할 일
- `deep-analyze-c` 또는 `analyze-c` 스킬로 소스 전체 분석 리포트 생성
- `docs/설계문서.md` 4절(데이터 흐름), 6절(미확인 항목) 채우기
- `scripts/build.ps1`로 실제 빌드 가능 여부 확인 (VS2019 설치 환경)

---
