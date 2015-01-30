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

int streaming_duration=0;

/**-1：暂停；0：停止；1：开始**/
int download_flag = 1;

static void log_packet(const AVFormatContext *fmt_ctx, const AVPacket *pkt,
		const char *tag) {
	AVRational *time_base = &fmt_ctx->streams[pkt->stream_index]->time_base;

	printf(
			"%s: pts:%s pts_time:%s dts:%s dts_time:%s duration:%s duration_time:%s stream_index:%d\n",
			tag, av_ts2str(pkt->pts), av_ts2timestr(pkt->pts, time_base),
			av_ts2str(pkt->dts), av_ts2timestr(pkt->dts, time_base),
			av_ts2str(pkt->duration), av_ts2timestr(pkt->duration, time_base),
			pkt->stream_index);
}

int saving_network_media(const char *in_filename, const char *out_filename,int64_t pts_array[]) {
	int fff = 0;

	LOG("download_streaming-->saving_network_media start...");
	AVOutputFormat *ofmt = NULL;
	AVFormatContext *ifmt_ctx = NULL, *ofmt_ctx = NULL;
	AVPacket pkt;
	int ret, i;
	int flag=0;

	ffmpeg.av_register_all();

	if ((ret = ffmpeg.avformat_open_input(&ifmt_ctx, in_filename, 0, 0)) < 0) {
		fprintf(stderr, "Could not open input file '%s'", in_filename);
		goto end;
	}

	if ((ret = ffmpeg.avformat_find_stream_info(ifmt_ctx, 0)) < 0) {
		fprintf(stderr, "Failed to retrieve input stream information");
		goto end;
	}

	ffmpeg.av_dump_format(ifmt_ctx, 0, in_filename, 0);

	streaming_duration = ifmt_ctx->duration/AV_TIME_BASE;

	LOG("download_streaming-->before avformat_alloc_output_context2...%s",out_filename);
	ret = ffmpeg.avformat_alloc_output_context2(&ofmt_ctx, NULL, NULL, out_filename);
	LOG("download_streaming-->after avformat_alloc_output_context2...%d",ret);

	char error[500];
	ffmpeg.av_strerror(ret, error, 500);
	LOG("download_streaming-->after avformat_alloc_output_context2...%s",error);
	if (!ofmt_ctx) {
		fprintf(stderr, "Could not create output context\n");
		ret = AVERROR_UNKNOWN;
		goto end;
	}
	LOG("download_streaming-->after avformat_alloc_output_context2...");

	ofmt = ofmt_ctx->oformat;

	for (i = 0; i < ifmt_ctx->nb_streams; i++) {
		LOG("download_streaming-->for index=%d...",i);
		if (pts_array){
			int64_t last_pts = pts_array[i];
		    ffmpeg.avformat_seek_file(ifmt_ctx,i,last_pts,last_pts,last_pts,AVSEEK_FLAG_ANY);
//		    ffmpeg.avio_seek_time(ifmt_ctx->pb,i,pts_array[i],AVSEEK_FLAG_ANY);
			LOG("download_streaming-->seek out file's stream=%d&&&pts=%lld...",i,last_pts);
		}
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

	LOG("download_streaming-->before avio_open...");
	if (!(ofmt->flags & AVFMT_NOFILE)) {
		ret = ffmpeg.avio_open(&ofmt_ctx->pb, out_filename, AVIO_FLAG_WRITE);
		if (ret < 0) {
			fprintf(stderr, "Could not open output file '%s'", out_filename);
			goto end;
		}
		if (pts_array){
			for (i=0;i<ofmt_ctx->nb_streams;i++) {
				ffmpeg.avio_seek_time(ofmt_ctx->pb,i,pts_array[i],AVSEEK_FLAG_ANY);
			}
		}
	}

	LOG("download_streaming-->before avformat_write_header...");
	if(!pts_array)
	    ret = ffmpeg.avformat_write_header(ofmt_ctx, NULL);
	if (ret < 0) {
		fprintf(stderr, "Error occurred when opening output file\n");
		goto end;
	}

	LOG("download_streaming-->before write...");
	while (download_flag) {
		if(download_flag<0){
			sleep(50);
			continue;
		}
		AVStream *in_stream, *out_stream;

		ret = ffmpeg.av_read_frame(ifmt_ctx, &pkt);
		if (ret < 0)
			break;

		in_stream = ifmt_ctx->streams[pkt.stream_index];
		out_stream = ofmt_ctx->streams[pkt.stream_index];

		log_packet(ifmt_ctx, &pkt, "in");
		if (fff == 0 && pkt.stream_index == 1) {
			fff = 1;
		} else {
			/* copy packet */
			pkt.pts = ffmpeg.av_rescale_q_rnd(pkt.pts, in_stream->time_base,
					out_stream->time_base,
					AV_ROUND_NEAR_INF | AV_ROUND_PASS_MINMAX);
			pkt.dts = ffmpeg.av_rescale_q_rnd(pkt.dts, in_stream->time_base,
					out_stream->time_base,
					AV_ROUND_NEAR_INF | AV_ROUND_PASS_MINMAX);
			pkt.duration = ffmpeg.av_rescale_q(pkt.duration, in_stream->time_base,
					out_stream->time_base);
			pkt.pos = -1;
			log_packet(ofmt_ctx, &pkt, "out");

			ret = ffmpeg.av_interleaved_write_frame(ofmt_ctx, &pkt);
		    int current_time = ffmpeg.av_rescale_q(pkt.pts,
		    		out_stream->time_base, AV_TIME_BASE_Q)/ AV_TIME_BASE;
		    if (flag++%5==0)
			    java_callback_onDownloadProgressChanged(pkt, current_time);
			if (ret < 0) {
				fprintf(stderr, "Error muxing packet\n");
				break;
			}
		}
		ffmpeg.av_free_packet(&pkt);
	}

	if (ret == AVERROR_EOF)
		ffmpeg.av_write_trailer(ofmt_ctx);
	java_callback_onDownloadFinished();
	end:

	ffmpeg.avformat_close_input(&ifmt_ctx);

	/* close output */
	if (ofmt_ctx && !(ofmt->flags & AVFMT_NOFILE))
		ffmpeg.avio_close(ofmt_ctx->pb);
	ffmpeg.avformat_free_context(ofmt_ctx);

	if (ret < 0 && ret != AVERROR_EOF) {
		char error[500];
		fprintf(stderr, "Error occurred: %s\n", ffmpeg.av_strerror(ret, error, 500));
		java_callback_onDownloadFailed();
		return 1;
	}

	return 0;
}

void set_download_flag(int flag) {
	download_flag = flag;
}

int get_download_flag(){
	return download_flag;
}

int get_duration(){
	return streaming_duration;
}

static void java_callback_onDownloadProgressChanged(AVPacket pkt, int current_time){
    JNIEnv *env;
    jclass cls;
    jmethodID mid;

    if ((*jvm)->AttachCurrentThread(jvm, &env, NULL) != JNI_OK)
        return -1;

    cls = (*env)->GetObjectClass(env, java_object);
    if (cls == NULL)
        goto error;
    mid = (*env)->GetMethodID(env, cls, "onDownloadProgressChanged", "(IIJ)V");
    if (mid == NULL)
        goto error;
//    char pts[sizeof(int64_t)*8+1];
//    lltoa(pkt.pts,pts,DECIMAL);
    (*env)->CallVoidMethod(env, java_object, mid, download_id, pkt.stream_index,pkt.pts,current_time);

error:
    (*jvm)->DetachCurrentThread(jvm);
}

static void java_callback_onDownloadFinished(){
    JNIEnv *env;
    jclass cls;
    jmethodID mid;

    if ((*jvm)->AttachCurrentThread(jvm, &env, NULL) != JNI_OK)
        return -1;

    cls = (*env)->GetObjectClass(env, java_object);
    if (cls == NULL)
        goto error;
    mid = (*env)->GetMethodID(env, cls, "onDownloadFinished", "(I)V");
    if (mid == NULL)
        goto error;
    (*env)->CallVoidMethod(env, java_object, mid, download_id);

error:
    (*jvm)->DetachCurrentThread(jvm);
}

static void java_callback_onDownloadFailed(){
    JNIEnv *env;
    jclass cls;
    jmethodID mid;

    if ((*jvm)->AttachCurrentThread(jvm, &env, NULL) != JNI_OK)
        return -1;

    cls = (*env)->GetObjectClass(env, java_object);
    if (cls == NULL)
        goto error;
    mid = (*env)->GetMethodID(env, cls, "onDownloadFailed", "(I)V");
    if (mid == NULL)
        goto error;
    (*env)->CallVoidMethod(env, java_object, mid, download_id);

error:
    (*jvm)->DetachCurrentThread(jvm);
}
