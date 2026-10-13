#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
db_export_mssql_to_pg.py - 절체 당일 운영 MSSQL(190.44 / 190.7) 데이터를 PostgreSQL INSERT 파일로 추출

  * 원본 DB 에는 SELECT 만 수행 (bcp queryout). 스키마는 실행 시점에 INFORMATION_SCHEMA 에서 읽는다
  * 출력: 실행 파일 폴더(scripts/) 아래 <YYYYMMDD_HHMMSS>/ 에
        00_create_db.sql, 01_nasa_license_schema.sql (스키마 사본)
        10_data_44.sql, 10_data_44_ipcs_log.sql, 12_data_7.sql, 13_after_insert.sql (데이터)
        apply.sh, apply_commands.txt (적용 스크립트), _report.txt (건수 검증)
  * 재실행 가능: 스키마는 IF NOT EXISTS, 데이터 파일은 TRUNCATE 후 INSERT (append 테이블은 동일 행 건너뜀)
  * 접속정보: scripts/db_export.ini (git 제외, 샘플 db_export.ini.sample)
  * 필요 도구: sqlcmd, bcp (Windows: SQL Server 클라이언트 도구 / Linux: mssql-tools)

사용:
  python db_export_mssql_to_pg.py                 # ini 의 모든 소스
  python db_export_mssql_to_pg.py --source 44     # 44 만
  python db_export_mssql_to_pg.py --tables "lic_access_list,IPCS_LOG"   (bat 에서는 쉼표 목록을 따옴표로)
  python db_export_mssql_to_pg.py --dry-run       # 테이블·행수만 확인, 파일 생성 안 함
