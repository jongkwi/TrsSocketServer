# onlic — TrsSocketServer Linux 포팅

Windows MFC `TrsSocketServer`(1.0.0.7)를 **기존 클라이언트 무변경**으로 Linux(Rocky 9)에 포팅한 소스.
구조는 `onmw_3.10.79`와 동일(공용 라이브러리 5개 `.a` → 앱 링크, g++ Makefile).

## 폴더

| 폴더 | 내용 | 출처 |
|---|---|---|
| `include/{ShareLib,LogObject,TimerObject,ListObject,UrlEnDe}`, `source/…` | 공용 라이브러리 5개 | `F:\AI_Pro\_proc\onmw\onmw_source\onmw_3.10.79` **바이트 동일 복사** (2026-10-13, `diff -rq` 확인) |
| `include/shareheader.h` | 플랫폼 공통 헤더 (`ShareLib.h`가 `<shareheader.h>`로 포함) | 동일 복사 |
| `include/onlic`, `source/onlic` | **이 프로젝트의 앱 코드** (신규) | 설계: `docs/리눅스참조구조.md` |
| `linux/Makefile_64bit` | onmw `linux/Makefile_64bit_curl` 과 같은 형식(명령 나열, `all:` / `onlic:` 타깃). `-D_UTF8` 제외, 앱명 `onlic`, `-lcurl` 항상 포함. `linux/Makefile` 은 이 파일로 위임 | 신규 |
| `cfg/` | `onlic.ini` (DB 접속정보 포함 → **커밋 금지**, `onlic.ini.sample`만 커밋) | 신규 |
| `lib/`, `bin/`, `log/` | 빌드 산출물·로그 (git 제외) | |
| `doc/` | onmw 운영 스크립트 참고 사본 (`runmw`, `ps_mw.chk.sh`, `mwcron`) | 참고용 |
| `win32/onlic/onlic.sln` | **Visual Studio 2010** 솔루션 (onmw `win32/NcMiddleVC90` 구조 복제). 라이브러리 5개 프로젝트 + `onlic` 앱 프로젝트(소스·헤더·설정/DB 파일). 열람·편집용 — onlic 앱은 Linux 전용 코드라 Windows 빌드 불가, 라이브러리는 빌드됨 | 신규 |
| `test/` | 프레이밍·프로토콜 단위 테스트, 클라이언트 시뮬레이터 | 신규 |

## 인코딩 주의
- 복사한 라이브러리 중 EUC-KR + CRLF 파일: `shareheader.h`, `ShareLib/{ReadNicMac,UserTime}.{h,cpp}`, `ShareLib/Fourier.cpp`, `LogObject/LogObject.{h,cpp}`, `TimerObject/TimerObject.cpp`, `ListObject/ListObject.cpp`, `UrlEnDe/url_en_de.cpp`
- 이 파일들은 **수정하지 않는 것이 원칙**(onmw와 동일 유지). 불가피하면 프로젝트 규칙(백업 + cp949 Python)대로
- 신규 작성하는 `onlic/` 소스는 **EUC-KR + CRLF**로 통일 (CP949 유지 결정, 알람 문구 리터럴 호환). ASCII만 있는 파일은 그대로 둬도 됨

## 빌드 (Rocky 9)
```bash
sudo dnf install -y gcc-c++ make unixODBC unixODBC-devel postgresql-odbc libcurl-devel
cd linux
make -f Makefile_64bit            # lib/*.a 5개 + bin/onlic (onmw 와 같은 방식)
make -f Makefile_64bit onlic      # 앱만 재빌드
make                              # 위와 동일 (Makefile 이 Makefile_64bit 로 위임)
```
플래그: `-D_UNIX -D_LINUX -D_64BIT -D_SOCK_SELECT -D_DB_TYPE_POSTGRES -D_CURL` (`-D_UTF8` 없음)

## 실행
```bash
cd bin
./onlic -d                  # 데몬. 설정 ../cfg/onlic.ini
```
역할은 `cfg/onlic.ini` `[ROLE]`로 분리: A(`lic`) 서버 `ALARM=0 LINK_CHECK=0`, B(`lic2`) 서버 모두 1.

## 호환 계약
포트·프레임·응답 규칙은 `docs/프로토콜명세.md`가 기준. 변경 금지 항목은 `docs/리눅스참조구조.md` §6.
