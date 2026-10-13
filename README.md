# TrsSocketServer

## 개요
콜센터 사이트 **감시·알람 중계 서버**입니다. C++ MFC 기반 Windows 다이얼로그 앱으로,
- TCP 6321에서 IPCS 클라이언트의 라이선스(실행 허가)를 확인해 `Run`/`Close`를 응답하고,
- UDP 5363으로 사이트 감시 에이전트의 장애/복구 이벤트를 받아 알람 뷰어(TCP 6770)·웹푸시·알림톡·이메일로 전달하며,
- 5초마다 링크 데이터가 21분 이상 끊긴 사이트를 감지합니다.

이 저장소의 목적은 **기존 클라이언트를 변경하지 않고 Linux로 포팅**하는 것입니다. 현재 소스 분석과 기능 기준선 작성이 완료된 상태입니다.

| 문서 | 내용 |
|---|---|
| `docs/기능목록.md` | 기능 기준선(F-xx), 죽은 코드(X-xx), 결함(B/S-xx), 검증 체크리스트 |
| `docs/프로토콜명세.md` | 포트별 바이트 레벨 프레임·응답 규칙 (클라이언트 호환 계약) |
| `docs/포팅영향분석.md` | Windows 의존성 매트릭스와 Linux 대체안, 결정 필요 사항 |
| `docs/TrsSocketServer_architecture_diagram.html` | 아키텍처 다이어그램 (브라우저) |
| `docs/*_심층분석_*.html/.md` | deep-analyze-c 자동 분석 리포트 |

## 빌드
Visual Studio 2019 (v142 툴셋, MFC 포함) 필요.
```powershell
# VS 개발자 PowerShell 또는 scripts/build.ps1 사용
msbuild Source\TrsSocketServer_Lic_VS2019_Atalk\TrsSocketServer.sln /p:Configuration=Release /p:Platform=Win32
```

## 실행
빌드 산출물 `TrsSocketServer.exe`를 `TRSSocket.ini`와 같은 폴더에서 실행합니다.

## 설정 (`TRSSocket.ini`)
| 섹션 | 키 | 실제 사용 |
|---|---|---|
| `[LINK_SERVER]` | `SERVER_PORT` | 도달 불가 분기에서만 읽음 (`SERVER_IP`는 미사용) |
| `[BLOCK_IP]` | `IP=0/1` | **미사용** (코드 참조 0건) |
| `[USER_LIC]` | `S01=N` | **미사용** (라이선스 판정은 DB `lic_access_list`) |

포트(6321/5363/6770), DB DSN/계정, 외부 URL, 알림톡 키는 소스에 하드코딩되어 있습니다.

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
