# db/ — 통합 PostgreSQL 스키마와 이관 스크립트

190.44 `Nasa_License`(라이선스)와 190.7 `NASA_License`(알람)를 **DB 1개 `nasa_license`** 로 통합한다 (확인결정사항 B17). 각 Linux 서버에 같은 스키마를 만들고(A=lic, B=lic2), 데이터는 역할에 맞게 넣는다.

## 0. DB 생성은 수동 (스크립트가 만들지 않음)
서버의 기존 PostgreSQL(포트 39999, 롤 `nasacti`)에 `postgres` 계정으로 1회 생성한다. 아래 모든 스크립트는 **DB 가 이미 있다고 전제**하고 접속만 확인한다(없으면 안내 후 종료).
```bash
su - postgres
createdb -p 39999 -O nasacti -D pg_default -E UTF8 nasa_license
psql -p 39999 -tAc "SELECT datname, pg_encoding_to_char(encoding) FROM pg_database WHERE datname='nasa_license'"   # nasa_license|UTF8
```
- 서버 인코딩 UTF8 + 클라이언트 EUCKR(onlic 이 `set client_encoding`) 조합. CP949 확장 한자·기호가 EUC-KR 범위를 벗어나 변환 오류가 나면 `-E EUC_KR -T template0` 로 다시 만드는 대안도 가능(psqlODBC 양쪽 지원). `apply.sh` 는 UTF8/EUC_KR 외 인코딩이면 중단
- 다시 만들 때: `dropdb -p 39999 nasa_license` 후 위 명령 재실행 (수동)
- 접속 기본값은 `127.0.0.1:39999`, `nasacti`, `nasa_license`. 다르면 `apply.sh` 실행 전 `PGHOST/PGPORT/PGUSER/PGDATABASE` 환경변수로 덮어쓰고, 비밀번호는 `PGPASSWORD` 또는 `~/.pgpass`

| 파일 | 용도 |
|---|---|
| `01_nasa_license_schema.sql` | 통합 DDL(**재실행 가능**: `IF NOT EXISTS`, FK 존재 검사). 테이블 19개 (라이선스 7 + 알람·OnAlarm 웹 12), PK/FK/인덱스 + onlic 권고 인덱스. `gen_pg_schema.py` 가 두 서버 카탈로그 덤프에서 생성 |
| `02a_pgloader_from_190_7.load` | 190.7 → 알람 테이블 전부 + IPCS_LOG 는 staging(`ipcs_log_from_lic2`) |
| `02b_pgloader_from_190_44.load` | 190.44 → 라이선스 테이블 7개 (`nasa_lic*` 는 44 가 원본) |
| `03_post_migrate.sql` | IPCS_LOG 병합, identity 시퀀스 보정, ANALYZE, 건수 검증 |

## 데이터 INSERT 파일 (`scripts/<YYYYMMDD_HHMMSS>/`, git 제외) — 절체 당일 `scripts/db_export.bat` 로 생성
```bat
rem Windows: scripts\db_export.bat 더블클릭 (또는 db_export.bat --dry-run / --source 44). 아래 python 직접 실행과 동일
```
```bash
# 이 PC(Windows, sqlcmd/bcp 설치)에서. 접속정보는 scripts/db_export.ini (샘플 db_export.ini.sample, git 제외)
python scripts/db_export_mssql_to_pg.py --dry-run     # 원본 건수만 확인
python scripts/db_export_mssql_to_pg.py               # 두 서버 전부 추출 → scripts/<YYYYMMDD_HHMMSS>/ (실행 파일 하위)
python scripts/db_export_mssql_to_pg.py --source 44   # 44 만
```
생성 파일 (원본 DB 는 SELECT 만, 무변경):

| 파일 | 내용 | 재실행 동작 |
|---|---|---|
| `10_data_44.sql` | 44: `lic_access_list`, `nasa_lic`, `nasa_lic_log`, `nasa_security`, `callbox_log`, `softphone_log` | 선두 `TRUNCATE` 후 INSERT (덮어쓰기) |
| `10_data_44_ipcs_log.sql` | 44: `ipcs_log` (identity 값 유지) | `TRUNCATE ipcs_log` 후 INSERT |
| `12_data_7.sql` | 7: `callcenter_link`, `site_*`, OnAlarm 웹 테이블 (TRUNCATE 후 INSERT) + 7 의 `ipcs_log` 1,986행은 **append 모드**(임시 테이블 → 같은 행이 없을 때만 INSERT) | 몇 번 돌려도 중복 없음 |
| `13_after_insert.sql` | identity 시퀀스 setval, ANALYZE, 건수 확인 | 항상 실행 가능 |
| `01_nasa_license_schema.sql` | 스키마 사본 (폴더만 옮기면 되도록 자동 복사) | |
| `apply.sh` | DB 접속·인코딩 확인(생성 안 함) → 위 순서대로 psql 실행. `./apply.sh`(서버 B 전부) / `./apply.sh A`(서버 A: 12_* 생략). 기본 접속 `127.0.0.1:39999 nasacti nasa_license`, 환경변수 PGHOST/PGPORT/PGUSER/PGDATABASE/PGPASSWORD 또는 ~/.pgpass 로 변경 | |
| `apply_commands.txt` | 같은 내용을 psql 명령 목록(텍스트)으로 | |
| `_report.txt` | 원본 건수 vs 생성 건수 (MISMATCH 있으면 스크립트 종료코드 1) | |

