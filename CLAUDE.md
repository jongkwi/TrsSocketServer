# TrsSocketServer 프로젝트 규칙

## 언어
- 한국어로 소통

## 프로젝트 개요
- 콜센터 사이트 **감시·알람 중계 서버**. C++ MFC 다이얼로그 기반 Windows 앱 (트레이 상주)
- TCP 6321: IPCS 클라이언트 라이선스 체크(`lic_access_list`) → raw `Run`/`Close`
- UDP 5363: 사이트 감시 에이전트의 장애/복구 이벤트(nType 30종) 수신 → ACK echo → 알람 뷰어(TCP 6770) 푸시 + 웹푸시 + 알림톡(PostgreSQL 큐) + 이메일
- 5초 타이머: LINK_CHECK 21분 미수신 사이트 장애 판정
- **주의**: `MyDefine.h`의 CTI(호분배/AGENT_CMD/PBX) 정의는 전부 미사용 잔재. 이 프로그램은 CTI 미들웨어가 아님 (2026-10-13 분석으로 확정). 단 `MyDefine.h`의 DB 레코드 구조체 5종(`EMAIL_SITE_USERS`, `SMS_SITE_USERS`, `SMS_SITE_BASIC_USER`, `CC_LINKINFO`, `SMS_EXCLUDE`)은 **살아 있는 코드** → 포팅 대상
- 프로젝트 목적: **기존 클라이언트 무변경으로 Linux 포팅**. 현재 단계: `Source/onlic/` 1~5단계 구현 완료(라이브러리 복사·골격·DB·프레이밍·소켓/워커/업무로직, 4,544줄), **Rocky 9 빌드·테스트 전**. 새 소스는 EUC-KR+CRLF (생성·패치는 Python cp949)
- 분석 문서: `docs/기능목록.md`(기준선), `docs/프로토콜명세.md`(호환 계약, 클라이언트 4종 대조 완료), `docs/포팅영향분석.md`(§8 결정 사항·§8.1 추가 필요 정보), `docs/리눅스참조구조.md`(onmw 기반 구조 매핑), `docs/DB스키마.md`(운영 MSSQL 조회로 확정한 DDL·ODBC·서버 구성), `docs/확인결정사항.md`(남은 확인 A·결정 B 목록), `Source/onlic/db/`(통합 PG DDL·이관 스크립트), `docs/작업요약_2026-10-13.md`(일일 요약), `docs/TrsSocketServer_architecture_diagram.html`
- 확정 결정(2026-10-13): CP949 유지 / 결함 수정 / **PostgreSQL + unixODBC 직접**(CDbProxy TCP 게이트웨이 미사용) / 죽은 코드 제외 / **onmw_3.10.79 라이브러리·패턴 동일 구조** (ShareLib·LogObject·TimerObject·ListObject·UrlEnDe 복사, 포트별 소켓 스레드 + WaitStack 워커 + 스레드당 DB 핸들)
- onmw `struct_def.h`는 포함 금지(값 충돌). `-D_UTF8` 미정의
- **운영 확정(2026-10-13)**: 190.44(1.0.0.01, 2018) = **라이선스 1차**, 로컬 MSSQL 34333 `Nasa_License`에 `lic_access_list` 859행·`IPCS_LOG` 32.6만 행(실시간). 190.7(1.0.0.7 = 현재 소스) = 알람 + 라이선스 2차, 로컬 `NASA_License`에 `CallCenter_Link`·`SITE_*`만 있고 `lic_access_list` 없음 → 폴백 요청은 `Close`. **포팅 구성 결정: Linux 서버 2대(Rocky 9), 각각 로컬 PG(통합 스키마 동일), 앱은 로컬 DB만, 이중화·IP 목록 동기화는 추후**. A=`lic` 라이선스만(`[ROLE] ALARM=0`), B=`lic2` 라이선스+알람. 빌드는 **onmw Makefile 구조 동일**(CMake 아님). DB 장애 시 6321은 현행 `Close` 유지. SMS 경로는 차후 대비 유지(`[ATALK]/[SMS] USE` 런타임 스위치). 발급 프로그램 PG 전환은 별도. 전환은 서버 완료 후 도메인 IP 변경 또는 기존 IP 재사용. **결정 완료 → 구현 착수 가능** (`docs/확인결정사항.md` §0)

