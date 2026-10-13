# 작업 로그

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