적용 (각 파일은 `BEGIN…COMMIT` 한 트랜잭션, 실패 시 전체 롤백). §0 의 DB 생성 후, 폴더 안의 `apply.sh` 또는 아래를 직접:
```bash
chmod +x apply.sh && ./apply.sh        # 서버 B(lic2). 서버 A(lic) 는 ./apply.sh A
# --- 수동 (apply_commands.txt 와 동일) ---
P="psql -h 127.0.0.1 -p 39999 -U nasacti -d nasa_license"
$P -f 01_nasa_license_schema.sql        # 재실행 가능 (IF NOT EXISTS)
$P -v ON_ERROR_STOP=1 -f <ts>/10_data_44.sql
$P -v ON_ERROR_STOP=1 -f <ts>/10_data_44_ipcs_log.sql
$P -v ON_ERROR_STOP=1 -f <ts>/12_data_7.sql    # 서버 A(lic) 는 생략 가능
$P -f <ts>/13_after_insert.sql
```
- 이미 추출해 둔 폴더에 `apply.sh` 가 없거나(구버전 추출) 접속 기본값을 바꿨으면: `python scripts/db_export_mssql_to_pg.py --regen-apply <폴더>` (추출은 다시 하지 않고 `apply.sh`/`apply_commands.txt`/`01` 사본만 재생성)
- 전화번호·사이트 DB 비밀번호(`db_pwd`) 포함 → 이관 후 타임스탬프 폴더 삭제. 폴더 통째로 서버에 복사해 적용
- 변환 규칙: NULL → `NULL`, 빈 문자열 → `''`, `'` → `''`, datetime → `'YYYY-MM-DD HH:MM:SS.mmm'`, 0x00·제어문자 제거, CP949 해석 불가 바이트 → U+FFFD (`IPCS_LOG.LOG_IP` 의 스캐너 쓰레기 값, 판정엔 `LOG_REAL_IP` 만 사용)
- 스키마가 바뀌어도 스크립트가 실행 시점의 INFORMATION_SCHEMA 를 읽어 컬럼을 맞춘다. 단 `01_nasa_license_schema.sql` 은 다시 생성해야 함(컬럼 추가 시)

## 순서 — pgloader 방식 (대안, Rocky 9, PostgreSQL 15/16)
```bash
sudo dnf install -y pgloader                           # pgloader 가 저장소에 없으면 docker: ghcr.io/dimitri/pgloader
# DB 는 §0 대로 수동 생성 후
P="psql -h 127.0.0.1 -p 39999 -U nasacti -d nasa_license"
$P -f 01_nasa_license_schema.sql
pgloader 02a_pgloader_from_190_7.load                  # PASSWORD 채운 뒤. 190.7:1433 접속 필요
pgloader 02b_pgloader_from_190_44.load                 # 190.44:34333 접속 필요
$P -f 03_post_migrate.sql
```
- 서버 A(lic) 는 02b 만, 서버 B(lic2) 는 02a + 02b(`lic_access_list` 초기 복사본) 를 넣는다. 스키마는 양쪽 동일
- pgloader 가 MSSQL 에 접속하려면 FreeTDS(`freetds`) 가 필요하다. 44 는 포트 34333
- `.load` 파일에 비밀번호를 적었으면 커밋하지 말 것 (.gitignore: `Source/onlic/db/*.load` 는 커밋 대상이므로 PASSWORD 자리표시자 유지)

## 두 서버 정의가 달랐던 부분 (통합 시 적용)
- `nasa_lic`: 44 에 `nl_bit varchar(50)` 컬럼이 더 있음 → 포함
- `nasa_security`: 44 는 varchar(50)/varchar(30) NOT NULL + PK(ns_official, ns_password), 7 은 nvarchar(50) PK 없음 → 44 기준
- `IPCS_LOG`: 두 서버 동일 DDL. 44 가 본체(32.6만 행), 7 의 1,986행은 identity 재부여해 병합
- 그 외 OnAlarm 웹 테이블(`enterprise`, `project`, `server`, `on_alarm_user`, `CallCenter_Link_Reference`, `SITE_AUTH`)은 7 에만 있음 → 7 기준

## onlic 과의 관계
- 코드의 SQL 은 `CallCenter_Link`, `IPCS_LOG` 처럼 대소문자 섞어 쓰지만 PostgreSQL 이 소문자로 접어 매핑된다
- `cc_update_time` 등 문자열 날짜 컬럼은 원본 그대로 varchar (코드가 문자열 비교)
- `DATEDIFF` 1건만 코드에서 PG 방언으로 치환됨 (`DbLic_IsNewSiteExpired`)
