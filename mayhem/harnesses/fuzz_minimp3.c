// libFuzzer port of upstream's minimp3/fuzzing/fuzz.c, widened to the loader vesc_tool's
// QMinimp3 uses (mp3dec_load_buf, float output) and the seekable ex decoder.
#define MINIMP3_FLOAT_OUTPUT
#define MINIMP3_IMPLEMENTATION
#include "minimp3_ex.h"
#include <stdint.h>
#include <stdlib.h>

int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size)
{
    if (size < 4) {
        return -1;
    }

    static mp3dec_t mp3d;
    mp3dec_frame_info_t info;
    mp3d_sample_t pcm[MINIMP3_MAX_SAMPLES_PER_FRAME];

    mp3dec_init(&mp3d);
    size_t off = 0;
    while (off < size) {
        int n = mp3dec_decode_frame(&mp3d, data + off, (int)(size - off), pcm, &info);
        if (info.frame_bytes <= 0) {
            break;
        }
        off += (size_t)info.frame_bytes;
        (void)n;
    }

    mp3dec_t dec;
    mp3dec_file_info_t finfo;
    mp3dec_init(&dec);
    if (mp3dec_load_buf(&dec, data, size, &finfo, 0, 0) == 0) {
        free(finfo.buffer);
    }

    mp3dec_ex_t ex;
    if (mp3dec_ex_open_buf(&ex, data, size, MP3D_SEEK_TO_SAMPLE) == 0) {
        mp3dec_ex_seek(&ex, ex.samples / 2);
        mp3d_sample_t buf[1152 * 2];
        mp3dec_ex_read(&ex, buf, 1152 * 2);
        mp3dec_ex_close(&ex);
    }
    return 0;
}
