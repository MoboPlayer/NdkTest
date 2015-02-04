/*
 * Copyright (c) 2013 Stefano Sabatini
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

/**
 * @file
 * libavformat/libavcodec demuxing and muxing API example.
 *
 * Remux streams from one container format to another.
 * @example remuxing.c
 */

#include "mobo_download_remuxing.h"

/**下载视频的总时长**/
int streaming_duration = 0;
/**下载开始时间点**/
int start_downloaded_time = -1;
/**当前下载到的时间点**/
int current_time_downloaded_to = 0;

/**-1：暂停；0：停止；1：开始**/
int download_flag = 1;
#define ATTACH_ERROR    -4
#define ATTACH_SUCCESS  0

static pthread_mutex_t* download_mutex;
static pthread_cond_t* download_cond;

static int JniThreadAttach(int *needDetach, JNIEnv **_env) {
	int getEnvStat;

	if (_env == NULL)
		return ATTACH_ERROR;

	*needDetach = 0;
	getEnvStat = (*jvm)->GetEnv(jvm, _env, JNI_VERSION_1_6);
	if (getEnvStat == JNI_EDETACHED) {
		LOG("JNI_EDETACHED");
		if ((*jvm)->AttachCurrentThread(jvm, (void **) _env, NULL) == 0) {
			*needDetach = 1;
			return ATTACH_SUCCESS;
		} else {
			LOG("Failed to attach");
			return ATTACH_ERROR;
		}
	} else if (getEnvStat == JNI_OK) {
		LOG("JNI_OK");
	} else if (getEnvStat == JNI_EVERSION) {
		LOG("JNI_EVERSION");
	}

	return ATTACH_SUCCESS;
}

static void JniThreadDetach(int needDetach) {
	if (needDetach)
		(*jvm)->DetachCurrentThread(jvm);
}

static void log_packet(const AVFormatContext *fmt_ctx, const AVPacket *pkt,
		const char *tag) {
//	AVRational *time_base = &fmt_ctx->streams[pkt->stream_index]->time_base;
//
//	LOG("download_streaming-->%s: pts:%s pts_time:%s dts:%s dts_time:%s duration:%s duration_time:%s stream_index:%d\n",
//			tag, av_ts2str(pkt->pts), av_ts2timestr(pkt->pts, time_base),
//			av_ts2str(pkt->dts), av_ts2timestr(pkt->dts, time_base),
//			av_ts2str(pkt->duration), av_ts2timestr(pkt->duration, time_base),
//			pkt->stream_index);
}
int init_mutex_cond() {

	if (!download_mutex) {
		download_mutex = (pthread_mutex_t *) malloc(sizeof(pthread_mutex_t));
		memset(download_mutex, 0, sizeof(pthread_mutex_t));
		pthread_mutex_init(download_mutex, NULL);
	}

	if (!download_cond) {
		download_cond = (pthread_cond_t*) malloc(sizeof(pthread_cond_t));
		memset(download_cond, 0, sizeof(pthread_cond_t));
		pthread_cond_init(download_cond, NULL);

	}
}

