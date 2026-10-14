# DB 스키마 · ODBC 연결 정보

소스의 SQL 문 전수(51건)를 **운영 MSSQL(203.239.190.7, 읽기 전용 조회, 2026-10-13)** 의 실제 카탈로그·데이터와 대조해 정리했다. 조회 원본은 `server/db/00~07_*.txt` (git 제외, 전화번호 마스킹).
PostgreSQL(`onpia_check`)은 아직 접속하지 않았으므로 §3은 코드 기준 추정이다.

표기: `[확인]` 운영 DB 또는 소스에서 직접 확인 / `[추정]` 코드 사용 방식에서 유추 / `[미확인]` 근거 없음

---

## 1. ODBC 연결 정보

### 1.1 DSN 3개 (`TrsSocketServerDlg.cpp:216-224`, START_TIMER 1회 실행)

| 멤버 | DSN | UID | PWD | 실제 DBMS | DB명 | 근거 |
|---|---|---|---|---|---|---|
| `m_dlgSql` | `ipcs_log` | `lic` | `***` | MSSQL (서버별로 **내용이 다른 DB**) | 190.44: `Nasa_License` = **라이선스 DB**(`lic_access_list`, `IPCS_LOG` 32.6만 행, `nasa_lic`) / 190.7: `NASA_License` = **알람 DB**(`CallCenter_Link`, `SITE_*`, `IPCS_LOG` 1,986행) | 두 서버 모두 운영 접속 `[확인]` |
| `m_pAtalkSql` | `onpia_check` | `nasacti` | `***` | PostgreSQL | (미확인) | `nextval()`, `now()`, `"MTS_ATALK_MSG"`, netstat 203.239.159.134:39999 `[확인]` |
| `m_smsSql` | `onpia_sms` | `lic` | `***` | MSSQL (동일 인스턴스) | `tcssdk` | 운영 접속 `[확인]`. **`SDK_SMS_SEND` 0행, 보관 테이블은 2019-04까지 → 미사용 확정** |

- 190.7 `lic` 로그인: 기본 DB `master`, sysadmin 아님, `NASA_License` db_owner → DSN이 DB를 지정해야 함. 190.44 `lic` 로그인: 기본 DB `Nasa_License`, 활성(`is_disabled=0`) → 앱 로그인은 정상으로 보임(사용자 '로그인 안됨'은 SSMS 접속 문제로 추정). 조회에 쓴 44 계정 `lic2`는 sysadmin
- 190.44 인스턴스: **SQL Server 2012 SP1 Developer Edition x86, Windows Server 2008 SP2(NT 6.0)** — OS·DB 모두 지원 종료. 포트 34333(`MSSQL44` 명명 인스턴스)
- 비밀번호는 소스에 평문 하드코딩. Linux 포팅 시 `cfg/*.ini`로 외부화하고 커밋 금지 (onmw 규칙과 동일)
- 앱은 Win32 빌드이므로 **32-bit ODBC 관리자**(`C:\Windows\SysWOW64\odbcad32.exe`)의 DSN을 사용

### 1.2 `CDirectODBC` 호출 방식 (`DirectODBC.cpp`)

| 항목 | 구현 | 라인 | Linux(unixODBC) 영향 |
|---|---|---|---|
| API 세대 | ODBC 2.x (`SQLAllocEnv`/`SQLAllocConnect`/`SQLAllocStmt`/`SQLFreeStmt(SQL_DROP)`) | `DirectODBC.cpp:105-113`, `DirectODBC.cpp:239-256` | unixODBC에서도 동작하나 deprecated. onmw `CDirectODBC`처럼 3.x(`SQLAllocHandle`, `SQLSetEnvAttr(SQL_ATTR_ODBC_VERSION)`)로 교체 |
| 연결 | `SQLConnect(DSN, UID, PWD)` — 연결 문자열 아님 | `DirectODBC.cpp:113` | `odbc.ini` DSN 방식 유지 가능 |
| 로그인 타임아웃 | `SQLSetConnectOption(SQL_LOGIN_TIMEOUT, 30)` | `DirectODBC.cpp:110` | 동일 (`SQL_ATTR_LOGIN_TIMEOUT`) |
| 쿼리 타임아웃 | 없음 | - | `SQL_ATTR_QUERY_TIMEOUT` 추가 권고 (6321 응답 5초 제한) |
| 오토커밋 | 설정 안 함 → 드라이버 기본 **ON** | - | PG에서 오토커밋 상태의 `COMMIT`은 `WARNING: there is no transaction in progress` (무해) |
| 명시적 COMMIT | `ExecSql` 성공 후 매번 `ExecSql("COMMIT")` 재귀 호출 | `DirectODBC.cpp:259` | 오토커밋 유지 시 제거 가능. `_DB_TYPE_POSTGRES` 분기에서 생략 |
| 실행 | `SQLExecDirect`, 매 호출마다 stmt 할당·해제 | `DirectODBC.cpp:247`, `DirectODBC.cpp:256` | 동일 |
| 결과 읽기 | `SQLFetch` + 컬럼별 `SQLGetData(SQL_C_CHAR/SQL_C_LONG)` | `DirectODBC.cpp:365`, `DirectODBC.cpp:497-502`, `DirectODBC.cpp:563-566`, `DirectODBC.cpp:622-626`, `DirectODBC.cpp:709-723`, `DirectODBC.cpp:814-818` | 동일. 버퍼 길이가 컬럼 최대 길이보다 짧은 곳은 §2에서 표기 |
| 재접속 | stmt 할당/실행 실패 시 `ConnectODBC()` 1회 재시도 후 **실패 반환** (재실행 없음). `RowCount`는 실패 시 **-1** | `DirectODBC.cpp:240-252`, `DirectODBC.cpp:347-381` | onmw 방식(lazy reconnect + 재실행)으로 보강 |
| SQL 조립 | `CString::Format` 문자열 결합, 이스케이프 없음 | 전체 | 바인딩(`SQLBindParameter`) 또는 `'`→`''` 이스케이프로 수정 (결정 사항 B 반영) |
| 인코딩 | CP949 바이트를 그대로 전송 (`SQL_C_CHAR`). MSSQL 정렬 `Korean_Wansung_CI_AS` = CP949 | - | PG: `set client_encoding to 'UHC'`. 드라이버는 서버에 이미 있는 PGDG `psqlodbc.so`(43 서버 `/usr/pgsql-15/lib/`) 그대로 — 2026-10-14 수정 전 onlic 빌드로 링크 장애 알림톡 한글 정상 수신 확인, isql 바이트도 CP949. ANSI 교체 불필요. `EUCKR` 은 PG 에서 KS X 1001 완성형만 지원해 CP949 확장 한글이 `22021` 오류 → UHC 로 확정 (2026-10-14) |
| 스레드 | 핸들 3개 모두 메인(UI) 스레드 전용. `AlarmWebTh_EmailPush`도 메인에서 호출 | - | Linux 설계: 스레드당 핸들(리눅스참조구조 §4) |
| 링크 | `odbc32.lib`, `odbccp32.lib`(Release만) | `vcxproj:98,137` | `-lodbc` |

