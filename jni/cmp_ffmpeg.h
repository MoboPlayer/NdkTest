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

#ifndef CMP_FFMPEG_H_
#define CMP_FFMPEG_H_

#include <dlfcn.h>
#include "config.h"
#include <inttypes.h>
#include <math.h>
#include <limits.h>
#include "libavformat/avformat.h"
#include "libavutil/audioconvert.h"
#include "libavcodec/avfft.h"
#include "libavcodec/version.h"

#include "libavutil/opt.h"
#include "libavutil/time.h"
#include "libavutil/dict.h"
#include "libavutil/samplefmt.h"
#include "libavutil/avstring.h"
#include "libavutil/frame.h"
#include "libavutil/channel_layout.h"
#include "libswscale/swscale.h"

#include "libswresample/swresample.h"
//#ifdef __CMPLAYER__CORE__

#include "cmp_log.h"


typedef struct ffmpeg_func_t
{
	void (*av_init_packet)(AVPacket *pkt);
	void (*av_free_packet)(AVPacket *pkt);
	void (*av_freep)(void *ptr);
	int (*av_dup_packet)(AVPacket *pkt);
	void *(*av_malloc)(unsigned int size);
	void *(*av_mallocz)(unsigned int size);
	void (*av_free)(void *ptr);
	int64_t (*av_gettime)(void);
	int64_t (*av_rescale_q)(int64_t a, AVRational bq, AVRational cq);
	int64_t (*av_rescale)(int64_t a, int64_t b, int64_t c);
	int (*av_index_search_timestamp)(AVStream *st, int64_t wanted_timestamp,
	                              int flags);
	int (*av_seek_frame)(AVFormatContext *s, int stream_index, int64_t timestamp, int flags);
	int (*avformat_seek_file)(AVFormatContext *s, int stream_index, int64_t min_ts, int64_t ts, int64_t max_ts, int flags);


	int (*av_read_frame)(AVFormatContext *s, AVPacket *pkt);
	void (*av_register_all)(void);

    int (*avformat_open_input)(AVFormatContext **ps, const char *filename, 
                            AVInputFormat *fmt, AVDictionary **options);

    int (*avformat_find_stream_info)(AVFormatContext *ic, AVDictionary **options);
    void (*avformat_close_input)(AVFormatContext **ps);

	void (*avpicture_free)(AVPicture *picture);

	AVCodec *(*avcodec_find_decoder)(enum AVCodecID id);

    int (*avcodec_open2)(AVCodecContext *avctx, const AVCodec *codec, AVDictionary **options);

	void (*avcodec_flush_buffers)(AVCodecContext *avctx);

	AVFrame *(*avcodec_alloc_frame)(void);

	int (*avcodec_decode_video2)(AVCodecContext *avctx, AVFrame *picture,
	                         int *got_picture_ptr,
	                         AVPacket *avpkt);

	int (*avpicture_alloc)(AVPicture *picture,
	                    enum PixelFormat pix_fmt, int width, int height);

	int (*avcodec_close)(AVCodecContext *avctx);

	unsigned (*avcodec_version)(void);
	unsigned (*avformat_version)(void);
	unsigned (*avutil_version)(void);

    int64_t (*avio_size)(AVIOContext *s);

	int (*avcodec_decode_audio3)(AVCodecContext *avctx, int16_t *samples,
	                         int *frame_size_ptr,
	                         AVPacket *avpkt);

    int (*avcodec_decode_audio4)(AVCodecContext *avctx, AVFrame *frame,
            int *got_frame_ptr, const AVPacket *avpkt);

	AVDictionaryEntry *(*av_dict_get)(AVDictionary *m, const char *key, const AVDictionaryEntry *prev, int flags);
	const char *(*avcodec_get_name)(enum AVCodecID id);
    
    struct SwrContext *(*swr_alloc_set_opts)(struct SwrContext *s,
            int64_t out_ch_layout, enum AVSampleFormat out_sample_fmt, int out_sample_rate,
            int64_t  in_ch_layout, enum AVSampleFormat  in_sample_fmt, int  in_sample_rate,
            int log_offset, void *log_ctx);
    int (*swr_init)(struct SwrContext *s);
    void (*swr_free)(struct SwrContext **ss);
    int (*av_get_bytes_per_sample)(enum AVSampleFormat sample_fmt);

    void (*avcodec_get_frame_defaults)(AVFrame *frame);

    void (*av_frame_unref)(AVFrame *frame);
    int (*av_get_channel_layout_nb_channels)(uint64_t channel_layout);
    int64_t (*av_get_default_channel_layout)(int nb_channels);
    int64_t (*av_frame_get_channels)(const AVFrame *frame);

    int (*av_samples_get_buffer_size)(int *linesize, int nb_channels, int nb_samples,
                               enum AVSampleFormat sample_fmt, int align);
    int (*avcodec_decode_subtitle2)(AVCodecContext *avctx, AVSubtitle *sub,
            int *got_sub_ptr,
            AVPacket *avpkt);
    void (*avsubtitle_free)(AVSubtitle *sub);
    AVFrame* (*av_frame_alloc)(void);
    void (*av_frame_free)(AVFrame **frame);
    int (*av_strerror)(int errnum, char *errbuf, size_t errbuf_size);

    int(*av_find_best_stream)(AVFormatContext *ic, enum AVMediaType type,
            int wanted_stream_nb, int related_stream,
            AVCodec **decoder_ret, int flags);
    const char *(*av_get_media_type_string)(enum AVMediaType media_type);
    void (*av_dump_format)(AVFormatContext *ic, int index,
                        const char *url, int is_output);
    struct SwsContext *(*sws_getCachedContext)(struct SwsContext *context, int srcW,
                                            int srcH, enum AVPixelFormat srcFormat,
                                            int dstW, int dstH,
                                            enum AVPixelFormat dstFormat, int flags,
                                            SwsFilter *srcFilter,
                                            SwsFilter *dstFilter,
                                            const double *param);
    int (*sws_scale)(struct SwsContext *c, const uint8_t *const srcSlice[],
                  const int srcStride[], int srcSliceY, int srcSliceH,
                  uint8_t *const dst[], const int dstStride[]);

}ffmpeg_func_t;


#define FF_FUNC_CHECK(X) \
	if (!X){ \
		LOGD("ffmpeg func not fund : %s ", #X ); \
	}; \

#define TO_STRING(X) #X


int init_ffmpeg_func(const char *lib_path, const char * ffmpeg_filename, ffmpeg_func_t *ffmpeg_func_p);
//#endif /* __CMPLAYER__CORE__ */

#endif /* CMP_FFMPEG_H_ */
