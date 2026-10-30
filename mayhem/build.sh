#!/usr/bin/env bash
#
# Builds the fuzz harnesses (sanitized, DWARF 3) + standalone reproducers, and the known-answer
# self-test (normal flags) that mayhem/test.sh runs. vesc_tool itself is a full Qt GUI app; the
# fuzz surface is its self-contained parsers/codecs (minimp3, lzokay, heatshrink, maddy, Packet),
# compiled here from source against Qt5Core only. All dependencies are apt packages baked into the
# image by mayhem/Dockerfile, so the build re-runs offline.
set -euo pipefail

[ -n "${SOURCE_DATE_EPOCH:-}" ] || unset SOURCE_DATE_EPOCH

: "${SANITIZER_FLAGS=-fsanitize=address,undefined -fno-sanitize-recover=all -fno-omit-frame-pointer}"
: "${DEBUG_FLAGS:=-g -gdwarf-3}"
: "${CC:=clang}" ; : "${CXX:=clang++}" ; : "${LIB_FUZZING_ENGINE:=-fsanitize=fuzzer}"
: "${MAYHEM_JOBS:=$(nproc)}"
: "${COVERAGE_FLAGS=}"
export SANITIZER_FLAGS DEBUG_FLAGS CC CXX LIB_FUZZING_ENGINE MAYHEM_JOBS COVERAGE_FLAGS

: "${SRC:=/mayhem}"
cd "$SRC"

# `*build-*` is already in upstream's .gitignore, so `git clean -ffdX` removes this dir.
B="$SRC/build-mayhem"
OUT="$SRC"
rm -rf "$B"
mkdir -p "$B/fuzz" "$B/standalone" "$B/oracle"

waitall() { local p; for p in $(jobs -p); do wait "$p" || exit 1; done; }

QT_CFLAGS="$(pkg-config --cflags Qt5Core) -fPIC"
QT_LIBS="$(pkg-config --libs Qt5Core)"
MOC="$(pkg-config --variable=host_bins Qt5Core)/moc"

"$MOC" packet.h -o "$B/moc_packet.cpp"

# $CC $SANITIZER_FLAGS $DEBUG_FLAGS -c "$STANDALONE_FUZZ_MAIN": C object so the C++ harnesses keep C linkage.
$CC $SANITIZER_FLAGS $DEBUG_FLAGS -c "$STANDALONE_FUZZ_MAIN" -o "$B/standalone/main.o"

LIB_C=(heatshrink/heatshrink_decoder.c heatshrink/heatshrink_encoder.c)
LIB_CXX=(lzokay/lzokay.cpp heatshrink/heatshrinkif.cpp packet.cpp "$B/moc_packet.cpp")

# Sanitized project objects. The fuzz set also carries SanCov (fuzzer-no-link) so the library code
# itself is covered; the standalone set has no libFuzzer runtime to satisfy it.
compile_set() {
    local dir="$1"; shift
    local extra=("$@") src
    for src in "${LIB_C[@]}"; do
        $CC $SANITIZER_FLAGS $DEBUG_FLAGS "${extra[@]}" -fPIC -O1 -I"$SRC" \
            -c "$src" -o "$dir/$(basename "$src").o" &
    done
    for src in "${LIB_CXX[@]}"; do
        # lzokay's get_le16 does an unaligned uint16_t load on every stream (benign on x86, trips
        # UBSan `alignment` on every input), so that check is relaxed for this one file only.
        local relax=()
        [ "$src" = lzokay/lzokay.cpp ] && relax=(-fno-sanitize=alignment)
        $CXX $SANITIZER_FLAGS $DEBUG_FLAGS "${extra[@]}" "${relax[@]}" $QT_CFLAGS -std=c++17 -O1 -I"$SRC" -I"$B" \
            -c "$src" -o "$dir/$(basename "$src").o" &
    done
    $CXX $SANITIZER_FLAGS $DEBUG_FLAGS "${extra[@]}" -c mayhem/lsan_off.cc -o "$dir/lsan_off.o" &
    waitall
}
compile_set "$B/fuzz" -fsanitize=fuzzer-no-link
compile_set "$B/standalone"

LIB_OBJS=(heatshrink_decoder.c.o heatshrink_encoder.c.o lzokay.cpp.o heatshrinkif.cpp.o packet.cpp.o moc_packet.cpp.o)
objs() { local dir="$1" o; for o in "${LIB_OBJS[@]}"; do printf '%s/%s\n' "$dir" "$o"; done; }

# link <name> <source> <cc|cxx> [needs project objects: 1]
link() {
    local name="$1" src="$2" lang="$3" proj="$4" comp flags=()
    if [ "$lang" = c ]; then comp="$CC"; else comp="$CXX"; flags=($QT_CFLAGS -std=c++17); fi
    local fo=() so=()
    if [ "$proj" = 1 ]; then mapfile -t fo < <(objs "$B/fuzz"); mapfile -t so < <(objs "$B/standalone"); fi
    $comp $SANITIZER_FLAGS $DEBUG_FLAGS $LIB_FUZZING_ENGINE "${flags[@]}" -O1 -I"$SRC/mayhem/stubs" -I"$SRC" -I"$SRC/minimp3" \
        "$src" "${fo[@]}" "$B/fuzz/lsan_off.o" $QT_LIBS -lm -o "$OUT/$name"
    $comp $SANITIZER_FLAGS $DEBUG_FLAGS "${flags[@]}" -O1 -I"$SRC/mayhem/stubs" -I"$SRC" -I"$SRC/minimp3" \
        "$src" "$B/standalone/main.o" "${so[@]}" "$B/standalone/lsan_off.o" $QT_LIBS -lm -o "$OUT/$name-standalone"
}

link fuzz_minimp3    mayhem/harnesses/fuzz_minimp3.c    c   0 &
link fuzz_lzokay     mayhem/harnesses/fuzz_lzokay.cpp   cxx 1 &
link fuzz_heatshrink mayhem/harnesses/fuzz_heatshrink.cpp cxx 1 &
link fuzz_maddy      mayhem/harnesses/fuzz_maddy.cpp    cxx 0 &
link fuzz_packet     mayhem/harnesses/fuzz_packet.cpp   cxx 1 &
waitall

# Oracle: the same sources with the project's NORMAL flags (no sanitizers, no DWARF pin).
$CC -O2 $COVERAGE_FLAGS -fPIC -I"$SRC" -c heatshrink/heatshrink_decoder.c -o "$B/oracle/hsd.o"
$CC -O2 $COVERAGE_FLAGS -fPIC -I"$SRC" -c heatshrink/heatshrink_encoder.c -o "$B/oracle/hse.o"
$CXX -O2 $COVERAGE_FLAGS $QT_CFLAGS -std=c++17 -I"$SRC/mayhem/stubs" -I"$SRC" -I"$SRC/minimp3" -I"$B" \
    mayhem/selftest.cpp lzokay/lzokay.cpp heatshrink/heatshrinkif.cpp packet.cpp "$B/moc_packet.cpp" \
    "$B/oracle/hsd.o" "$B/oracle/hse.o" $QT_LIBS -lm -o "$OUT/mayhem-selftest"
