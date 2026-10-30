#!/usr/bin/env bash
#
# mayhem/test.sh — RUN the known-answer suite (mayhem/selftest.cpp, built by mayhem/build.sh) over the
# vesc_tool code the fuzz targets cover. Each `KAT` line it prints is compared to a golden value here, in
# bash, so a neutered binary (no output) fails every case.
#
# IMPORTANT:
#  * Must assert BEHAVIOR/OUTPUT, not just exit status. The oracle has to check asserted values /
#    golden-output diffs / known-answer results — so a PATCH that "fixes" a bug by making the program
#    exit(0) (or any no-op) FAILS here. Running inputs and checking only "exit 0 / didn't crash" is
#    NOT a functional test (it's trivially reward-hackable) — use the project's real assertion suite.
#  * Do NOT build here — mayhem/build.sh already compiled the test suite (with the project's normal
#    flags). This script only RUNS the pre-built tests and reports counts. If the test runner is
#    missing, that's a build.sh bug — fail loudly rather than silently rebuilding.
#  * REQUIRED OUTPUT — a CTRF (https://ctrf.io) summary so Mayhem/the PATCH grader reads the counts:
#      - writes a CTRF JSON report to ${CTRF_REPORT:-$SRC/ctrf-report.json}, and
#      - prints a one-line `CTRF {...}` marker to stdout (same JSON, compact).
#    Only `results.summary` (with tests/passed/failed/pending/skipped/other) is required.
#    Use the emit_ctrf helper below; it computes tests = passed+failed+skipped and sets the exit
#    code (0 iff failed==0). Map your framework's output to passed/failed/skipped.
set -uo pipefail
[ -n "${SOURCE_DATE_EPOCH:-}" ] || unset SOURCE_DATE_EPOCH
: "${MAYHEM_JOBS:=$(nproc)}"   # build parallelism; env-overridable, falls back to nproc (use -j"$MAYHEM_JOBS")
cd "$SRC"

# emit_ctrf <tool> <passed> <failed> [skipped] [pending] [other]
# Writes a CTRF report (file + stdout `CTRF {...}` marker) and returns non-zero iff failed>0.
emit_ctrf() {
  local tool="$1" passed="$2" failed="$3" skipped="${4:-0}" pending="${5:-0}" other="${6:-0}"
  local tests=$(( passed + failed + skipped + pending + other ))
  cat > "${CTRF_REPORT:-$SRC/ctrf-report.json}" <<JSON
{
  "results": {
    "tool": { "name": "$tool" },
    "summary": {
      "tests": $tests,
      "passed": $passed,
      "failed": $failed,
      "pending": $pending,
      "skipped": $skipped,
      "other": $other
    }
  }
}
JSON
  printf 'CTRF {"results":{"tool":{"name":"%s"},"summary":{"tests":%d,"passed":%d,"failed":%d,"pending":%d,"skipped":%d,"other":%d}}}\n' \
    "$tool" "$tests" "$passed" "$failed" "$pending" "$skipped" "$other"
  [ "$failed" -eq 0 ]
}


RUNNER="$SRC/mayhem-selftest"
[ -x "$RUNNER" ] || { echo "missing $RUNNER (mayhem/build.sh should have built it)" >&2; emit_ctrf "kat" 0 1 0; exit $?; }

ACTUAL="$("$RUNNER" 2>&1)"

# name | golden line
EXPECTED=(
  'KAT mp3_frame samples=1152 frame_bytes=417 hz=44100 channels=2 layer=3 bitrate=128'
  'KAT mp3_load ret=0 samples=4608 hz=44100 channels=2'
  'KAT lzo_compress ret=0 len=14 data=1261201f02006263270800110000'
  'KAT lzo_roundtrip ret=0 len=76 same=1'
  'KAT heatshrink enc=b4596d96cb7c80005800 dec=hello hello hello hello'
  'KAT maddy <h1>Title</h1><p>Some <i>italic</i> and <strong>bold</strong> text. </p>'
  'KAT crc16 31c3'
  'KAT packet_frame 0203010203613103'
  'KAT packet_rx count=1 payload=010203'
)

passed=0; failed=0
for want in "${EXPECTED[@]}"; do
  if grep -qxF -- "$want" <<<"$ACTUAL"; then
    passed=$((passed + 1))
  else
    failed=$((failed + 1)); echo "FAIL: expected line: $want" >&2
  fi
done
[ "$failed" -eq 0 ] || { echo "--- selftest output ---" >&2; echo "$ACTUAL" >&2; }
emit_ctrf "vesc_tool-kat" "$passed" "$failed" 0
