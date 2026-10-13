# TrsSocketServer

## 개요
CTI(Computer Telephony Integration) 미들웨어 소켓 서버입니다. C++ MFC 기반 Windows 다이얼로그 앱으로, 상담원·교환기·웹·시스템 클라이언트의 TCP/UDP 접속을 받아 호분배와 전화 제어 명령을 중계하고, ODBC로 DB와 연동하며 사용자 라이선스를 관리합니다.

이 저장소는 기존 소스의 **분석 · 문서화 · 유지보수**를 위한 프로젝트입니다.

## 빌드
Visual Studio 2019 (v142 툴셋, MFC 포함) 필요.
```powershell
# VS 개발자 PowerShell 또는 scripts/build.ps1 사용
msbuild Source\TrsSocketServer_Lic_VS2019_Atalk\TrsSocketServer.sln /p:Configuration=Release /p:Platform=Win32
```

## 실행
빌드 산출물 `TrsSocketServer.exe`를 `TRSSocket.ini`와 같은 폴더에서 실행합니다.

## 설정 (`TRSSocket.ini`)
| 섹션 | 키 | 설명 |
|---|---|---|
| `[LINK_SERVER]` | `SERVER_IP`, `SERVER_PORT` | 연동 서버 주소 |
| `[BLOCK_IP]` | `IP=0/1` | 차단 IP 목록 |
| `[USER_LIC]` | `S01=N` | 사용자 라이선스 수 |

## 폴더 구조
```
TrsSocketserver/
├── Source/TrsSocketServer_Lic_VS2019_Atalk/   # C++ MFC 소스 (SVN 관리)
├── docs/        # 작업 로그, 설계 문서, 분석 리포트
├── scripts/     # 빌드/분석 보조 스크립트
├── tests/       # 테스트 코드
├── logs/        # 실행 로그 (git 제외)
└── _backup/     # 수정 전 백업 (git 제외)
```

## 주의
- `Source/` 내 cpp/h 파일 다수가 **EUC-KR + CRLF** 인코딩입니다. 편집 시 인코딩을 보존하세요.
- 버전 관리: SVN(서버 공유) + git(로컬 이력) 병행. `.svn/`은 git에서 제외됩니다.

## 문서
- `docs/work-log.md` — 작업 로그 (최신이 위)
- `docs/설계문서.md` — 구조/흐름 분석
- `CLAUDE.md` — Claude Code 작업 규칙
