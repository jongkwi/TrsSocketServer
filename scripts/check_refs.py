"""분석 문서의 소스 참조(파일:라인)가 실제 소스와 일치하는지 검증하는 스크립트.

사용법:
    python scripts/check_refs.py docs/기능목록.md docs/프로토콜명세.md docs/포팅영향분석.md

동작:
    1. 문서에서 `파일명.cpp:NNN` / `파일명.h:NNN` / `:NNN-MMM` 패턴을 추출한다.
       (파일명 없이 `:NNN`만 있으면 TrsSocketServer.cpp 로 간주 — 문서 관례)
    2. 해당 소스(EUC-KR)를 cp949로 읽어 라인 범위가 파일 길이 안에 있는지 확인한다.
    3. 참조 라인 ±2 범위에 같은 표 행에서 언급된 식별자(백틱 안의 영문 심볼)가 있으면 OK,
       없으면 WARN 으로 출력한다. (심볼이 없는 참조는 라인 범위만 검사)
"""
from __future__ import annotations

import logging
import re
import sys
from pathlib import Path

SRC_DIR = Path(__file__).resolve().parent.parent / "Source" / "TrsSocketServer_Lic_VS2019_Atalk"
DEFAULT_FILE = "TrsSocketServer.cpp"
FILE_ALIAS = {"Dlg.cpp": "TrsSocketServerDlg.cpp"}
# 파일명(.cpp/.h/.vcxproj/.ini) 뒤 또는 백틱 뒤의 `:NNN`만 참조로 본다.
# `192.168.0.1:13` 같은 IP:port 는 ':' 앞이 숫자/점이므로 제외한다.
REF_RE = re.compile(
    r"(?:([A-Za-z_]+\.(?:cpp|h|vcxproj|ini))\s*)?`?(?<![0-9.])"
    r":(\d+)(?:[-~,](\d+))?`?"
)
SYM_RE = re.compile(r"`([A-Za-z_][A-Za-z0-9_:]{3,})`")
CONTEXT = 2
# 파일명 없이 `:NNNN` 으로 나오는 포트 번호는 라인 참조가 아니다 (lic.onpia.co.kr:6321 등)
PORT_NUMBERS = {"6321", "6770", "5363", "5360", "5362", "5357", "1433", "34333", "39999", "5432", "20001"}

logging.basicConfig(level=logging.INFO, format="%(levelname)s %(message)s")
log = logging.getLogger("check_refs")


def load_source(name: str) -> list[str] | None:
    """소스 파일을 cp949로 읽어 라인 리스트로 반환한다. 없으면 None."""
    path = SRC_DIR / FILE_ALIAS.get(name, name)
    if not path.exists():
        return None
    return path.read_text(encoding="cp949", errors="replace").splitlines()


def check_line(doc_line: str, cache: dict) -> tuple[int, int, int]:
    """문서 한 줄의 참조를 검사한다. (ok, warn, err) 개수 반환."""
    ok = warn = err = 0
    symbols = [s.split("::")[-1] for s in SYM_RE.findall(doc_line)]
    current_file = DEFAULT_FILE
    for fname, start, end in REF_RE.findall(doc_line):
        if not fname and start in PORT_NUMBERS and not end:
            continue
        if fname:
            current_file = fname
        lines = cache.setdefault(current_file, load_source(current_file))
        if lines is None:
            # 이 저장소에 없는 파일(onmw·OnDemon·ncbk 등 외부 클라이언트 소스)은 검증 불가 → WARN
            log.warning("외부 소스(검증 생략): %s (참조 :%s)", current_file, start)
            warn += 1
            continue
        s, e = int(start), int(end or start)
        if e < s:  # ":1016-1033" 처럼 끝이 더 작으면 (예: ":874,1191") 단일 라인 두 개로 취급
            e = s
        if s < 1 or e > len(lines):
            log.error("범위 초과: %s:%d-%d (파일 %d줄)", current_file, s, e, len(lines))
            err += 1
            continue
        if not symbols:
            ok += 1
            continue
        window = "\n".join(lines[max(0, s - 1 - CONTEXT): min(len(lines), e + CONTEXT)])
        if any(sym in window for sym in symbols):
            ok += 1
        else:
            log.warning("심볼 미발견: %s:%d-%d  symbols=%s", current_file, s, e, symbols[:4])
            warn += 1
    return ok, warn, err


def main(doc_paths: list[str]) -> int:
    """문서들을 검사하고 요약을 출력한다. 오류가 있으면 1 반환."""
    cache: dict = {}
    total = [0, 0, 0]
    for doc in doc_paths:
        text = Path(doc).read_text(encoding="utf-8")
        counts = [0, 0, 0]
        for line in text.splitlines():
            if ":" not in line:
                continue
            r = check_line(line, cache)
            counts = [a + b for a, b in zip(counts, r)]
        log.info("%s → OK %d / WARN %d / ERR %d", doc, *counts)
        total = [a + b for a, b in zip(total, counts)]
    log.info("합계 → OK %d / WARN %d / ERR %d", *total)
    return 1 if total[2] else 0


if __name__ == "__main__":
    if len(sys.argv) < 2:
        print(__doc__)
        sys.exit(2)
    sys.exit(main(sys.argv[1:]))