void des_mutex_cond() {

	if (download_cond) {
		pthread_cond_destroy(download_cond);
		free(download_cond);
		download_cond = NULL;
	}

	if (download_mutex) {
		pthread_mutex_destroy(download_mutex);
		free(download_mutex);
		download_mutex = NULL;
	}
}
int saving_network_media(const char *in_filename, const char *out_filename,
		int64_t skip_bytes) { //int64_t pts_array[]
	int fff = 0;

	AVOutputFormat *ofmt = NULL;
	AVFormatContext *ifmt_ctx = NULL, *ofmt_ctx = NULL;
	AVPacket pkt;
	int ret, i;
	int flag = 0;
	char error[500];

	ffmpeg.av_register_all();
	ffmpeg.avformat_network_init();

	if (skip_bytes > 0) {
		ifmt_ctx = ffmpeg.avformat_alloc_context();
		ifmt_ctx->skip_initial_bytes = skip_bytes;
	}

	if ((ret = ffmpeg.avformat_open_input(&ifmt_ctx, in_filename, 0, 0)) < 0) {
		ffmpeg.av_strerror(ret, error, 500);
		LOG("download_streaming-->avformat_open_input res=%s...", error);
		goto end;
	}

	if ((ret = ffmpeg.avformat_find_stream_info(ifmt_ctx, 0)) < 0) {
		fprintf(stderr, "Failed to retrieve input stream information");
		ffmpeg.av_strerror(ret, error, 500);
		LOG("download_streaming-->avformat_open_input res=%s...", error);
		goto end;
	}

	LOG("download_streaming-->avformat_open_input after");
	ffmpeg.av_dump_format(ifmt_ctx, 0, in_filename, 0);

	streaming_duration = ifmt_ctx->duration / AV_TIME_BASE;

	ret = ffmpeg.avformat_alloc_output_context2(&ofmt_ctx, NULL, NULL,
			out_filename);

	ffmpeg.av_strerror(ret, error, 500);
//	LOG("download_streaming-->after avformat_alloc_output_context2...%s",error);
	if (!ofmt_ctx) {
		fprintf(stderr, "Could not create output context\n");
		ret = AVERROR_UNKNOWN;
		LOG("download_streaming-->avformat_open_input res=%s...", error);
		goto end;
	}

	ofmt = ofmt_ctx->oformat;

	LOG("download_streaming-->before for");

//	if (skip_bytes > 0) {
//		LOG("download_streaming-->avio_skip position=%lld...", skip_bytes);
//		ffmpeg.avio_skip(ifmt_ctx->pb, skip_bytes);
//	}

	for (i = 0; i < ifmt_ctx->nb_streams; i++) {
//		if (pts_array){
//			LOG("download_streaming-->pts_array is not null");
//			int64_t last_pts = pts_array[i];
//			LOG("download_streaming-->seek out file's stream=%d&&&pts=%lld...",i,last_pts);
////		    ffmpeg.avformat_seek_file(ifmt_ctx,i,last_pts,last_pts,last_pts,AVSEEK_FLAG_ANY);
//		    ffmpeg.avio_skip(ifmt_ctx->pb,skip_bytes);
////		    ffmpeg.avio_seek_time(ifmt_ctx->pb,i,pts_array[i],AVSEEK_FLAG_ANY);
//			LOG("download_streaming-->seek out successed");
//		}
		AVStream *in_stream = ifmt_ctx->streams[i];
		AVStream *out_stream = ffmpeg.avformat_new_stream(ofmt_ctx,
				in_stream->codec->codec);
		if (!out_stream) {
			fprintf(stderr, "Failed allocating output stream\n");
			ret = AVERROR_UNKNOWN;
			goto end;
		}

		ret = ffmpeg.avcodec_copy_context(out_stream->codec, in_stream->codec);
		if (ret < 0) {
			fprintf(stderr,
					"Failed to copy context from input to output stream codec context\n");
			goto end;
		}
		out_stream->codec->codec_tag = 0;
		if (ofmt_ctx->oformat->flags & AVFMT_GLOBALHEADER)
			out_stream->codec->flags |= CODEC_FLAG_GLOBAL_HEADER;
	}
	ffmpeg.av_dump_format(ofmt_ctx, 0, out_filename, 1);

	if (!(ofmt->flags & AVFMT_NOFILE)) {
		ret = ffmpeg.avio_open(&ofmt_ctx->pb, out_filename, AVIO_FLAG_WRITE);
		if (ret < 0) {
			fprintf(stderr, "Could not open output file '%s'", out_filename);
			goto end;
		}
//		if (pts_array) {
//			for (i = 0; i < ofmt_ctx->nb_streams; i++) {
//				ffmpeg.avio_seek_time(ofmt_ctx->pb, i, pts_array[i],
//						AVSEEK_FLAG_ANY);
//			}
//		}

		if (skip_bytes > 0) {
			LOG("download_streaming-->avio_skip position=%lld...", skip_bytes);
			ffmpeg.avio_skip(ofmt_ctx->pb, skip_bytes);
		}
	}

	if (!skip_bytes) //pts_array
		ret = ffmpeg.avformat_write_header(ofmt_ctx, NULL);
	if (ret < 0) {
		fprintf(stderr, "Error occurred when opening output file\n");
		goto end;
	}

	LOG("download_streaming-->before while");
	while (download_flag) {
		if (download_flag == FLAG_DOWNLOAD_PAUSE) {
			pthread_mutex_lock(download_mutex);
			LOG("download_streaming-->pause download");
			pthread_cond_wait(download_cond, download_mutex);
			LOG("download_streaming-->resume download");
			pthread_mutex_unlock(download_mutex);
		}
		AVStream *in_stream, *out_stream;

		ret = ffmpeg.av_read_frame(ifmt_ctx, &pkt);
		if (ret < 0) {
			LOG("download_streaming-->while -- ret=%d...", ret);
			break;
		}

		in_stream = ifmt_ctx->streams[pkt.stream_index];
		out_stream = ofmt_ctx->streams[pkt.stream_index];

		log_packet(ifmt_ctx, &pkt, "in");
		if (fff == 0 && pkt.stream_index == 1) {
			fff = 1;
		} else {
			int current = ffmpeg.av_rescale_q(pkt.pts, out_stream->time_base,
					AV_TIME_BASE_Q) / AV_TIME_BASE;
			if (skip_bytes && start_downloaded_time == -1)
				start_downloaded_time = current;
			/* copy packet */
			pkt.pts = ffmpeg.av_rescale_q_rnd(pkt.pts, in_stream->time_base,
					out_stream->time_base,
					AV_ROUND_NEAR_INF | AV_ROUND_PASS_MINMAX);
			pkt.dts = ffmpeg.av_rescale_q_rnd(pkt.dts, in_stream->time_base,
					out_stream->time_base,
					AV_ROUND_NEAR_INF | AV_ROUND_PASS_MINMAX);
			pkt.duration = ffmpeg.av_rescale_q(pkt.duration,
					in_stream->time_base, out_stream->time_base);
			pkt.pos = -1;
			log_packet(ofmt_ctx, &pkt, "out");

			ret = ffmpeg.av_interleaved_write_frame(ofmt_ctx, &pkt);
			if (current > current_time_downloaded_to) {
				java_callback_onDownloadProgressChanged(ofmt_ctx->pb->pos,
						current_time_downloaded_to);
				current_time_downloaded_to = current;
			}
			if (ret < 0) {
				fprintf(stderr, "Error muxing packet\n");
				break;
			}
		}
		ffmpeg.av_free_packet(&pkt);
	}

	LOG("download_streaming-->end write......");
	if (ret == AVERROR_EOF) {
		LOG("download_streaming--> av_write_trailer......");
		ffmpeg.av_write_trailer(ofmt_ctx);
		java_callback_onDownloadFinished();
	}
	end: ffmpeg.av_strerror(ret, error, 500);
	LOG("download_streaming-->avformat_open_input res=%s...", error);
	ffmpeg.avformat_close_input(&ifmt_ctx);

	/* close output */
	if (ofmt_ctx && !(ofmt->flags & AVFMT_NOFILE)) {
		LOG("download_streaming-->avio_close......");
		ffmpeg.avio_close(ofmt_ctx->pb);
	}
	ffmpeg.avformat_free_context(ofmt_ctx);

	if (ret < 0 && ret != AVERROR_EOF) {
		char error[500];
		fprintf(stderr, "Error occurred: %s\n",
				ffmpeg.av_strerror(ret, error, 500));
		java_callback_onDownloadFailed();
		return 1;
	}

	return 0;
}