### 1.3 Linux 측 DSN 구성 (제안, 결정: PostgreSQL + unixODBC)

```ini
; /etc/odbcinst.ini  (43 서버 실제 구성 2026-10-14 — onmw 와 공용, 변경하지 않음)
[PostgreSQL]
Description = psqlODBC
Driver      = /usr/pgsql-15/lib/psqlodbc.so   ; 이 드라이버로 onlic 알림톡 한글 정상 확인. ANSI(psqlodbca) 교체 불필요

; /etc/odbc.ini  (DSN 이름은 기존과 동일하게 유지하면 ini 변경 최소화)
[ipcs_log]
Driver     = PostgreSQL
Servername = 127.0.0.1
Database   = nasa_license
Port       = 39999

[onpia_check]
Driver     = PostgreSQL
Servername = 203.239.159.134
Database   = <onpia_check DB명>
Port       = 39999
```

`onpia_sms` DSN은 유지(§4, SMS Agent 신규 설치 결정). 서버 2대 각각 로컬 PG를 두므로 `Servername`은 양쪽 모두 `localhost` (리눅스참조구조 §8).

### 1.4 운영 서버 구성 (2026-10-13, 사용자 제공 DSN 목록 + `server/netstat44.txt`, `server/netstat7.txt`)

| 항목 | `lic.onpia.co.kr` = 203.239.190.44 (WIN-RA38LD3IOLC) | `lic2.onpia.co.kr` = 203.239.190.7 (WIN-2UR59F71KAE) |
|---|---|---|
| `TrsSocketServer.exe` | **ver 1.0.0.01 (2018-07-30)** — 라이선스 확인 1차 용도. 6321/6770 LISTEN, UDP 5363 바인드 | **ver 1.0.0.7 (2025-12-08)** — 알람 + 라이선스 2차. 6321/6770 LISTEN, UDP 5363 바인드. 소스 `TrsSocketServer.rc:115` `FILEVERSION 1,0,0,7`과 일치 |
| 클라이언트 접속 | **6321·6770 접속 0건** (OnAlarm.exe가 190.7:6770으로 나가는 것뿐) | 6321 ESTABLISHED 13건, 6770 12건(211.33.174.57에서 11건 + 44의 OnAlarm.exe 1건) |
| `ipcs_log` DSN | 127.0.0.1 → **로컬 MSSQL(`sqlservr.exe`, TCP 34333 비표준 포트, SQL Browser UDP 1434)**. 앱이 127.0.0.1:34333에 ESTABLISHED 1건 + 직전 포트 46개 TIME_WAIT → 재접속 반복 흔적(로그인 실패 추정, 사용자 "로그인안됨") | 127.0.0.1:1433 ESTABLISHED (`NASA_License`) |
| `onpia_check` DSN | 미등록(사용자 목록에 없음). 로컬 `postgres.exe`가 39999 LISTEN | 203.239.159.134:**39999** ESTABLISHED (외부 PostgreSQL, 비표준 포트) |
| `onpia_sms` DSN | `Onpia_SMS` → 203.239.190.7 | 203.239.190.7:1433(자기 자신) ESTABLISHED, DB `tcssdk`. **미사용 확정**(§4) |
| `onpia_alarm` DSN | `Onpia_Alarm` → 203.239.190.7 `NASA_License` (OnAlarm.exe가 사용, 1433 접속 2건) | 로컬 `NASA_License` — **이 서버 코드는 `onpia_alarm` DSN을 쓰지 않음** (알람 뷰어 OnAlarm.exe용) |
| 로컬 MSSQL 내용 | `Nasa_License`(34333): `lic_access_list` 859, `IPCS_LOG` **326,235**(최신 2026-10-13 14:00 = 조회 시각, 월 3~6천 건), `nasa_lic` 698(발급 도구가 2026-10-02까지 사용), `nasa_lic_log` 1,947, `CALLBOX_LOG`/`SOFTPHONE_LOG`(2015년 이전 잔재). **`CallCenter_Link`·`SITE_*` 없음** | `NASA_License`(1433): `CallCenter_Link` 2,049, `SITE_*`, `IPCS_LOG` 1,986(2차 폴백 유입분), `nasa_lic` 79(2018년 이후 미갱신 사본), OnAlarm 웹 테이블. **`lic_access_list` 없음** |
| 기타 프로세스 | `NcLicense.exe`(로컬 MSSQL 34333에 3접속 — 라이선스 파일 발급 도구, `nasa_lic` 테이블 사용 추정), `OnAlarm.exe`(알람 뷰어), `NcrCheck.exe`, MySQL 3306, Apache/ruby | `McsAgentJ.exe`/`McsMonJ.exe`(MSSQL 로컬 접속), FileZilla, `NcrCheck.exe` |

