#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
framing_sim.py - Windows TrsSocketServer ReadMsgProc/SearchSocketSTX_ETX/CutSocketSTX_ETX 의 참조 모델 (Python)
  * 바이트 규칙을 원본 그대로 재현해 테스트 벡터(framing_vectors.txt)를 만든다
  * Rocky 9 에서 framing_test (C++) 가 같은 벡터로 Framing.cpp 를 검증한다
사용: python framing_sim.py            → 케이스 실행 + framing_vectors.txt 생성
"""
import sys, os

MAXLENGTH = 500
RESIDUAL_MAX = MAXLENGTH * 2
STX, ETX, SEP = b"\x02", b"\x03", b"\xff"

def c_str(b: bytes) -> bytes:
    """C strlen 기준: 첫 NUL 까지"""
    i = b.find(b"\x00")
    return b if i < 0 else b[:i]

def search_stx_etx(data: bytes):
    start = end = -1
    i = 0
    while i < len(data):
        if data[i:i+1] == STX:
            start = i
            j = i + 1
            while j < len(data):
                if data[j:j+1] == STX:
                    start = j
                elif data[j:j+1] == ETX:
                    end = j
                    break
                j += 1
            break
        i += 1
    return start, end

def cut_stx_etx(data: bytes, start: int, end: int, frames: list):
    """returns (len, new_data)"""
    if start != -1 and end != -1:
        ln = end - start + 1
        cmd = data[start+1:start+3]
        # C atoi: 선행 숫자만
        s = cmd.decode("latin1")
        num = ""
        for ch in s:
            if ch.isdigit() or (ch in "+-" and not num):
                num += ch
            else:
                break
        try:
            ret = int(num) if num not in ("", "+", "-") else 0
        except ValueError:
            ret = 0
        payload = data[start+1:end]
        frames.append((ret, payload))
        return ln, data[end+1:]
    elif start != -1 and end == -1:
        return 0, data[start:]
    else:
        return 0, b""

def on_receive(residual: bytes, rdata: bytes, frames: list) -> bytes:
    """원본 ReadMsgProc. residual/rdata 는 C 문자열(NUL 이후 무시). 반환 = 새 residual"""
    rdata = c_str(rdata[:MAXLENGTH])
    residual = c_str(residual[:RESIDUAL_MAX])
    ln = len(residual) + len(rdata)
    temp = residual + rdata if ln < MAXLENGTH * 2 else rdata
    while True:
        start, end = search_stx_etx(temp)
        ln, temp = cut_stx_etx(temp, start, end, frames)
        if ln <= 0:
            return temp if len(temp) < MAXLENGTH else b""

def udp_extract(rdata: bytes):
    rdata = c_str(rdata[:MAXLENGTH])
    start, end = search_stx_etx(rdata)
    if start != -1 and end != -1:
        return rdata[start+1:end]
    return None

def hangul_strcut(src: bytes, nmax: int) -> bytes:
    out = bytearray(); i = 0; j = 0
    while i < len(src):
        if src[i] & 0x80:
            if j + 2 > nmax: break
            out.append(src[i]); i += 1; j += 1
            if i >= len(src): break
            out.append(src[i]); j += 1
        else:
            if j + 1 > nmax: break
            out.append(src[i]); j += 1
        i += 1
    return bytes(out)

# ---------------- 테스트 케이스 ----------------
LIC = STX + b"IVR" + SEP + b"001122334455IP:10.0.0.1" + ETX          # 6321 요청 (프로토콜명세 §3.1)
VRS = STX + b"VRS" + SEP + b"aabbccddeeffIP:192.168.0.5" + ETX
HB  = STX + b"3" + ETX                                               # 6770 heartbeat (payload 첫 글자 '3')
UDP = STX + b"001" + SEP + b"100" + SEP + b"10.0.0.1" + SEP + b"LINK" + ETX   # UDP LINK_CHECK

CASES = [
    ("single_frame",        [LIC]),
    ("two_frames_one_chunk",[LIC + VRS]),
    ("split_two_chunks",    [LIC[:12], LIC[12:]]),
    ("garbage_before_stx",  [b"abc" + LIC]),
    ("double_stx",          [STX + b"aaa" + STX + b"bbb" + ETX]),        # 마지막 STX~첫 ETX → "bbb"
    ("heartbeat",           [HB]),
    ("nul_inside_chunk",    [LIC[:8] + b"\x00" + LIC[8:], LIC]),         # NUL 이후 무시 (원본 quirk)
    ("etx_missing_small",   [STX + b"IVR" + SEP + b"partial"]),          # 잔여 보관
    ("etx_missing_large",   [STX + b"X" * 498]),                         # 잔여 499 → 보관(<500)
    ("etx_missing_500",     [STX + b"X" * 499, b"Y" * 10]),              # 1차 500B → 폐기, 2차는 Y 만 남음
    ("residual_overflow",   [STX + b"A" * 400, STX + b"B" * 400, b"C" * 250 + ETX]), # 400+401<1000 → 합침, 이후 801+251>1000 → 잔여 폐기
    ("three_frames_split",  [LIC + VRS[:5], VRS[5:] + HB]),
    ("frame_at_500_boundary",[(STX + b"Z" * 497 + ETX)[:500], (STX + b"Z" * 497 + ETX)[500:]]),  # 정확히 500 바이트 프레임 분할
    ("empty_frame",         [STX + ETX]),
    ("only_etx",            [b"abc" + ETX + LIC]),
]

def run():
    lines = []
    ok = True
    for name, chunks in CASES:
        residual = b""
        all_frames = []
        per_chunk = []
        for ch in chunks:
            frames = []
            residual = on_receive(residual, ch, frames)
            per_chunk.append(frames)
            all_frames.extend(frames)
        fr = ";".join("%d:%s" % (r, p.hex()) for r, p in all_frames)
        lines.append("%s|%s|%s|%s" % (name, ",".join(c.hex() for c in chunks), fr, residual.hex()))
        print("[%s] chunks=%d frames=%d residual=%r" % (name, len(chunks), len(all_frames), residual[:40]))
        for r, p in all_frames:
            print("    cmd=%d payload=%r" % (r, p[:60]))
    # UDP / HangulStrCut 벡터
    u = udp_extract(UDP)
    lines.append("udp_extract|%s|0:%s|" % (UDP.hex(), u.hex()))
    print("[udp_extract] payload=%r" % u)
    kr = "온피아-192.168.0.1에서 onwatch 프로세스 장애가 발생 했습니다. 추가문구추가문구추가문구".encode("cp949")
    cut = hangul_strcut(kr, 80)
    lines.append("hangul_cut_80|%s|0:%s|" % (kr.hex(), cut.hex()))
    print("[hangul_cut_80] %d bytes -> %d bytes : %s" % (len(kr), len(cut), cut.decode("cp949", "replace")))
    assert len(cut) <= 80 and cut.decode("cp949")  # 경계에서 깨지지 않음
    out = os.path.join(os.path.dirname(os.path.abspath(__file__)), "framing_vectors.txt")
    with open(out, "w", newline="\n") as f:
        f.write("# name|chunk_hex[,chunk_hex...]|cmd:payload_hex[;...]|residual_hex\n")
        f.write("\n".join(lines) + "\n")
    print("wrote", out, len(lines), "vectors")
    return ok

if __name__ == "__main__":
    sys.exit(0 if run() else 1)
