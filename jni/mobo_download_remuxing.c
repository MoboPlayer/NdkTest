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
int current_stream_index = 0;

/**-1：暂停；0：停止；1：开始**/
int download_flag = 1;
#define ATTACH_ERROR    -4
#define ATTACH_SUCCESS  0

#define MAX_STREAM_COUNT  15

static pthread_mutex_t* download_mutex;
static pthread_cond_t* download_cond;

static int JniThreadAttach(int *needDetach, JNIEnv **_env) {
	int getEnvStat;

	if (_env == NULL)
		return ATTACH_ERROR;

	*needDetach = 0;
	getEnvStat = (*jvm)->GetEnv(jvm, _env, JNI_VERSION_1_6);
	if (getEnvStat == JNI_EDETACHED) {
//		LOG("JNI_EDETACHED");
		if ((*jvm)->AttachCurrentThread(jvm, (void **) _env, NULL) == 0) {
			*needDetach = 1;
			return ATTACH_SUCCESS;
		} else {
//			LOG("Failed to attach");
			return ATTACH_ERROR;
		}
	} else if (getEnvStat == JNI_OK) {
//		LOG("JNI_OK");
	} else if (getEnvStat == JNI_EVERSION) {
//		LOG("JNI_EVERSION");
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

static int64_t get_file_size(char *input_url) {
	AVIOContext *input;
	int res = avio_open2(&input, input_url, AVIO_FLAG_READ, NULL, NULL);
	if (res)
		return 0;
	return avio_size(input);
}

char* str_join(char *s1, char *s2)
{
    char *result = malloc(strlen(s1)+strlen(s2)+1);
    if (result == NULL) exit (1);

    strcpy(result, s1);
    strcat(result, s2);

    return result;
}
static void rewrite_exist_file(char *out_file, AVFormatContext *ofmt_ctx) {
	AVFormatContext *ifmt_ctx_exist = NULL;
	AVPacket pkt;
	char error[500];
	int ret;
	char tmp[5]=".tmp";
	char *in_file_tmp=str_join(out_file,tmp);
	if ((ret = ffmpeg.avformat_open_input(&ifmt_ctx_exist, in_file_tmp, 0, 0))
			< 0) {
		ffmpeg.av_strerror(ret, error, 500);
		LOG("download_streaming-->avformat_open_input res=%s...", error);
		goto end;
	}

	if ((ret = ffmpeg.avformat_find_stream_info(ifmt_ctx_exist, 0)) < 0) {
		fprintf(stderr, "Failed to retrieve input stream information");
		ffmpeg.av_strerror(ret, error, 500);
		LOG("download_streaming-->avformat_find_stream_info res=%s...", error);
		goto end;
	}

	LOG("download_streaming-->avformat_open_input after");
	ffmpeg.av_dump_format(ifmt_ctx_exist, 0, out_file, 0);
	ffmpeg.avformat_write_header(ofmt_ctx, NULL);
	while (download_flag) {
		ret = ffmpeg.av_read_frame(ifmt_ctx_exist, &pkt);
		if (ret < 0) {
			LOG("download_streaming-->while -- ret=%d...", ret);
			break;
		}
		LOG("download_streaming-->while -- has read exist file");
		ret = ffmpeg.av_interleaved_write_frame(ofmt_ctx, &pkt);
		ffmpeg.av_free_packet(&pkt);
	}
	end: ffmpeg.avformat_close_input(&ifmt_ctx_exist);
	java_callback_onRewriteFinished();
}

int saving_network_media(const char *in_filename, const char *out_filename,
		int64_t *pts_ptr, int64_t skip_bytes) { //int64_t pts_array[]//
	int fff = 0;

	AVOutputFormat *ofmt = NULL;
	AVFormatContext *ifmt_ctx = NULL, *ofmt_ctx = NULL;
	AVPacket pkt;
	int ret, i;
	int flag = 0;
	char error[500];
	int64_t pts_array[MAX_STREAM_COUNT];
	int current;

	ffmpeg.av_register_all();
	ffmpeg.avformat_network_init();

	if ((ret = ffmpeg.avformat_open_input(&ifmt_ctx, in_filename, 0, 0)) < 0) {
		ffmpeg.av_strerror(ret, error, 500);
		LOG("download_streaming-->avformat_open_input res=%s...", error);
		goto end;
	}

	if ((ret = ffmpeg.avformat_find_stream_info(ifmt_ctx, 0)) < 0) {
		fprintf(stderr, "Failed to retrieve input stream information");
		ffmpeg.av_strerror(ret, error, 500);
		LOG("download_streaming-->avformat_find_stream_info res=%s...", error);
		goto end;
	}

	LOG("download_streaming-->avformat_open_input after.");
	ffmpeg.av_dump_format(ifmt_ctx, 0, in_filename, 0);

	streaming_duration = ifmt_ctx->duration / AV_TIME_BASE;

	ret = ffmpeg.avformat_alloc_output_context2(&ofmt_ctx, NULL, NULL,
			out_filename);

	ffmpeg.av_strerror(ret, error, 500);
//	LOG("download_streaming-->after avformat_alloc_output_context2...%s",error);
	if (!ofmt_ctx) {
		fprintf(stderr, "Could not create output context\n");
//		ret = AVERROR_UNKNOWN;
		LOG("download_streaming-->Could not create output context res=%s...", error);
		goto end;
	}

	ofmt = ofmt_ctx->oformat;

	LOG("download_streaming-->before for");

	int64_t seek_time_int64t;
	int stream_index;
	for (i = 0; i < ifmt_ctx->nb_streams; i++) {
		AVStream *in_stream = ifmt_ctx->streams[i];
		int neen_seek = 0;
		if (pts_ptr) {
			LOG("download_streaming-->seek stream+++seek_time=%lld", seek_time_int64t);
			if (seek_time_int64t < pts_ptr[i]) {
				neen_seek = 1;
				seek_time_int64t = pts_ptr[i];
				stream_index = i;
				int seek_res = -1;
				if (seek_time_int64t > 0) {
					seek_res = ffmpeg.avformat_seek_file(ifmt_ctx, i, INT64_MIN,
							seek_time_int64t, INT64_MAX, AVSEEK_FLAG_BACKWARD); //| AVSEEK_FLAG_ANY
				}
				LOG("download_streaming-->seek stream+++seek_time=%lld+++seek_res=%d", seek_time_int64t, seek_res);
			}
		}
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
	LOG("download_streaming-->after for");

	ffmpeg.av_dump_format(ofmt_ctx, 0, out_filename, 1);

	LOG("download_streaming-->before avio_open");
	if (!(ofmt->flags & AVFMT_NOFILE)) {
		ret = ffmpeg.avio_open(&ofmt_ctx->pb, out_filename, AVIO_FLAG_WRITE);
		if (ret < 0) {
			fprintf(stderr, "Could not open output file '%s'", out_filename);
			goto end;
		}
	}
	LOG("download_streaming-->after avio_open");

	if (skip_bytes <= 0) //pts_array
		ret = ffmpeg.avformat_write_header(ofmt_ctx, NULL);
	LOG("download_streaming-->ofmt_ctx->pb->pos=%lld", ofmt_ctx->pb->pos);
	if (ret < 0) {
		fprintf(stderr, "Error occurred when opening output file\n");
		goto end;
	}

//	if (skip_bytes > ofmt_ctx->pb->pos) {
//		ffmpeg.avio_skip(ofmt_ctx->pb, skip_bytes - ofmt_ctx->pb->pos);
////		ffmpeg.avio_seek_time(ofmt_ctx->pb, seek_time_int64t, stream_index,
////				AVSEEK_FLAG_BACKWARD);
//		LOG("download_streaming-->avio_skip position=%lld...ofmt_ctx->pb->pos=%lld", skip_bytes, ofmt_ctx->pb->pos);
//	}

//	if (seek_time_int64t > 0)
//		ffmpeg.avformat_seek_file(ofmt_ctx, stream_index, INT64_MIN,
//				seek_time_int64t, INT64_MAX, AVSEEK_FLAG_BACKWARD);
	if (skip_bytes > 0) {
		LOG("download_streaming-->skip_bytes=%lld,start rewrite_exist_file...", skip_bytes);
//		write_existed(out_filename, ofmt_ctx);
		rewrite_exist_file(out_filename, ofmt_ctx);
	}
	LOG("download_streaming-->before while...");
	int flag_complete_second = 0;
	while (1) {
		if (!download_flag && flag_complete_second)
			break;
		flag_complete_second = 0;
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
			current = ffmpeg.av_rescale_q(pkt.pts, out_stream->time_base,
					AV_TIME_BASE_Q) / AV_TIME_BASE;
//			current = ffmpeg.av_gettime() / 1000;
			if (pts_ptr && start_downloaded_time == -1)
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
//			pkt.pos = -1;

			if (pts_ptr && pkt.dts <= pts_ptr[pkt.stream_index])
				continue;

			log_packet(ofmt_ctx, &pkt, "out");

			pts_array[pkt.stream_index] = pkt.dts;
			LOG("download_streaming-->while -- dts=%lld...stream_index=%d...pts=%lld...current=%d", pkt.dts, pkt.stream_index, pkt.pts,current);
			ret = ffmpeg.av_interleaved_write_frame(ofmt_ctx, &pkt);
			if (current > current_time_downloaded_to) {
				if (ofmt_ctx->pb->pos < pkt.pos) {
					LOG("download_streaming-->sleep................");
					ffmpeg.av_usleep(50 * 1000);
					LOG("download_streaming-->after sleep ................");
				}
				flag_complete_second = 1;
				java_callback_onDownloadProgressChanged(ofmt_ctx->pb->pos,
						pts_array, ifmt_ctx->nb_streams, current);
				current_time_downloaded_to = current;
			}
			if (ret < 0) {
				fprintf(stderr, "Error muxing packet\n");
				break;
			}
		}
		ffmpeg.av_free_packet(&pkt);
	}

	if (!download_flag) {
		ffmpeg.avio_flush(ofmt_ctx->pb);
		java_callback_onDownloadProgressChanged(ofmt_ctx->pb->pos, pts_array,
				ifmt_ctx->nb_streams, current);
	}

	LOG("download_streaming-->end write......");
	if (ret == AVERROR_EOF) {
		LOG("download_streaming--> av_write_trailer......");
		ffmpeg.av_write_trailer(ofmt_ctx);
		java_callback_onDownloadFinished();
	}
	end: ffmpeg.av_strerror(ret, error, 500);
	LOG("download_streaming-->goto end res=%s...", error);
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
		java_callback_onDownloadFailed(error);
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

static void java_callback_onDownloadProgressChanged(int64_t pos,
		int64_t pts_array[], int stream_num, int current_time) {
//	LOG("download_streaming-->onDownloadProgressChanged -- current_time=%d...", current_time);
	JNIEnv *env;
	jclass cls;
	jmethodID mid;
	int i;

	int _needDetach = 0;
	int attachStatus = ATTACH_ERROR;

	attachStatus = JniThreadAttach(&_needDetach, &env);
	if (attachStatus == ATTACH_ERROR) {
		return ATTACH_ERROR;
	}

	cls = (*env)->GetObjectClass(env, java_object);
	if (cls == NULL)
		goto error;

	mid = (*env)->GetMethodID(env, cls, "onDownloadProgressChanged", "(JIIJ)V");

	if (mid == NULL)
		goto error;

	for (i = 0; i < stream_num; i++)
		(*env)->CallVoidMethod(env, java_object, mid, pos, current_time, i,
				pts_array[i]);

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
	mid = (*env)->GetMethodID(env, cls, "onDownloadFinished", "()V");
	if (mid == NULL)
		goto error;
	(*env)->CallVoidMethod(env, java_object, mid);

	error: JniThreadDetach(_needDetach);
}

static void java_callback_onDownloadFailed(char* msg) {
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
	mid = (*env)->GetMethodID(env, cls, "onDownloadFailed",
			"(Ljava/lang/String;)V");
	if (mid == NULL)
		goto error;
	(*env)->CallVoidMethod(env, java_object, mid,
			(*env)->NewStringUTF(env, msg));

	error: JniThreadDetach(_needDetach);
}


static void java_callback_onRewriteFinished() {
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
	mid = (*env)->GetMethodID(env, cls, "onRewriteFinished", "()V");
	if (mid == NULL)
		goto error;
	(*env)->CallVoidMethod(env, java_object, mid);

	error: JniThreadDetach(_needDetach);
}