해석
- **실질 운영 서버는 190.7(lic2)** 이다. UDP 송신자(OnDemon·ncbk)는 `lic2`만 쓰고, 알람 뷰어도 190.7:6770에 붙어 있다. 사이트 마스터 `NASA_License`의 원본도 190.7 로컬 MSSQL이며 `cc_update_time` 최대값이 조회 시각과 일치(링크 감시 가동 중)
- 190.44(lic)는 라이선스 체크 1차 접속지(`lic.onpia.co.kr`, 클라이언트는 lic **connect 실패 시에만** lic2로 폴백)인데 6321 접속이 0건이다. 44의 DB 로그인이 실패 중이면 `RowCount`가 -1 → 서버가 `Close`를 보내 클라이언트가 종료(`KillLib 6`)되는 경로이므로, Linux 전환 시 **DB 장애를 `Close`로 응답하지 않도록** 수정 대상
- 190.7의 6321 ESTABLISHED 13건은 전부 외부 스캐너류 주소(147.185.133.x 등)다. 서버가 세션을 닫지 않아 누적된 것이며 정상 클라이언트는 응답 수신 후 스스로 닫는다 → Linux 판에 **유휴 타임아웃(예: 30초) 추가 가능**(호환 영향 없음)
- 6770에 211.33.174.57에서 11세션이 열려 있다 → 뷰어 재접속 시 이전 세션 미종료 가능성. 포팅 후 heartbeat 미수신 세션 정리 검토
- 새 Linux 서버의 PostgreSQL 배치 후보: 159.134의 기존 PG(39999)에 `NASA_License` 이관 스키마를 함께 두거나, Linux 서버 로컬 PG. `onpia_check`는 포트 39999 유지

### 1.5 라이선스 판정 경로 — 두 서버의 DB 역할 분리 `[확인, 2026-10-13 해소]`

| | 190.44 (1차, `lic.onpia.co.kr`) | 190.7 (2차, `lic2.onpia.co.kr`) |
|---|---|---|
| exe | 1.0.0.01 (2018, 소스 없음) | 1.0.0.7 = 현재 소스 |
| `lic_access_list` | **있음** 859행(전부 `'Y'`, 2026-09-07까지 추가 중) | **없음** |
| `IPCS_LOG` | 326,235행, **지금도 매시간 기록**(2026-10-13 14:00) | 1,986행 (44 connect 실패 시 폴백 유입분만) |
| 결과 | 정상 `Run`/`Close` 판정 | `SELECT count(*) FROM lic_access_list` 실패 → `RowCount` -1 → **항상 `Close`** |

- 즉 라이선스 체크는 사실상 44 단독으로 돌고 있고, 44가 죽어 7로 폴백되는 순간 클라이언트는 `Close`를 받아 종료된다(7의 `IPCS_LOG` 반복 행이 그 흔적). 앞선 "배포 exe가 소스와 다르다"는 가설은 **철회** — 7은 소스와 같고 DB가 다를 뿐
- 44의 netstat에 6321 접속이 0건이었던 이유: 요청이 1초 내 끝나는 단발 세션이라 캡처 시점에 없었을 뿐
- Linux 포팅은 **두 서버의 역할을 하나로 합친다**: 라이선스(`lic_access_list`, `IPCS_LOG` 대용량) + 알람(`CallCenter_Link`, `SITE_*`). 두 DB는 이름만 같고 내용이 다르므로 PG 복원 시 **44의 `Nasa_License`와 7의 `NASA_License`를 하나의 DB로 병합**해야 한다 (`IPCS_LOG`는 UNION, `nasa_lic`은 44 기준)
- 주의: `lic_access_list`에 IP를 추가하는 주체(NcLicense.exe 또는 다른 관리 도구)와 `nasa_lic`를 쓰는 NcLicense.exe(Windows)가 44 MSSQL을 직접 쓴다 → 44 폐기 시 이 도구들의 DB 대상도 PG로 바꿔야 함(확인결정사항 A11)

---

## 2. `ipcs_log` = `NASA_License` (MSSQL) — 운영 DDL `[확인]`

