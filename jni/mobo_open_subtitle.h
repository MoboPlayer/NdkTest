/**
 * The MIT License (MIT)
 *
 * Copyright (c) 2014 MoboPlayer.com
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 */
#include "cmp_ffmpeg.h"

#ifndef SUB_FFMPEG_H_
#define SUB_FFMPEG_H_

typedef struct SubtitleData {
    AVFormatContext     *fmt_ctx;
    AVCodecContext      *dec_ctx;
    AVStream            *stream;
    AVSubtitle          *subtitles_array;
    int                 subtitle_index;
    int                 subtitle_size;
}SubtitleData;

typedef SubtitleData *sub_data_p;

int open_subtitle(const char *file, int stream_index, int subtiltle_index);
char *get_subtitle_ontime(int cur_time, int subtiltle_index);
void close_subtitle(int subtiltle_index);
int is_subtitle_exits(const char *file);
//char *get_subtitle_language(int subtiltle_index);
int get_subtitle_type(int subtiltle_index);
char *sj_get_sa_info(const char *file, char *text);
#endif
