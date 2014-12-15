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
#include "mobo_thumbnail.h"

static AVFormatContext *fmt_ctx = NULL;
static AVCodecContext *video_dec_ctx = NULL;
static AVStream *video_stream = NULL;
static struct SwsContext *sws_context = NULL;
static AVFrame *frame = NULL;
static AVPicture picture;
static const char *src_filename = NULL;

static int video_stream_idx = -1;

extern ffmpeg_func_t ffmpeg;

static int open_codec_context(int *stream_idx, AVFormatContext *fmt_ctx,
		enum AVMediaType type) {
	int ret;
	AVStream *st;
	AVCodecContext *dec_ctx = NULL;
	AVCodec *dec = NULL;
	AVDictionary *opts = NULL;

	ret = ffmpeg.av_find_best_stream(fmt_ctx, type, -1, -1, NULL, 0);
	if (ret < 0) {
		fprintf(stderr, "Could not find %s stream in input file '%s'\n",
				ffmpeg.av_get_media_type_string(type), src_filename);
		return ret;
	} else {
		*stream_idx = ret;
		st = fmt_ctx->streams[*stream_idx];

		/* find decoder for the stream */
		dec_ctx = st->codec;
		dec = ffmpeg.avcodec_find_decoder(dec_ctx->codec_id);
		if (!dec) {
			fprintf(stderr, "Failed to find %s codec\n",
					ffmpeg.av_get_media_type_string(type));
			return AVERROR(EINVAL);
		}

		if ((ret = ffmpeg.avcodec_open2(dec_ctx, dec, NULL)) < 0) {
			fprintf(stderr, "Failed to open %s codec\n",
					ffmpeg.av_get_media_type_string(type));
			return ret;
		}
	}

	return 0;
}

//use ffmpeg generate file's thumbnail at gen_second(second)
int gen_thumbnail(const char *file, int gen_second) {
	int ret = 0;
	AVPacket packet;
	int got_frame = 0;
	int64_t seek_target = 0;

	src_filename = file;
	/* register all formats and codecs */
	ffmpeg.av_register_all();

	/* open input file, and allocate format context */
	if ((ret=ffmpeg.avformat_open_input(&fmt_ctx, src_filename, NULL, NULL)) < 0) {
		fprintf(stderr, "Could not open source file %s\n", src_filename);
		 char error[500];
		 ffmpeg.av_strerror(ret, error, 500);

		LOG("gen_thumbnail---->%s",error);
		return -1;
	}

	/* retrieve stream information */
	if (ffmpeg.avformat_find_stream_info(fmt_ctx, NULL) < 0) {
		fprintf(stderr, "Could not find stream information\n");
		return -2;
	}

	if (open_codec_context(&video_stream_idx, fmt_ctx, AVMEDIA_TYPE_VIDEO)
			>= 0) {
		video_stream = fmt_ctx->streams[video_stream_idx];
		video_dec_ctx = video_stream->codec;
	} else {
		return -5;
	}

	/* dump input information to stderr */
	ffmpeg.av_dump_format(fmt_ctx, 0, src_filename, 0);

	if (!video_stream) {
		fprintf(stderr,
				"Could not find audio or video stream in the input, aborting\n");
		ret = -3;
		goto end;
	}

	if (gen_second > fmt_ctx->duration / AV_TIME_BASE)
		gen_second = 0;

	seek_target = gen_second * AV_TIME_BASE;
	seek_target = ffmpeg.av_rescale_q(seek_target, AV_TIME_BASE_Q,
			video_stream->time_base);
	ffmpeg.avformat_seek_file(fmt_ctx, video_stream_idx, 0, seek_target, INT64_MAX,
			AVSEEK_FLAG_FRAME);

	if (!frame) {
		fprintf(stderr, "Could not find frame\n");
		ret = -4;
		goto end;
	}

	while (!got_frame && ffmpeg.av_read_frame(fmt_ctx, &packet) >= 0) {

		if (packet.stream_index == video_stream_idx) {
			ffmpeg.avcodec_decode_video2(video_dec_ctx, frame, &got_frame, &packet);
		}

		ffmpeg.av_free_packet(&packet);
	}
	return ret;

end:
	ffmpeg.avcodec_close(video_dec_ctx);
	ffmpeg.avformat_close_input(&fmt_ctx);

	return ret;
}

AVPicture *get_rgb24_picture(const char *file, int gen_second, int *width,
		int *height) {
	int ret = 0;
	frame = ffmpeg.av_frame_alloc();
	if (!frame)
		return NULL;

	ret = gen_thumbnail(file, gen_second);

	if (ret < 0) {
		if (frame)
			ffmpeg.av_frame_free(&frame);
		return NULL;
	}

	ret = ffmpeg.avpicture_alloc(&picture, AV_PIX_FMT_ARGB, video_dec_ctx->width,
			video_dec_ctx->height);

	if (ret < 0) {
		if (frame)
			ffmpeg.av_frame_free(&frame);
		return NULL;
	}

	*width = video_dec_ctx->width;
	*height = video_dec_ctx->height;

	sws_context = ffmpeg.sws_getCachedContext(sws_context, video_dec_ctx->width,
			video_dec_ctx->height, video_dec_ctx->pix_fmt, video_dec_ctx->width,
			video_dec_ctx->height, AV_PIX_FMT_ARGB, SWS_FAST_BILINEAR, NULL, NULL,
			NULL);

	ffmpeg.sws_scale(sws_context, (const uint8_t **) frame->data, frame->linesize, 0,
			video_dec_ctx->height, picture.data, picture.linesize);

	if (frame) {
		ffmpeg.av_frame_free(&frame);
	}
	if (video_dec_ctx) {
//        avcodec_close(video_dec_ctx);
	}
	if (fmt_ctx) {
		ffmpeg.avformat_close_input(&fmt_ctx);
	}

	AVPicture *p = &picture;
	return p;
}

void free_avpicture(AVPicture *picture){
	ffmpeg.avpicture_free(picture);
}