DB에는 19개 테이블이 있고 이 서버가 쓰는 것은 6개(+ 존재하지 않는 `lic_access_list`). 나머지(`CallCenter_Link_Reference`, `enterprise`, `project`, `server`, `on_alarm_user`, `SITE_AUTH`, `nasa_lic`, `nasa_lic_log`, `nasa_security`, `CALLBOX_LOG`, `SOFTPHONE_LOG`, `stc_calllist`)는 OnAlarm 웹·NcLicense 등 다른 도구의 것이며 **포팅 범위 밖**(단, 같은 DB를 공유하므로 PostgreSQL 이관 시 함께 옮길지 결정 필요).

### 2.1 `CallCenter_Link` — 사이트 마스터 (2,049행, cc_use='Y' 606행, `'신규1'` 897행)

| # | 컬럼 | 운영 타입 | NULL/기본값 | 코드 사용 | 값 형식 / 의미 | 코드 버퍼 |
|---|---|---|---|---|---|---|
| 1 | `cc_index` | bigint identity **PK** | NOT NULL | R | | `SQL_C_LONG`(int) — bigint를 int로 읽음, 현재 최대 22413이라 문제 없음 |
| 2 | `cc_name` | varchar(256) | `''` | R/W | 신규 등록 시 `'new_site'` | 51 (**짧음**) |
| 3 | `cc_site_name` | varchar(256) | `''` | R/W | 알람 문구의 사이트명. 신규 `'신규1'` → 운영자가 수동 변경. `'신규1'`+3시간 경과면 알람 억제 | 256 |
| 4 | `cc_ip` | varchar(50) | NOT NULL `''` | R/W | 사이트 시스템(사설) IP | 51 |
| 5 | `cc_real_ip` | varchar(50) | NOT NULL `''` | R/W | 공인 IP (UDP 송신 peer) | 51 |
| 6 | `cc_use` | char(1) | `'Y'` | R | `'Y'`만 처리 대상 | - |
| 7 | `cc_insert_time` | datetime | `getdate()` | R | `DATEDIFF(hour, cc_insert_time, getdate()) > 3`, `IS NOT NULL` | - |
| 8 | `cc_update_time` | **varchar(256)** | `''` | R/W | `YYYYMMDDHHMMSS` 14자(1,828행) 또는 `''`(221행). `<=` 문자열 비교로 21분 판정 | - |
| 9 | `cc_link_error` | varchar(1) | `'N'` | R/W | `'N'` 정상 / `'Y'` 링크 장애 (현재 Y 1,443행) | - |
| 10 | `cc_pause` | varchar(50) | `''` | R/W | `YYYYMMDD`까지 문자 일시정지(98행 설정), 경과·오류 시 `''` | `GetValue` 21 |
| 11 | `cc_domain` | varchar(50) | NULL | - | 다른 도구용 | |
| 12 | `cc_memo` | text | NULL | - | 다른 도구용 | |
| 13-18 | `db_user`, `db_pwd`, `db_port`, `db_name`, `db_type`, `db_ip` | varchar | NULL | - | 사이트 DB 접속정보(7행 입력, OnAlarm 웹용 추정). **평문 비밀번호 보관 → 이관 시 주의** | |
| 19 | `cc_nas_failcnt` | int | `0` | R/W | NAS 연결 실패 누적(최대 999), 10회 이하만 알람. 271행 >0 | `GetUnsignedIntValue` |
| 20 | `cc_nasupload_fail` | varchar(128) | `''` | R/W | 소스는 `YYYY-MM-DD`를 쓰지만 운영 데이터는 `20240822` 형식(8자) → **다른 버전 exe가 기록 중**(§1.5 근거) | `GetValue` 21 |

인덱스: `CallCenter_Link_pk`(cc_index, nonclustered unique), `ClusteredIndex-20190116-151630`(비고유 클러스터드, 키 컬럼 미확인). `cc_ip`+`cc_real_ip` 조회에 쓸 인덱스 없음.

### 2.2 `IPCS_LOG` — 라이선스 접속 로그 (190.7: 1,986행 / **190.44: 326,235행**, 2016-03 ~ 현재, 2023년 이후 연 6~10만 건)

| # | 컬럼 | 운영 타입 | NULL/기본값 | 코드 사용 | 값 |
|---|---|---|---|---|---|
| 1 | `lic_index` | bigint identity | NOT NULL | - | PK 구성원 |
| 2 | `lic_insert_date` | smalldatetime | NOT NULL `getdate()` | - | PK 구성원 (`PK_IPCS_LOG` = lic_index + lic_insert_date) |
| 3 | `log_data` | varchar(200) | NULL | W | 클라이언트 MAC 12 hex (코드는 `LOG_DATA`로 씀, CI 정렬이라 무관) |
| 4 | `LOG_IP` | varchar(50) | NULL | W | 프레임 내 `IP:` 뒤 사설 IP |
| 5 | `LOG_TYPE` | varchar(10) | NULL | W | `'IVR'` / `'VRS'` |
| 6 | `LOG_REAL_IP` | varchar(50) | NULL | W | peer 공인 IP |

두 서버의 DDL은 동일. 같은 MAC·시각이 2~4행씩 반복 기록됨(44에서는 한 VRS 사이트가 **매시간 4회** 요청 — 로컬 `.lic`가 계속 무효인 사이트) → PK가 `lic_index` 포함이라 충돌 없음. PostgreSQL에서는 컬럼명이 소문자로 접히므로 `log_data`/`log_ip` 혼용 문제 없음. 이관 시 32.6만 행 + 월 3~6천 건 증가 → `lic_insert_date` 인덱스와 보관 주기 정책 권고.

