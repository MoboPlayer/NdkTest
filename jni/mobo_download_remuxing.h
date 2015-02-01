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

#ifndef MOBO_DOWNLOD_REMUXING_FLAG
#define MOBO_DOWNLOD_REMUXING_FLAG

#include <libavutil/timestamp.h>
#include <libavformat/avformat.h>
#include <jni.h>
#include <pthread.h>
#include "cmp_ffmpeg.h"

#define FLAG_DOWNLOAD_START 1
#define FLAG_DOWNLOAD_PAUSE -1
#define FLAG_DOWNLOAD_STOP 0

extern JavaVM *jvm;
extern jobject java_object;
extern int download_id;
extern ffmpeg_func_t ffmpeg;

int init_mutex_cond();

void des_mutex_cond();

int saving_network_media(const char *in_filename, const char *out_filename,int64_t skip_bytes);//pts_array[]

void set_download_flag(int flag);

static void java_callback_onDownloadProgressChanged(int64_t pos,int current_time);//AVPacket pkt

static void java_callback_onDownloadFinished();

static void java_callback_onDownloadFailed();

#endif
