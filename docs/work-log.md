# 작업 로그

## [2026-10-13] 프로젝트 초기화

### 작업 내용
- 폴더 구조 생성: `docs/`, `scripts/`, `tests/`, `logs/`, `_backup/`
- `.gitignore`, `CLAUDE.md`, `README.md`, `docs/설계문서.md`, `docs/work-log.md` 작성
- `scripts/build.ps1` (MSBuild 빌드 스크립트) 작성
- git init + 첫 커밋 (`core.autocrlf=false`로 CRLF 보존)
- 기존 소스 현황 파악: `Source/TrsSocketServer_Lic_VS2019_Atalk/` — MFC 다이얼로그 앱, cpp 11 / h 14, 약 6,100줄
  - 클래스 9개 (App, Dlg, ListenSocket, ClientSocket, UdpSocket, DirectODBC, AlramWeb, SystemTray, URLEncode)
  - 인코딩: cpp/h 다수 EUC-KR + CRLF (AlramWeb, DirectODBC, TrsSocketServer, TrsSocketServerDlg, MyDefine.h, resource.h, StdAfx.h, UdpSocket.h, AlramWeb.h)

### 결정 사항
- 프로젝트 성격: **기존 C++ 소스 분석/유지보수** (Python 템플릿 구조는 적용하지 않음 → requirements.txt, config.yaml, src/, cli/ 생략)
- 버전 관리: **SVN 유지 + git 로컬 병행**. `.svn/` git 제외. SVN에는 `Source/`만 등록된 상태(rev 1)
- EUC-KR 파일 수정 시 Edit/Write 금지, Python cp949 처리 + `_backup/` 백업 필수 (CLAUDE.md에 명시)

### 다음 할 일
- `deep-analyze-c` 또는 `analyze-c` 스킬로 소스 전체 분석 리포트 생성
- `docs/설계문서.md` 4절(데이터 흐름), 6절(미확인 항목) 채우기
- `scripts/build.ps1`로 실제 빌드 가능 여부 확인 (VS2019 설치 환경)

---
