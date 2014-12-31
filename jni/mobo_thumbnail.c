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
//		return ret;
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

	return ret;
}

//use ffmpeg generate file's thumbnail at gen_second(second)
int gen_thumbnail(const char *file, int gen_second, int gen_IDR_frame) {
	int ret = 0;
	AVPacket packet;
	int got_frame = 0;
	int64_t seek_target = 0;

	src_filename = file;
	/* register all formats and codecs */
	ffmpeg.av_register_all();

	/* open input file, and allocate format context */
	if ((ret = ffmpeg.avformat_open_input(&fmt_ctx, src_filename, NULL, NULL))
			< 0) {
		fprintf(stderr, "Could not open source file %s\n", src_filename);
		char error[500];
		ffmpeg.av_strerror(ret, error, 500);

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
		ret = -5;
		goto end;
	}

	/* dump input information to stderr */
	ffmpeg.av_dump_format(fmt_ctx, 0, src_filename, 0);

	if (!video_stream) {
		fprintf(stderr,
				"Could not find audio or video stream in the input, aborting\n");
		ret = -3;
		goto end;
	}

	if (!frame) {
		fprintf(stderr, "Could not find frame\n");
		ret = -4;
		goto end;
	}

	if (gen_IDR_frame || gen_second > fmt_ctx->duration / AV_TIME_BASE)
		gen_second = 0;

	if (!gen_IDR_frame) {
		seek_target = gen_second * AV_TIME_BASE;
		seek_target = ffmpeg.av_rescale_q(seek_target, AV_TIME_BASE_Q,
				video_stream->time_base);
//	ffmpeg.avformat_seek_file(fmt_ctx, video_stream_idx, 0, seek_target,
//			seek_target, //INT64_MAX
//			AVSEEK_FLAG_BACKWARD|AVSEEK_FLAG_ANY); //AVSEEK_FLAG_FRAME

//		LOG("gen_thumbnail-->seek_target=%lld+++time_base->num=%d+++time_base->den=%d", seek_target, video_stream->time_base.num, video_stream->time_base.den);

		if (seek_target < 0)
			seek_target = gen_second * 1000;
		ffmpeg.av_seek_frame(fmt_ctx, video_stream_idx, seek_target,
				AVSEEK_FLAG_BACKWARD); // | AVSEEK_FLAG_FRAME
	}

	int got_right_frame = 0;
	int flag = 0;
	while (!got_right_frame && ffmpeg.av_read_frame(fmt_ctx, &packet) >= 0) {
		if (packet.stream_index == video_stream_idx) {
			ffmpeg.avcodec_decode_video2(video_dec_ctx, frame, &got_frame,
					&packet);
			if (got_frame) {
				if (!gen_IDR_frame) {
//					LOG("gen_thumbnail++%lld++%lld", frame->pkt_pts, seek_target);
					int64_t current_time = ffmpeg.av_rescale_q(frame->pkt_pts,
							video_stream->time_base, AV_TIME_BASE_Q);
					float time_diff = (float) current_time / AV_TIME_BASE
							- gen_second;
//						LOG("gen_thumbnail-->current_time=%lld----seek_target=%lld----gen_second=%d---time_diff=%f", current_time, seek_target, gen_second, time_diff);
					if (time_diff > -0.03) { //&& time_diff < 0.03
						got_right_frame = 1;
					} else {
						if (time_diff >= -10) {
							ffmpeg.av_frame_free(&frame);
							frame = NULL;
							frame = ffmpeg.av_frame_alloc();
							if (!frame)
								break;
						} else { //if (time_diff > 10 || time_diff < -10)
							got_right_frame = 1;
						}
					}
				} else {
					if (frame->pict_type == AV_PICTURE_TYPE_I) {
						got_right_frame = 1;
					}
				}
			}
		}
		ffmpeg.av_free_packet(&packet);
	}
	if (gen_IDR_frame && !got_right_frame) {
		ffmpeg.avcodec_decode_video2(video_dec_ctx, frame, &got_frame, &packet);
		ffmpeg.av_free_packet(&packet);
	}

	return ret;

end:
    if (frame) {
		ffmpeg.av_frame_free(&frame);
		frame = NULL;
	}

	if (video_dec_ctx != NULL) {
		ffmpeg.avcodec_close(video_dec_ctx);
		video_dec_ctx = NULL;
	}
	if (fmt_ctx) {
		ffmpeg.avformat_close_input(&fmt_ctx);
		fmt_ctx = NULL;
	}

	return ret;
}

AVPicture *get_rgb24_picture(const char *file, int gen_second, int *width,
		int *height, int gen_IDR_frame) {
	int ret = 0;
	frame = ffmpeg.av_frame_alloc();
	if (!frame) {
		return NULL;
	}

	ret = gen_thumbnail(file, gen_second, gen_IDR_frame);

	if (ret < 0) {
		if (frame)
			ffmpeg.av_frame_free(&frame);
		return NULL;
	}

	if (*width <= 0 || *height <= 0) {
		*width = (video_dec_ctx->width);
		*height = (video_dec_ctx->height);
	}
//	LOG("gen_thumbnail---width=%d,height=%d...", *width, *height);

	ret = ffmpeg.avpicture_alloc(&picture, AV_PIX_FMT_ARGB, *width, *height);

	if (ret < 0) {
		if (frame)
			ffmpeg.av_frame_free(&frame);
		return NULL;
	}

	sws_context = ffmpeg.sws_getCachedContext(sws_context, video_dec_ctx->width,
			video_dec_ctx->height, video_dec_ctx->pix_fmt, *width, *height,
			AV_PIX_FMT_ARGB, SWS_FAST_BILINEAR, NULL, NULL, NULL);

	ffmpeg.sws_scale(sws_context, (const uint8_t **) frame->data,
			frame->linesize, 0, video_dec_ctx->height, picture.data,
			picture.linesize);

	if (frame) {
		ffmpeg.av_frame_free(&frame);
		frame = NULL;
	}
	if (video_dec_ctx) {
		ffmpeg.avcodec_close(video_dec_ctx);
		video_dec_ctx = NULL;
	}
	if (fmt_ctx) {
		ffmpeg.avformat_close_input(&fmt_ctx);
		fmt_ctx = NULL;
	}

	AVPicture *p = &picture;
	return p;
}

void free_avpicture(AVPicture *picture) {
	ffmpeg.avpicture_free(picture);
}

