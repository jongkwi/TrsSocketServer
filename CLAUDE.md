# TrsSocketServer 프로젝트 규칙

## 언어
- 한국어로 소통

## 프로젝트 개요
- CTI(Computer Telephony Integration) 미들웨어 소켓 서버. C++ MFC 다이얼로그 기반 Windows 앱
- 상담원(AGT)·교환기(PBX)·웹(WEB)·시스템(SYS) 소켓을 수용하여 호분배, 로그온/로그아웃, 전화 제어 명령을 중계
- ODBC로 DB 연동, 사용자 라이선스(`USER_LIC`) 관리, 웹 알람 전송, 시스템 트레이 상주
- 이 프로젝트의 목적: **기존 C++ 소스 분석 → 문서화 → 유지보수/리팩토링**

## 소스 위치
- 메인 소스: `Source/TrsSocketServer_Lic_VS2019_Atalk/`
- 솔루션: `TrsSocketServer.sln` / `TrsSocketServer.vcxproj` (VS2019, v142, MFC Static, MultiByte)
- 런타임 설정: `TRSSocket.ini` (LINK_SERVER / BLOCK_IP / USER_LIC)
- 상수/명령코드 정의: `MyDefine.h`

## 주요 클래스
| 클래스 | 파일 | 역할 |
|---|---|---|
| `CTrsSocketServerApp` | TrsSocketServer.cpp/h | 앱 진입점 (CWinApp) |
| `CTrsSocketServerDlg` | TrsSocketServerDlg.cpp/h | 메인 다이얼로그, 타이머, 리스트 출력, 트레이 처리 |
| `CListenSocket` | ListenSocket.cpp/h | TCP 수신 대기 (CAsyncSocket) |
| `CClientSocket` | ClientSocket.cpp/h | 접속 클라이언트 세션 처리 |
| `CUdpSocket` | UdpSocket.cpp/h | UDP 통신 |
| `CDirectODBC` | DirectODBC.cpp/h | ODBC 직접 DB 접근 |
| `CAlramWeb` | AlramWeb.cpp/h | 웹 알람 전송 |
| `CSystemTray` | SystemTray.cpp/h | 트레이 아이콘 |
| `CURLEncode` | URLEncode.cpp/h | URL 인코딩 |
| (함수) | charconv.cpp/h | 문자 인코딩 변환 |

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