## 소스 위치
- 메인 소스: `Source/TrsSocketServer_Lic_VS2019_Atalk/`
- 솔루션: `TrsSocketServer.sln` / `TrsSocketServer.vcxproj` (Debug v142·MultiByte / Release v100·NotSet, MFC Static, `_ATALK` 정의)
- 포트·타이머 상수: `StdAfx.h:27-40` (6321/5363/6770 하드코딩)
- 런타임 설정: `TRSSocket.ini` — 실제 사용 키는 `[LINK_SERVER] SERVER_PORT` 1개뿐 (도달 불가 분기)
- DB DSN/계정: `TrsSocketServerDlg.cpp:216-224` 하드코딩 (문서에는 마스킹할 것). `ipcs_log`=MSSQL NASA_License, `onpia_check`=PostgreSQL, `onpia_sms`=MSSQL tcssdk(개발 PC DSN 기준)

## 주요 클래스
| 클래스 | 파일 | 역할 |
|---|---|---|
| `CTrsSocketServerApp` | TrsSocketServer.cpp/h | **모든 비즈니스 로직** (ServerProc, ServerUdpProc, OnCheckLinkData, 알림, 로그) |
| `CTrsSocketServerDlg` | TrsSocketServerDlg.cpp/h | 메인 다이얼로그, 타이머(START/LINK), DB 연결 트리거, 화면 로그, 트레이 |
| `CListenSocket` | ListenSocket.cpp/h | 6321/6770 Accept → App 위임 |
| `CClientSocket` | ClientSocket.cpp/h | 접속 세션, 잔여 버퍼 `m_Rdata[1001]` |
| `CUdpSocket` | UdpSocket.cpp/h | 5363 수신 → App 위임. nType enum 정의 |
| `CDirectODBC` | DirectODBC.cpp/h | ODBC 2.x 직접 호출. 인스턴스 3개 (MSSQL / PostgreSQL / SMS) |
| `CAlramWeb` | AlramWeb.cpp/h | 워커 스레드 + 큐, WinInet POST (웹푸시·이메일) |
| `CSystemTray` | SystemTray.cpp/h | 트레이 아이콘 |
| `CURLEncode` / charconv | URLEncode·charconv.cpp/h | CP949→UTF-8 %XX 인코딩 |

## 인코딩 규칙 (매우 중요)
- `Source/` 내 cpp/h 다수가 **EUC-KR + CRLF** 입니다 (`file` 명령 결과 ISO-8859 / Non-ISO extended-ASCII)
- EUC-KR 파일은 **Edit/Write 도구 사용 금지** → Python으로 cp949 읽기/쓰기, CRLF 유지
- 수정 전 반드시 `_backup/{파일명}.bak_YYYYMMDD_HHMMSS` 백업
- ASCII로 표시되는 파일도 한글 주석 추가 시 EUC-KR로 저장할 것 (MultiByte 빌드)
- 수정 후 `file` 명령으로 인코딩이 바뀌지 않았는지 재확인

## 작업 흐름 규칙
- 작업 전 계획을 먼저 제시하고 확인 받을 것
- 파일 수정 전 반드시 해당 파일을 먼저 읽을 것
- 소켓/DB/스레드 관련 수정은 영향 범위(호출부) 먼저 파악
- 빌드는 VS2019 MSBuild로 확인 (`scripts/build.ps1` 참고)
- 분석 작업은 `analyze-c`, `deep-analyze-c`, `c-cpp-expert` 스킬 활용
- 절체 데이터 추출: `scripts/db_export_mssql_to_pg.py` (ini 접속정보 git 제외, 원본 DB 는 SELECT 만). 산출물 `scripts/<YYYYMMDD_HHMMSS>/` 는 개인정보 포함 → git 제외, 이관 후 삭제

## 버전 관리
- **SVN**: 서버 공유용 (`https://jk-gp75/svn/TrsSocketServcer`), `Source/`만 등록
- **git**: 로컬 이력용, `.svn/`은 git 제외. `core.autocrlf=false` (CRLF 보존)
- 커밋은 사용자가 명시적으로 요청할 때만

## 기술 스택
- C++ / MFC (CAsyncSocket, CDialog, CWinApp)
- Visual Studio 2019 (PlatformToolset v142), MFC Static, MultiByte(EUC-KR)
- ODBC (SQL), Windows Socket, UDP
- 설정: INI 파일

## 폴더 구조
```
TrsSocketserver/
├── Source/            # 기존 C++ 소스 (SVN 관리, 인코딩 주의)
├── docs/              # work-log.md, 설계문서.md, 분석 리포트
├── scripts/           # 빌드/분석 보조 스크립트
├── tests/             # 테스트 코드 (소켓 시뮬레이터 등)
├── logs/              # 실행 로그 (git 제외)
└── _backup/           # 수정 전 원본 백업 (git 제외)
```