### 2.3 `lic_access_list` — IP 허용 목록 (**190.44에만 존재**, 859행) `[확인]`

| # | 컬럼 | 운영 타입 | NULL/기본값 | 코드 사용 | 값 |
|---|---|---|---|---|---|
| 1 | `lic_ip_index` | int identity | NOT NULL | - | PK 구성원. 최대 26,704(삭제 이력 많음) |
| 2 | `nasa_con_ip` | varchar(50) | NOT NULL | R | peer 공인 IP 완전 일치. PK 구성원(복합 PK라 **중복 IP 36건 존재**) |
| 3 | `nasa_insert_date` | varchar(20) | NULL | - | `'Mar 29 2016  1:32AM'`(초기 일괄) 또는 `'2026-09-07 14:41:59'` 형식 혼재 |
| 4 | `nasa_use` | varchar(10) | `'Y'` | R | 현재 전부 `'Y'` |

- 분포: 203.239.159.x 213건, 203.239.190.x 66건(온피아 자체 대역 279건), 그 외 고객 대역 580건. 2026-09-07까지 신규 등록 중(등록 주체 미확인, A11)
- 190.7에는 없으므로 7로 폴백된 요청은 전부 `Close`(§1.5). PG 병합 시 44의 859행을 그대로 복사

### 2.4 `SITE_SMS_USER` — 사이트별 알림 수신자 (25,640행, 1,469 사이트)

| # | 컬럼 | 운영 타입 | NULL | 코드 사용 | 값 (운영 분포) | 코드 버퍼 |
|---|---|---|---|---|---|---|
| 1 | `sms_site_ip` | varchar(50) | NOT NULL | R/W | 사이트 IP. `'ALL'` 와일드카드 행은 **0건** | 51 |
| 2 | `sms_site_real_ip` | varchar(50) | NOT NULL | R/W | 공인 IP | 51 |
| 3 | `sms_site_name` | varchar(256) | NULL | R/W | 알람 문구 사이트명 (`m_pSmsSiteUsers[0]`) | 51 (**짧음**) |
| 4 | `sms_use` | char(1) | NOT NULL | R/W | **`'X'` 13,999 / `'T'` 11,638 / `'Y'` 3** | 2 |
| 5 | `sms_user_name` | varchar(50) | NULL | R/W | 수신자명 | 51 |
| 6 | `sms_user_phone` | varchar(50) | NOT NULL | R/W | 11자리 휴대폰(전행 11자) | 21 |

- 코드 의미: `'Y'` 일반+긴급 수신, `'T'` 긴급(USE_ALL)만, 그 외(`'X'`) 미수신. 운영에서 `'Y'`는 3행(1명, 3사이트)뿐이므로 **사실상 모든 알림이 USE_ALL 경로의 `'T'` 사용자(내부 담당자)에게만 간다**
- 신규 사이트 감지 시 `SITE_SMS_BASIC_USER`의 `'Y'/'T'` 사용자만 복사(`TrsSocketServer.cpp:1231-1234`)
- PK·인덱스 없음(힙). `(sms_use, sms_site_ip, sms_site_real_ip)` 인덱스 추가 권고(이관 시)

### 2.5 `SITE_SMS_BASIC_USER` — 신규 사이트 기본 수신자 (28행)

| # | 컬럼 | 운영 타입 | NULL | 값 (운영 분포) | 코드 버퍼 |
|---|---|---|---|---|---|
| 1 | `site_sms_index` | bigint identity | NOT NULL | | `SQL_C_LONG` |
| 2 | `site_sms_name` | varchar(50) | NOT NULL | 담당자명 | 51 |
| 3 | `site_sms_telnum` | varchar(50) | NOT NULL | 휴대폰 (1행은 대표번호 1588-xxxx) | 21 |
| 4 | `site_sms_use` | char(1) | NOT NULL | **`'S'` 11 / `'T'` 8 / `'N'` 9**, `'Y'` 0 | 2 |

코드는 `'Y'`/`'T'`만 로드하므로 **현재 신규 사이트에 복사되는 기본 수신자는 `'T'` 8명**. `'S'`는 코드에 없는 값(다른 도구 의미).

### 2.6 `SITE_EMAIL_USER` — 이메일 알림 설정 (8행, 사이트 1곳 203.239.190.54만)

조회 조건: `email_site_ip=? AND email_site_real_ip=? AND alarm_type=<nType>` (`TrsSocketServer.cpp:1867`)

