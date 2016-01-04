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

#ifndef MOBO_THUMBNAIL_FLAG
#define MOBO_THUMBNAIL_FLAG

#include "cmp_ffmpeg.h"

struct mobo_thumbnail_data{
	AVFormatContext *fmt_ctx;
	AVCodecContext *video_dec_ctx;
	AVStream *video_stream;
	struct SwsContext *sws_context;
	AVFrame *frame;
	AVPicture picture;
	const char *src_filename;
	int video_stream_idx;
	int need_key_frame;
};

typedef struct mobo_thumbnail_data mobo_thumbnail_data;

int gen_thumbnail(const char *file, int gen_second, int gen_IDR_frame,mobo_thumbnail_data *thumbnail_data) ;
AVPicture *get_rgb24_picture(const char *file, int gen_second, int *width,
		int *height, int gen_IDR_frame,mobo_thumbnail_data *thumbnail_data, int dst_img_format) ;
int write_png_file(char* file_name,int width,int height,unsigned short* buf);
#endif