"""
from __future__ import annotations
import argparse, configparser, io, logging, os, re, shutil, subprocess, sys, time
from datetime import datetime

NULLM = "<<NULL>>"
FS, RS = "|#|", "#@#\n"
NUL = chr(0)
CTRL = re.compile("[" + "".join(chr(i) for i in list(range(0, 9)) + [11, 12] + list(range(14, 32))) + "]")
BATCH = 1000
RETRY = 3            # 네트워크 순단 대비: sqlcmd/bcp 실패 시 재시도 횟수
RETRY_WAIT = 5       # 초
LOGIN_TIMEOUT = "30" # sqlcmd -l (초). 44 는 인터넷 경유 34333 포트라 여유 있게

log = logging.getLogger("db_export")


def run_with_retry(cmd: list[str], what: str, codepage: str) -> tuple[int, str]:
    """subprocess 실행. 실패(종료코드≠0)하면 RETRY 회까지 재시도 (연결 순단 대비). 반환 (returncode, 출력)"""
    out = ""
    for i in range(1, RETRY + 1):
        r = subprocess.run(cmd, capture_output=True)
        out = (r.stdout + r.stderr).decode(codepage, "replace")
        if r.returncode == 0:
            return 0, out
        log.warning("%s 실패 (%d/%d): %s", what, i, RETRY, out.strip().splitlines()[-1][:200] if out.strip() else "")
        if i < RETRY:
            time.sleep(RETRY_WAIT)
    return 1, out


# ---------------------------------------------------------------- 설정
def load_config(path: str) -> configparser.ConfigParser:
    if not os.path.exists(path):
        sys.exit("설정 파일이 없습니다: %s  (db_export.ini.sample 을 복사해 접속정보를 채우세요)" % path)
    cp = configparser.ConfigParser(interpolation=None)
    cp.read(path, encoding="utf-8")
    return cp


def tool_path(cp: configparser.ConfigParser, name: str) -> str:
    return cp.get("tools", name, fallback=name)


def resolve_dir(cp_path: str, d: str) -> str:
    """ini 의 상대 경로는 ini 파일 위치 기준"""
    if os.path.isabs(d):
        return d
    return os.path.normpath(os.path.join(os.path.dirname(os.path.abspath(cp_path)), d))


# ---------------------------------------------------------------- MSSQL 조회
def run_sqlcmd(cp, src: dict, query: str) -> list[list[str]]:
    """sqlcmd -W -s'|' -h -1 로 조회. 메시지/경고 행 제거 후 행 리스트 반환"""
    cmd = [tool_path(cp, "sqlcmd"), "-S", src["server"], "-U", src["user"], "-P", src["password"],
           "-d", src["db"], "-l", LOGIN_TIMEOUT, "-W", "-s", "|", "-w", "8000", "-h", "-1", "-Q", "SET NOCOUNT ON; " + query]
    rc, out = run_with_retry(cmd, "sqlcmd(%s)" % src["name"], src.get("codepage", "cp949"))
    if rc != 0:
        raise RuntimeError("sqlcmd 실패 (%s, %d회 재시도 후): %s" % (src["name"], RETRY, out.strip()[-500:]))
    rows = []
    for ln in out.splitlines():
        ln = ln.rstrip("\r")
        if not ln.strip() or ln.startswith("(") or ln.startswith("Msg ") or ln.startswith("메시지 ") or ln.startswith("경고"):
            continue
        rows.append([c.strip() for c in ln.split("|")])
    return rows


def fetch_columns(cp, src: dict, tables: list[str]) -> dict[str, list[dict]]:
    q = ("SELECT c.TABLE_NAME, c.ORDINAL_POSITION, c.COLUMN_NAME, c.DATA_TYPE, "
         "ISNULL(CAST(c.CHARACTER_MAXIMUM_LENGTH AS varchar),''), c.IS_NULLABLE, "
         "COLUMNPROPERTY(OBJECT_ID(c.TABLE_SCHEMA+'.'+c.TABLE_NAME), c.COLUMN_NAME, 'IsIdentity') "
         "FROM INFORMATION_SCHEMA.COLUMNS c WHERE c.TABLE_SCHEMA='dbo' ORDER BY c.TABLE_NAME, c.ORDINAL_POSITION")
    cols: dict[str, list[dict]] = {}
    for r in run_sqlcmd(cp, src, q):
        if len(r) < 7:
            continue
        cols.setdefault(r[0], []).append(dict(name=r[2], type=r[3], len=r[4], nul=r[5], ident=r[6]))
    want = {}
    lower = {k.lower(): k for k in cols}
    for t in tables:
        key = lower.get(t.lower())
        if key is None:
            raise RuntimeError("%s 에 테이블이 없습니다: %s" % (src["name"], t))
        want[key] = cols[key]
    return want


def fetch_counts(cp, src: dict, tables: list[str]) -> dict[str, int]:
    """테이블별 건수. 연결 1회로 한 번에 조회 (테이블마다 접속하던 방식은 순단에 취약)"""
    out = {t: -1 for t in tables}
    if not tables:
        return out
    q = " UNION ALL ".join("SELECT '%s' AS t, COUNT_BIG(*) AS c FROM [dbo].[%s]" % (t, t) for t in tables)
    for r in run_sqlcmd(cp, src, q):
        if len(r) >= 2 and r[0] in out and r[1].strip().lstrip("-").isdigit():
            out[r[0]] = int(r[1].strip())
    return out


# ---------------------------------------------------------------- bcp 추출
def sel_expr(c: dict) -> str:
    ty = c["type"].lower(); n = "[%s]" % c["name"]
    if ty in ("datetime", "smalldatetime", "datetime2"):
        e = "CONVERT(varchar(23), %s, 121)" % n
    elif ty == "date":
        e = "CONVERT(varchar(10), %s, 23)" % n
    elif ty in ("text", "ntext", "varchar", "nvarchar", "char", "nchar"):
        e = "CAST(%s AS varchar(max))" % n
    elif ty in ("bigint", "int", "smallint", "tinyint", "bit", "decimal", "numeric", "float", "real", "money"):
        e = "CAST(%s AS varchar(50))" % n
    elif ty in ("varbinary", "binary", "image"):
        e = "'0x' + CONVERT(varchar(max), %s, 2)" % n
    else:
        e = "CAST(%s AS varchar(max))" % n
    return "ISNULL(%s, '%s')" % (e, NULLM)


def bcp_table(cp, src: dict, table: str, cols: list[dict], work_dir: str) -> str:
    q = "SELECT %s FROM %s.dbo.[%s]" % (", ".join(sel_expr(c) for c in cols), src["db"], table)
    out = os.path.join(work_dir, "%s_%s.txt" % (src["name"], table))
    cmd = [tool_path(cp, "bcp"), q, "queryout", out, "-c", "-t", FS, "-r", RS,
           "-S", src["server"], "-U", src["user"], "-P", src["password"]]
    rc, msg = run_with_retry(cmd, "bcp(%s.%s)" % (src["name"], table), src.get("codepage", "cp949"))
    if rc != 0 or not os.path.exists(out):
        raise RuntimeError("bcp 실패 %s.%s (%d회 재시도 후): %s" % (src["name"], table, RETRY, msg.strip()[-500:]))
    return out


# ---------------------------------------------------------------- 변환
def pg_lit(v: str, c: dict) -> str:
    if v == NULLM:
        return "NULL"
    if v == NUL:                       # bcp -c: 빈 문자열은 0x00 1바이트, NULL 은 빈 필드
        v = ""
    v = CTRL.sub("", v)                # PostgreSQL 은 0x00 불가. 제어문자(깨진 클라이언트 데이터) 제거, \t\n\r 유지
    ty = c["type"].lower()
    if ty in ("bigint", "int", "smallint", "tinyint", "decimal", "numeric", "float", "real", "money"):
        return v if v.strip() != "" else "NULL"
    if ty == "bit":
        return "true" if v.strip() in ("1", "True") else "false"
    if ty in ("varbinary", "binary", "image"):
        return "decode('%s','hex')" % v[2:] if v.startswith("0x") else "NULL"
    return "'" + v.replace("'", "''") + "'"


def convert(src: dict, table: str, cols: list[dict], path: str, skip_ident: bool, f, codepage: str) -> tuple[int, int]:
    """truncate 모드: 파일 선두의 TRUNCATE 가 대상 테이블을 비운 뒤 INSERT
       append 모드(skip_ident, 예: 7 의 IPCS_LOG): 임시 테이블에 넣고 이미 같은 행이 있으면 건너뜀 → 재실행해도 중복 없음"""
    use = [c for c in cols if not (skip_ident and c["ident"] == "1")]
    raw = open(path, "rb").read().decode(codepage, "replace")
    rows = raw.split(RS)
    if rows and rows[-1] == "":
        rows.pop()
    n = bad = 0
    batch: list[str] = []
    tgt = table.lower()
    collist = ", ".join(c["name"].lower() for c in use)
    if skip_ident:
        stage = "_stage_" + tgt
        f.write("\n-- %s (원본 %s, 추출 %d행, identity 제외, append 모드: 동일 행은 건너뜀)\n" % (table, src["name"], len(rows)))
        f.write("DROP TABLE IF EXISTS %s;\nCREATE TEMP TABLE %s AS SELECT %s FROM %s WHERE false;\n" % (stage, stage, collist, tgt))
        hdr = "INSERT INTO %s (%s) VALUES\n" % (stage, collist)
    else:
        f.write("\n-- %s (원본 %s, 추출 %d행)\n" % (table, src["name"], len(rows)))
        hdr = "INSERT INTO %s (%s) VALUES\n" % (tgt, collist)
    for r in rows:
        vals = r.split(FS)
        if len(vals) != len(cols):
            bad += 1
            continue
        batch.append("(" + ", ".join(pg_lit(v, c) for v, c in zip(vals, cols) if not (skip_ident and c["ident"] == "1")) + ")")
        n += 1
        if len(batch) >= BATCH:
            f.write(hdr + ",\n".join(batch) + ";\n"); batch = []
    if batch:
        f.write(hdr + ",\n".join(batch) + ";\n")
    if skip_ident:
        cond = " AND ".join("x.%s IS NOT DISTINCT FROM s.%s" % (c["name"].lower(), c["name"].lower()) for c in use)
        f.write("INSERT INTO %s (%s) SELECT %s FROM %s s WHERE NOT EXISTS (SELECT 1 FROM %s x WHERE %s);\n" % (tgt, collist, collist, stage, tgt, cond))
        f.write("DROP TABLE IF EXISTS %s;\n" % stage)
    if bad:
        log.warning("%s: %d malformed rows skipped", table, bad)
    return n, bad


# ---------------------------------------------------------------- 적용 스크립트
def write_apply_scripts(out_dir: str, schema_files: list[str], cp) -> None:
    """추출 폴더 안에 apply.sh / apply_commands.txt 생성. 데이터 파일은 폴더 안의 1?_data_*.sql 을 이름순으로 적용"""
    pguser = cp.get("output", "pg_user", fallback="onlic")
    pgdb = cp.get("output", "pg_db", fallback="nasa_license")
    data_files = sorted(f for f in os.listdir(out_dir) if re.match(r"^1[0-9]_data_.*\.sql$", f))
    after = "13_after_insert.sql" if os.path.exists(os.path.join(out_dir, "13_after_insert.sql")) else ""
    alarm_srcs = [x.strip().lower() for x in cp.get("output", "alarm_sources", fallback="7").split(",") if x.strip()]

    def is_alarm(fname: str) -> bool:
        return any(re.search(r"_data_%s(_|\.)" % re.escape(a), fname.lower()) for a in alarm_srcs)

    txt = ["# %s 적용 순서 (psql). 이 폴더를 Linux 서버로 복사한 뒤 그 폴더에서 실행. 재실행 가능" % os.path.basename(out_dir), ""]
    sh = ["#!/bin/bash",
          "# apply.sh - 이 폴더의 스키마·데이터를 PostgreSQL 에 적용 (재실행 가능)",
          "#   ./apply.sh        전부 적용 (서버 B=lic2: 라이선스 + 알람)",
          "#   ./apply.sh A      서버 A=lic : 알람 데이터 파일(12_*) 생략",
          "#   접속: PGUSER / PGDATABASE / PGHOST / PGPASSWORD 환경변수 또는 ~/.pgpass",
          "set -e",
          'cd "$(dirname "$0")"',
          'ROLE="${1:-B}"',
          'export PGUSER="${PGUSER:-%s}" PGDATABASE="${PGDATABASE:-%s}"' % (pguser, pgdb),
          'PSQL="psql -v ON_ERROR_STOP=1"',
          'echo "== apply $(basename "$PWD") role=$ROLE user=$PGUSER db=$PGDATABASE =="',
          ""]
    if "00_create_db.sql" in schema_files:
        txt.append("# (최초 1회, 슈퍼유저) psql -U postgres -f 00_create_db.sql")
        sh.append("# 최초 1회만 (롤/DB 생성, 비밀번호 수정 후): psql -U postgres -f 00_create_db.sql")
    if "01_nasa_license_schema.sql" in schema_files:
        txt.append("psql -U %s -d %s -f 01_nasa_license_schema.sql" % (pguser, pgdb))
        sh.append("$PSQL -f 01_nasa_license_schema.sql")
    for f in data_files:
        if is_alarm(f):
            txt.append("psql -U %s -d %s -v ON_ERROR_STOP=1 -f %s   # 서버 A(lic) 는 생략" % (pguser, pgdb, f))
            sh.append('if [ "$ROLE" = "A" ]; then echo "skip %s (role A)"; else $PSQL -f %s; fi' % (f, f))
        else:
            txt.append("psql -U %s -d %s -v ON_ERROR_STOP=1 -f %s" % (pguser, pgdb, f))
            sh.append("$PSQL -f %s" % f)
    if after:
        txt.append("psql -U %s -d %s -f %s" % (pguser, pgdb, after))
        sh.append("$PSQL -f %s" % after)
    sh.append('echo "== done =="')
    io.open(os.path.join(out_dir, "apply_commands.txt"), "w", encoding="utf-8", newline="\n").write("\n".join(txt) + "\n")
    io.open(os.path.join(out_dir, "apply.sh"), "w", encoding="utf-8", newline="\n").write("\n".join(sh) + "\n")


# ---------------------------------------------------------------- 메인
def parse_sources(cp) -> list[dict]:
    srcs = []
    for sec in cp.sections():
        if not sec.startswith("source_"):
            continue
        s = dict(cp[sec])
        s["name"] = sec[len("source_"):]
        s["tables"] = [t.strip() for t in s.get("tables", "").split(",") if t.strip()]
        s["drop_identity_tables"] = {t.strip().lower() for t in s.get("drop_identity_tables", "").split(",") if t.strip()}
        s["file_prefix"] = s.get("file_prefix", "1%s" % s["name"])
        s["split_tables"] = {t.strip().lower() for t in s.get("split_tables", "").split(",") if t.strip()}
        srcs.append(s)
    if not srcs:
        sys.exit("ini 에 [source_*] 섹션이 없습니다")
    return srcs


def main(argv=None) -> int:
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    here = os.path.dirname(os.path.abspath(__file__))
    ap.add_argument("--config", default=os.path.join(here, "db_export.ini"))
    ap.add_argument("--source", default="all", help="all 또는 ini 의 source_<이름> 중 <이름> (쉼표 구분)")
    ap.add_argument("--tables", default="", help="추출할 테이블만 (쉼표 구분, 기본 ini 의 tables)")
    ap.add_argument("--out", default="", help="출력 루트 (기본 ini [output] dir = 스크립트 폴더). 아래에 YYYYMMDD_HHMMSS 폴더 생성")
    ap.add_argument("--dry-run", action="store_true", help="행수만 확인하고 파일을 만들지 않음")
    ap.add_argument("--keep-work", action="store_true", help="bcp 중간 파일 보관")
    a = ap.parse_args(argv)

    logging.basicConfig(level=logging.INFO, format="%(asctime)s %(levelname)s %(message)s")
    cp = load_config(a.config)
    srcs = parse_sources(cp)
    if a.source != "all":
        pick = {x.strip() for x in a.source.split(",")}
        srcs = [s for s in srcs if s["name"] in pick]
        if not srcs:
            sys.exit("--source 에 해당하는 소스가 없습니다: %s" % a.source)

    out_root = a.out or resolve_dir(a.config, cp.get("output", "dir", fallback="."))
    stamp = datetime.now().strftime("%Y%m%d_%H%M%S")
    out_dir = os.path.join(out_root, stamp)
    work_dir = os.path.join(out_dir, "_work")
    report: list[str] = ["db_export_mssql_to_pg %s" % stamp, ""]
    all_ident: list[tuple[str, str]] = []
    all_tables: list[str] = []
    t0 = time.time()

    for s in srcs:
        want = [t.strip() for t in a.tables.split(",") if t.strip()]
        if want:
            avail = {t.lower() for t in s["tables"]}
            tables = [t for t in want if t.lower() in avail]
            if not tables:
                log.info("[%s] --tables 에 해당하는 테이블 없음, 건너뜀", s["name"])
                continue
        else:
            tables = s["tables"]
        log.info("[%s] %s/%s 테이블 %d개 스키마 조회", s["name"], s["server"], s["db"], len(tables))
        cols = fetch_columns(cp, s, tables)
        counts = fetch_counts(cp, s, list(cols))
        for t in cols:
            report.append("%-6s %-28s rows=%d" % (s["name"], t, counts[t]))
            log.info("[%s] %-28s rows=%d", s["name"], t, counts[t])
        if a.dry_run:
            continue
        os.makedirs(work_dir, exist_ok=True)
        groups: list[tuple[str, list[str]]] = []
        main_tables = [t for t in cols if t.lower() not in s["split_tables"]]
        if main_tables:
            groups.append(("%s_data_%s.sql" % (s["file_prefix"], s["name"]), main_tables))
        for t in cols:
            if t.lower() in s["split_tables"]:
                groups.append(("%s_data_%s_%s.sql" % (s["file_prefix"], s["name"], t.lower()), [t]))
        for fname, group in groups:
            path = os.path.join(out_dir, fname)
            with io.open(path, "w", encoding="utf-8", newline="\n") as f:
                f.write("-- %s : %s (%s/%s) → PostgreSQL. 생성 %s, 원본 SELECT 전용\n" % (fname, s["name"], s["server"], s["db"], stamp))
                f.write("-- 개인정보·접속정보 포함 가능 → git 제외, 이관 후 삭제\n")
                f.write("SET client_encoding TO 'UTF8';\nBEGIN;\n")
                trunc = [t.lower() for t in group if t.lower() not in s["drop_identity_tables"]]
                if trunc:
                    f.write("-- 재실행 가능: 이 파일이 적재하는 테이블을 먼저 비운다 (서로 FK 가 있는 테이블은 한 문장에 함께)\n")
                    f.write("TRUNCATE TABLE %s;\n" % ", ".join(trunc))
                for t in group:
                    skip = t.lower() in s["drop_identity_tables"]
                    bp = bcp_table(cp, s, t, cols[t], work_dir)
                    n, bad = convert(s, t, cols[t], bp, skip, f, s.get("codepage", "cp949"))
                    ok = "OK" if n == counts[t] else "MISMATCH"
                    report.append("%-6s %-28s src=%d inserted=%d skipped=%d %s%s" % (s["name"], t, counts[t], n, bad, ok, " (identity 제외)" if skip else ""))
                    log.info("[%s] %-28s inserted=%d (src %d) %s", s["name"], t, n, counts[t], ok)
                    for c in cols[t]:
                        if c["ident"] == "1" and not skip:
                            all_ident.append((t.lower(), c["name"].lower()))
                    if t.lower() not in [x.lower() for x in all_tables]:
                        all_tables.append(t)
                    if not a.keep_work:
                        try:
                            os.remove(bp)
                        except OSError:
                            pass
                f.write("COMMIT;\n")
            log.info("wrote %s (%d bytes)", path, os.path.getsize(path))

    if a.dry_run:
        print("\n".join(report))
        return 0

    after = os.path.join(out_dir, "13_after_insert.sql")
    with io.open(after, "w", encoding="utf-8", newline="\n") as f:
        f.write("-- 13_after_insert.sql : INSERT 적용 후 identity 시퀀스 보정 + ANALYZE + 건수\n")
        for t, c in all_ident:
            f.write("SELECT setval(pg_get_serial_sequence('%s', '%s'), COALESCE((SELECT MAX(%s) FROM %s), 0) + 1, false);\n" % (t, c, c, t))
        f.write("ANALYZE;\n")
        if all_tables:
            f.write("SELECT " + " UNION ALL SELECT ".join("'%s' AS t, COUNT(*) AS cnt FROM %s" % (t.lower(), t.lower()) for t in all_tables) + ";\n")

    # 스키마 파일 복사 + 적용 스크립트 (폴더만 서버로 옮기면 되도록)
    schema_dir = resolve_dir(a.config, cp.get("output", "schema_dir", fallback="../Source/onlic/db"))
    copied = []
    for fn in ("00_create_db.sql", "01_nasa_license_schema.sql"):
        src_f = os.path.join(schema_dir, fn)
        if os.path.exists(src_f):
            shutil.copyfile(src_f, os.path.join(out_dir, fn)); copied.append(fn)
        else:
            log.warning("스키마 파일 없음: %s", src_f)
    write_apply_scripts(out_dir, copied, cp)

    report.append("")
    report.append("elapsed %.1fs  out=%s" % (time.time() - t0, out_dir))
    io.open(os.path.join(out_dir, "_report.txt"), "w", encoding="utf-8", newline="\n").write("\n".join(report) + "\n")
    if not a.keep_work:
        try:
            os.rmdir(work_dir)
        except OSError:
            pass
    print("\n".join(report))
    return 0 if all("MISMATCH" not in r for r in report) else 1


if __name__ == "__main__":
    sys.exit(main())