| # | 컬럼 | 운영 타입 | 기본값 | 코드 버퍼 | 비고 |
|---|---|---|---|---|---|
| 1 | `idx` | bigint identity | | LONG | |
| 2 | `email_site_ip` | varchar(50) NOT NULL | `''` | 51 | |
| 3 | `email_site_real_ip` | varchar(50) NOT NULL | `''` | 51 | |
| 4 | `email_site_name` | varchar(100) | `''` | 101 | |
| 5 | `alarm_type` | int | `0` | LONG | 운영값 0,1,301~304,401,402 |
| 6 | `alarm_proc_name` | varchar(50) | `''` | 51 | 운영값 `onbk` (type 0/1) → 메시지에 `strstr` 포함될 때만 발송 |
| 7 | `email_to` | varchar(500) | `''` | 501 | 콤마 구분 복수 주소 |
| 8 | `email_from` | varchar(200) | `'support@onpia.co.kr'` | 201 | 운영 8행 모두 `''`/NULL → HTTP `cc` 비어서 전송 |
| 9 | `email_title` | varchar(200) | `''` | 201 | `[온피아54] NAS 연결 장애` 등 |
| 10-12 | `email_msg1~3` | varchar(500) | `''` | 501 | 운영 전부 NULL/`''` |
| 13 | `email_url` | varchar(2048) | `'http://203.239.190.7/'` | **읽지 않음** | **결함 B: SELECT 16컬럼 vs GetData 15개** — 13번 `email_url`이 `email_attr1` 버퍼(101)로 들어가고 이후 한 칸씩 밀림. 수정 대상 |
| 14-16 | `email_attr1~3` | varchar(100) | `''` | 101 | 운영 전부 비어 있음 |

PK·인덱스 없음. 2017 백업의 `Tmp_SITE_EMAIL_USER`는 현재 DB에 없음.

### 2.7 `SITE_SmsExcludeTime` — 시간대별 알림 제외 (4행)

| # | 컬럼 | 운영 타입 | 기본값 | 코드 버퍼 | 운영 데이터 |
|---|---|---|---|---|---|
| 1 | `sm_index` | int identity | | LONG | 1~4 |
| 2 | `sm_type` | int | NULL | LONG | 0(PROC_ERROR), 1(PROC_RECOVER) |
| 3 | `sm_msg` | varchar(200) | NULL | 201 | `o npbxlink`, `on pbxlink`, `a sterisk` — 프로세스명에 공백을 넣어 **완전 일치가 되지 않게 비활성화**해 둔 상태로 보임 |
| 4 | `sm_stime` | varchar(10) | NULL | 11 | `0000` |
| 5 | `sm_etime` | varchar(10) | NULL | 11 | `0500` |
| 6 | `sm_use` | **char(2)** | `'Y'` | 2 | `'Y '`(패딩) — 코드는 `sm_use='Y'` 비교라 MSSQL은 패딩 무시. PG `char(2)`도 비교 시 패딩 무시하나 `=` 리터럴은 `'Y'`로 유지 |

즉 현재 운영에서 실제로 제외되는 알림은 없다(메시지 불일치). 00:00~05:00 야간 억제 의도.

---

## 3. `onpia_check` (PostgreSQL 203.239.159.134:39999) — 알림톡 큐 `[추정, 미접속]`

### 3.1 `"MTS_ATALK_MSG"` (대소문자 구분 식별자, INSERT 전용) + 시퀀스 `mts_atalk_msg_seq`

INSERT 4곳(`TrsSocketServer.cpp:874,1191,1265,1510`) 모두 동일 컬럼·순서:

| 컬럼 | 타입 [추정] | 값 | 비고 |
|---|---|---|---|
| `TRAN_PR` | bigint PK | `nextval('mts_atalk_msg_seq')` | 시퀀스는 소문자(따옴표 없음) |
| `TRAN_SENDER_KEY` | varchar(40) | `'fdd97f25…'` (40자 hex, 하드코딩) | 설정 외부화 대상 `[ATALK] SENDER_KEY` |
| `TRAN_TMPL_CD` | varchar | 템플릿 코드 30종 (`procError001` 등, 프로토콜명세 §7.3). USER_MSG는 `''` | |
| `TRAN_CALLBACK` | varchar | `'15889310'` | 발신번호 |
| `TRAN_PHONE` | varchar | `sms_user_phone` | 수신자 1명당 1행 |
| `TRAN_SUBJECT` | varchar | `'OnpiaATalk'` | |
| `TRAN_MSG` | text | 알람 문구 (≤ 80B `Hangul_StrCut`) | CP949 바이트 |
| `TRAN_DATE` | timestamp | `now()` | |
| `TRAN_TYPE` | int | `5` | |
| `TRAN_STATUS` | varchar(1) | `'1'` | 대기 상태로 추정, MTS 에이전트가 소비 |
| `TRAN_REPLACE_TYPE` | varchar(1) | `'S'` | 실패 시 SMS 대체 발송으로 추정 |
| `TRAN_REPLACE_MSG` | text | `TRAN_MSG`와 동일 | |

- 실제 발송은 외부 MTS 에이전트(미확인). 포팅 시 INSERT 포맷만 유지하면 호환
- `TrsSocketServerDlg.cpp:370` `onpia_reschk_log` INSERT는 2022-03-29 주석 처리된 테스트 코드 → **포팅 제외**
- 확인 필요: `\d "MTS_ATALK_MSG"`, `\d mts_atalk_msg_seq`, DB명 (접속 계정 `nasacti`)

---

## 4. `onpia_sms` = `tcssdk` (MSSQL, 동일 인스턴스) — 현재 미사용. **결정: 지금은 불필요, SMS Agent 는 차후 별도 설치 + ODBC 설정으로 연결** (2026-10-13)