void set_download_flag(int flag) {

	download_flag = flag;
	if (flag == FLAG_DOWNLOAD_START) {
		pthread_cond_signal(download_cond);
	}
}

int get_download_flag() {
	return download_flag;
}

int get_duration() {
	return streaming_duration;
}

int get_current_time_downloaded_to() {
	return current_time_downloaded_to;
}

int get_start_downloaded_time() {
	return start_downloaded_time;
}

static void java_callback_onDownloadProgressChanged(int64_t pos, //AVPacket pkt,
		int current_time) {
	LOG("download_streaming-->onDownloadProgressChanged -- current_time=%d...", current_time);
	JNIEnv *env;
	jclass cls;
	jmethodID mid;

	int _needDetach = 0;
	int attachStatus = ATTACH_ERROR;

	attachStatus = JniThreadAttach(&_needDetach, &env);
	if (attachStatus == ATTACH_ERROR) {
		return ATTACH_ERROR;
	}

	cls = (*env)->GetObjectClass(env, java_object);
	if (cls == NULL)
		goto error;
	mid = (*env)->GetMethodID(env, cls, "onDownloadProgressChanged", "(JI)V");
	if (mid == NULL)
		goto error;
	(*env)->CallVoidMethod(env, java_object, mid, pos, //pkt.size,
			current_time);

	error: JniThreadDetach(_needDetach);
	(*env)->DeleteLocalRef(env, cls);
}

static void java_callback_onDownloadFinished() {
	JNIEnv *env;
	jclass cls;
	jmethodID mid;

	int _needDetach = 0;
	int attachStatus = ATTACH_ERROR;

	attachStatus = JniThreadAttach(&_needDetach, &env);
	if (attachStatus == ATTACH_ERROR) {
		return ATTACH_ERROR;
	}

	cls = (*env)->GetObjectClass(env, java_object);
	if (cls == NULL)
		goto error;
	mid = (*env)->GetMethodID(env, cls, "onDownloadFinished", "(I)V");
	if (mid == NULL)
		goto error;
	(*env)->CallVoidMethod(env, java_object, mid);

	error: JniThreadDetach(_needDetach);
}

static void java_callback_onDownloadFailed() {
	JNIEnv *env;
	jclass cls;
	jmethodID mid;

	int _needDetach = 0;
	int attachStatus = ATTACH_ERROR;

	attachStatus = JniThreadAttach(&_needDetach, &env);
	if (attachStatus == ATTACH_ERROR) {
		return ATTACH_ERROR;
	}

	cls = (*env)->GetObjectClass(env, java_object);
	if (cls == NULL)
		goto error;
	mid = (*env)->GetMethodID(env, cls, "onDownloadFailed", "(I)V");
	if (mid == NULL)
		goto error;
	(*env)->CallVoidMethod(env, java_object, mid);

	error: JniThreadDetach(_needDetach);
}