- `SDK_SMS_SEND` 0행. 보관 테이블 `sdk_sms_send_20170919`(6,563) / `_20171013`(6,774) / `_20190418`(6,667) → 2019-04 이후 발송 없음. `SDK_SMS_REPORT` 45만 행은 과거 이력
- 운영 DDL(참고): `MSG_ID bigint, USER_ID varchar(20) NOT NULL, SCHEDULE_TYPE int, SUBJECT varchar(50), SMS_MSG varchar(200), CALLBACK_URL, NOW_DATE varchar(20), SEND_DATE varchar(20) NOT NULL, CALLBACK varchar(20), DEST_TYPE int, DEST_COUNT int, DEST_INFO text NOT NULL, KT_OFFICE_CODE, CDR_ID, RESERVED1~9, SEND_STATUS/SEND_COUNT/SEND_RESULT int, SEND_PROC_TIME, STD_ID` (상용 SMS SDK 테이블). 코드 INSERT 컬럼 8개와 호환
- `_ATALK` 빌드에서 `SDK_SMS_SEND` INSERT가 컴파일되는 경로는 IVR nType 200/201(`IVRSMSSendProc`, `TrsSocketServer.cpp:1299`)뿐이며 송신자 없음(프로토콜명세 §5)
- **사용자 결정(2026-10-13, 최종)**: 지금은 SMS Agent 불필요. 차후 별도 설치 시 ODBC DSN `onpia_sms` 설정 + ini `[SMS] USE=1` 로 활성화. 그때까지 `onlic` 은 SMS DB 에 접속하지 않음(`USE=0`), `tcssdk` 이관 없음. 켤 때의 전제 조건 두 가지 — (1) SMS Agent가 PostgreSQL의 `SDK_SMS_SEND`를 읽을 수 있어야 함(Agent가 MSSQL 전용이면 `tcssdk`를 MSSQL로 남겨야 함), (2) 현재 코드에서 이 테이블에 쓰는 경로는 IVR 200/201뿐이므로 **장애 알람도 SMS로 보내려면 기능 추가**가 필요(확인결정사항 A7)

---

## 5. 포팅 결정·확인 필요 (포팅영향분석 §8.1 갱신)

| # | 항목 | 현재 상태 | 필요 조치 |
|---|---|---|---|
| 1 | `lic_access_list` | **해소**: 190.44 `Nasa_License`에 859행 존재, 190.7에만 없음 | PG 병합 시 44 데이터 복사. 6321 판정 기준은 소스 그대로(IP 허용 목록) 유지 가능 |
| 2 | `NASA_License` → PostgreSQL 이관 범위 | **결정: 전체 백업 → 신규 Linux PG 복원**. 단 **44와 7의 DB를 병합**해야 함(§1.5) | 44: `lic_access_list`, `IPCS_LOG`(32.6만), `nasa_lic`·`nasa_lic_log` / 7: `CallCenter_Link`, `SITE_*`, OnAlarm 웹 테이블. OnAlarm 웹·NcLicense·IP 등록 도구가 MSSQL에 남으면 갈라짐 → 함께 전환 확인(A4·A11) |
| 3 | `onpia_check` PostgreSQL | **결정: 기존 서버 유지, ODBC 연결** (203.239.159.134:39999, `nasacti`) | DDL 확인은 선택(INSERT 타입 검증) |
| 4 | `onpia_sms` | 현재 미사용이나 **유지 결정** (SMS Agent 신규 설치) | `tcssdk` 복원 범위(`SDK_SMS_SEND` + REPORT 계열), Agent의 PG 호환, 알람 SMS 범위(A7) |
| 5 | 데이터 이관 | 7: `CallCenter_Link` 2,049 / `SITE_SMS_USER` 25,640 / `SITE_SMS_BASIC_USER` 28 / `SITE_EMAIL_USER` 8 / `SITE_SmsExcludeTime` 4 / `IPCS_LOG` 1,986 — 44: `lic_access_list` 859 / `IPCS_LOG` 326,235 / `nasa_lic` 698 / `nasa_lic_log` 1,947 | `cc_update_time` 등 문자열 날짜는 그대로 복사. `IPCS_LOG`는 두 서버 UNION(identity 재부여). `db_pwd` 평문 컬럼 취급 결정 |
| 6 | 코드 버퍼 < 컬럼 길이 | `cc_name` 51<256, `sms_site_name` 51<256, `cc_nasupload_fail` 21<128 | `SQLGetData` 절단(SQL_SUCCESS_WITH_INFO)이라 오류는 아님. Linux 구조체는 운영 길이에 맞춰 확대 |

---

## 6. PostgreSQL DDL (운영 MSSQL 타입을 1:1 변환, 이 서버 사용 테이블만)

> **2026-10-13 갱신**: 두 서버 통합 DDL(19개 테이블 전부)과 pgloader 이관 스크립트는 **`Source/onlic/db/`** 에 생성됨 (`01_nasa_license_schema.sql`, `02a/02b_pgloader_*.load`, `03_post_migrate.sql`, `README.md`). 아래는 이 서버가 쓰는 테이블의 발췌이며 최종본은 `Source/onlic/db/` 가 기준.

```sql
-- MSSQL NASA_License → PostgreSQL (식별자는 소문자로 접힘. 코드의 CallCenter_Link 등은 그대로 동작)
CREATE TABLE callcenter_link (
  cc_index          bigserial PRIMARY KEY,
  cc_name           varchar(256) DEFAULT '',
  cc_site_name      varchar(256) DEFAULT '',
  cc_ip             varchar(50)  NOT NULL DEFAULT '',
  cc_real_ip        varchar(50)  NOT NULL DEFAULT '',
  cc_use            char(1)      DEFAULT 'Y',
  cc_insert_time    timestamp    DEFAULT now(),
  cc_update_time    varchar(256) DEFAULT '',      -- YYYYMMDDHHMMSS 문자열 비교
  cc_link_error     varchar(1)   DEFAULT 'N',
  cc_pause          varchar(50)  DEFAULT '',
  cc_domain         varchar(50),
  cc_memo           text,
  db_user           varchar(256), db_pwd varchar(256), db_port varchar(100),
  db_name           varchar(256), db_type varchar(100), db_ip varchar(100),
  cc_nas_failcnt    integer      DEFAULT 0,
  cc_nasupload_fail varchar(128) DEFAULT ''
);
CREATE INDEX ix_callcenter_link_ip ON callcenter_link (cc_ip, cc_real_ip);

CREATE TABLE ipcs_log (
  lic_index       bigserial,
  lic_insert_date timestamp(0) NOT NULL DEFAULT now(),
  log_data        varchar(200),
  log_ip          varchar(50),
  log_type        varchar(10),
  log_real_ip     varchar(50),
  PRIMARY KEY (lic_index, lic_insert_date)
);

-- 190.44 Nasa_License 원본 DDL 변환 (복합 PK 유지, 중복 IP 36건 그대로 들어옴)
CREATE TABLE lic_access_list (
  lic_ip_index     serial,
  nasa_con_ip      varchar(50) NOT NULL,
  nasa_insert_date varchar(20),
  nasa_use         varchar(10) DEFAULT 'Y',
  PRIMARY KEY (lic_ip_index, nasa_con_ip)
);
CREATE INDEX ix_lic_access_list_ip ON lic_access_list (nasa_con_ip, nasa_use);
CREATE INDEX ix_ipcs_log_date ON ipcs_log (lic_insert_date);

CREATE TABLE site_sms_user (
  sms_site_ip      varchar(50)  NOT NULL,
  sms_site_real_ip varchar(50)  NOT NULL,
  sms_site_name    varchar(256),
  sms_use          char(1)      NOT NULL,
  sms_user_name    varchar(50),
  sms_user_phone   varchar(50)  NOT NULL
);
CREATE INDEX ix_site_sms_user ON site_sms_user (sms_use, sms_site_ip, sms_site_real_ip);

CREATE TABLE site_sms_basic_user (
  site_sms_index  bigserial PRIMARY KEY,
  site_sms_name   varchar(50) NOT NULL,
  site_sms_telnum varchar(50) NOT NULL,
  site_sms_use    char(1)     NOT NULL
);

CREATE TABLE site_email_user (
  idx                bigserial PRIMARY KEY,
  email_site_ip      varchar(50)  NOT NULL DEFAULT '',
  email_site_real_ip varchar(50)  NOT NULL DEFAULT '',
  email_site_name    varchar(100) DEFAULT '',
  alarm_type         integer      DEFAULT 0,
  alarm_proc_name    varchar(50)  DEFAULT '',
  email_to           varchar(500) DEFAULT '',
  email_from         varchar(200) DEFAULT 'support@onpia.co.kr',
  email_title        varchar(200) DEFAULT '',
  email_msg1         varchar(500) DEFAULT '',
  email_msg2         varchar(500) DEFAULT '',
  email_msg3         varchar(500) DEFAULT '',
  email_url          varchar(2048) DEFAULT 'http://203.239.190.7/',
  email_attr1        varchar(100) DEFAULT '',
  email_attr2        varchar(100) DEFAULT '',
  email_attr3        varchar(100) DEFAULT ''
);

CREATE TABLE site_smsexcludetime (
  sm_index serial PRIMARY KEY,
  sm_type  integer,
  sm_msg   varchar(200),
  sm_stime varchar(10),
  sm_etime varchar(10),
  sm_use   char(2) DEFAULT 'Y'
);
```

- 방언 치환 1건: `TrsSocketServer.cpp:903` `DATEDIFF(hour, cc_insert_time, getdate()) > 3` → `EXTRACT(EPOCH FROM now() - cc_insert_time)/3600 > 3`
- `"MTS_ATALK_MSG"`는 따옴표 식별자이므로 PG에서 대문자 테이블이어야 함(기존 PG 그대로 사용)
- 데이터 이관: MSSQL `bcp` → CSV(CP949) → PG `COPY ... ENCODING 'EUCKR'`

---

## 7. 포팅 시 반드시 가져갈 코드 자산

- 레코드 구조체 5종은 **`MyDefine.h:410-467`** 에 정의되어 있다 (`EMAIL_SITE_USERS` ~`:428`, `SMS_SITE_USERS` ~`:441`, `SMS_SITE_BASIC_USER` ~`:448`, `CC_LINKINFO` ~`:456`, `SMS_EXCLUDE` ~`:467`). `MyDefine.h`의 CTI 정의는 죽은 코드지만 **이 5개 구조체는 살아 있는 코드**이므로 Linux `struct_def.h`(신규)로 옮긴다 (리눅스참조구조 §3 매핑 보정)
- 버퍼 길이는 §5-6의 세 컬럼(`cc_name`, `sms_site_name`, `cc_nasupload_fail`)만 운영 길이에 맞춰 확대하고 나머지는 유지
