/*
 * =====================================================================================
 *
 *       Filename:  MoboOpenSubtitle.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  12/01/2014 22:09:40
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Sun Jun (), sunjun@clov4r.com
 *   Organization:  
 *
 * =====================================================================================
 */
#include "mobo_open_subtitle.h"

#define DEFAULT_SUBTITLE_SIZE   2000


ffmpeg_func_t ffmpeg;


sub_data_p g_sub_p;
char *sj_get_raw_text_from_ssa(const char *ssa);


void init_ffmpeg(const char *lib_path, const char * ffmpeg_filename)
{
	init_ffmpeg_func(lib_path, ffmpeg_filename, &ffmpeg);
}

static int open_codec_context(sub_data_p sub_p, int index, enum AVMediaType type)
{
    int ret, stream_index = -1;
    AVStream *st;
    AVCodecContext *dec_ctx = NULL;
    AVCodec *dec = NULL;

    AVFormatContext *fmt_ctx = sub_p->fmt_ctx;

    int i,j;
    for (i = 0, j = 0; i < fmt_ctx->nb_streams; i++) {
        st = fmt_ctx->streams[i];
        st->discard = AVDISCARD_ALL;
        if (st->codec->codec_type == type) {
            if (j == index)
                stream_index = i;
            j++;
        }
    }

    if (stream_index == -1)
        return -1;

    st = fmt_ctx->streams[stream_index];
    st->discard = AVDISCARD_DEFAULT;
    dec_ctx = st->codec;

    sub_p->stream = st;
    sub_p->dec_ctx = dec_ctx;

    /* find decoder for the stream */
    dec = ffmpeg.avcodec_find_decoder(dec_ctx->codec_id);
    if (!dec) {
        return -1;
    }

    /* Init the decoders, with or without reference counting */
    if ((ret =ffmpeg.avcodec_open2(dec_ctx, dec, NULL)) < 0) {
        return ret;
    }

    return 0;
}

int time_in_subtitle(AVSubtitle *subtitle, int time)
{
    long long start_time = subtitle->pts/(AV_TIME_BASE/1000) + subtitle->start_display_time;
    long long end_time = subtitle->pts/(AV_TIME_BASE/1000) + subtitle->end_display_time;
    
    if (time >= start_time && time <= end_time) {
        return 1;
    }
    return 0;
}

int time_lower_subtitle(AVSubtitle *subtitle, int time)
{
    long long start_time = subtitle->pts/(AV_TIME_BASE/1000) + subtitle->start_display_time;
    
    if (time < start_time) {
        return 1;
    }
    return 0;
}

int time_greater_subtitle(AVSubtitle *subtitle, int time)
{
    long long end_time = subtitle->pts/(AV_TIME_BASE/1000) + subtitle->end_display_time;
    
    if (time > end_time) {
        return 1;
    }
    return 0;
}


int binary_search(AVSubtitle *a, int key, int n)
{
    int left = 0, right = n - 1, mid = 0;
    
    while (left < right) {
        mid = (left + right) / 2;
        
        if (time_in_subtitle(&a[mid], key))
            return mid;
        
        if (time_lower_subtitle(&a[mid], key)) {
            right = mid - 1;
        } else if (time_greater_subtitle(&a[mid], key)) {
            left = mid + 1;
        }
    }
    return -1;
}

void close_subtitle()
{
    if (g_sub_p) {
    	int i;// error: 'for' loop initial declarations are only allowed in C99 or C11 mode
        for (i = 0; i < g_sub_p->subtitle_size; i++) {
            AVSubtitle *sp = &(g_sub_p->subtitles_array[i]);
           ffmpeg.avsubtitle_free(sp);
        }
        ffmpeg.av_freep(g_sub_p);
    }
}

char *get_subtitle_ontime(int cur_time)
{
    if (!g_sub_p) {
        return NULL;
    }
    int array_index = -1;
    char *subtitle_text = NULL;
    sub_data_p sub_p = g_sub_p;
    array_index = binary_search(sub_p->subtitles_array, cur_time, sub_p->subtitle_index);
    AVSubtitle *subtitle = NULL;
    if (array_index >= 0) {
        subtitle = &sub_p->subtitles_array[array_index];
        AVSubtitleRect *ffregion = subtitle->rects[0];
        if (ffregion->type == SUBTITLE_TEXT) {
            subtitle_text = ffregion->text;
        } else if (ffregion->type == SUBTITLE_ASS) {
            subtitle_text = sj_get_raw_text_from_ssa(ffregion->ass);
        }
    }
    return subtitle_text;
}

int subtitle_read_decode(sub_data_p sub_p)
{
    int ret = 0;
    int got_frame = 0;
    
    AVPacket pkt;
    ffmpeg.av_init_packet(&pkt);
    
    sub_p->subtitle_size = DEFAULT_SUBTITLE_SIZE;
    sub_p->subtitle_index = 0;
    sub_p->subtitles_array = ffmpeg.av_mallocz(sub_p->subtitle_size * sizeof(AVSubtitle));
    
    AVSubtitle *subtitle = NULL;
    
    if (!sub_p->subtitles_array) {
        return -1;
    }

    while (ffmpeg.av_read_frame(sub_p->fmt_ctx, &pkt) >= 0) {
        if (pkt.stream_index == sub_p->stream->index) {
            if (sub_p->subtitle_index >= sub_p->subtitle_size) {
                sub_p->subtitle_size *= 2;
                AVSubtitle *temp_p = (AVSubtitle *)realloc((void *)sub_p->subtitles_array, sub_p->subtitle_size * sizeof(AVSubtitle));
                if (!temp_p) {
                    ffmpeg.av_freep(sub_p->subtitles_array);
                    return -2;
                } else {
                    sub_p->subtitles_array = temp_p;
                }
            }
            
            subtitle = &sub_p->subtitles_array[sub_p->subtitle_index];
            
            ret = ffmpeg.avcodec_decode_subtitle2(sub_p->dec_ctx, subtitle, &got_frame, &pkt);
            if (ret < 0) {
                ffmpeg.av_freep(sub_p->subtitles_array);
                return ret;
            }
            
            if (got_frame) {
                sub_p->subtitle_index++;
                fprintf(stderr, "got subtitle\n");
            }
            
            ffmpeg.av_free_packet(&pkt);
        }
    }
    return ret;
}

int open_subtitle(const char *file, int stream_index)
{
    AVFormatContext *fmt_ctx = NULL;
    int ret = 0;
    g_sub_p = ffmpeg.av_mallocz(sizeof(SubtitleData));
    sub_data_p sub_p = g_sub_p;
    
    ffmpeg.av_register_all();

    /* open input file, and allocate format context */
    if (ffmpeg.avformat_open_input(&fmt_ctx, file, NULL, NULL) < 0) {
        fprintf(stderr, "Could not open source file %s\n", file);
        return -1;
    }

    /* retrieve stream information */
    if (ffmpeg.avformat_find_stream_info(fmt_ctx, NULL) < 0) {
        fprintf(stderr, "Could not find stream information\n");
        return -1;
    }

    sub_p->fmt_ctx = fmt_ctx;
    ret = open_codec_context(sub_p, stream_index, AVMEDIA_TYPE_SUBTITLE);
    if (ret < 0) {
        ffmpeg.avformat_close_input(&sub_p->fmt_ctx);
        return ret;
    }
    
    ret = subtitle_read_decode(sub_p);
    
    ffmpeg.avcodec_close(sub_p->dec_ctx);
    ffmpeg.avformat_close_input(&sub_p->fmt_ctx);
    if (ret < 0) {
        ffmpeg.av_freep(g_sub_p);
    }
    return ret;
}

char *sj_get_raw_text_from_ssa(const char *ssa)
{
    int n,intag,inescape;
    char *multiblock = NULL, *realloced_mb;
    char *allocated;
    const char *dialogue, *ptr, *tag_start;

    if (!ssa) return NULL;

    /* turns out some SSA packets have several blocks, each on a single line, so loop */
    while (ssa) {
        dialogue=strstr(ssa, "Dialogue:");
        if (!dialogue) break;

        ptr = dialogue;

        char *temp = ptr;
        int nn = 9;
        for (n=0;n<9;++n) {
            temp=strchr(temp,',');
            if (!temp) break;
            ++temp;
        }

        if (n == 9) {
            nn = 9;
        } else {
            nn = 4;
        }
        for (n=0;n<nn;++n) {
            ptr=strchr(ptr,',');
            if (!ptr) return NULL;
            ++ptr;
        }
        dialogue = ptr;
        allocated = strdup(dialogue);

        /* find all "{...}" tags - the following must work for UTF-8 */
        intag=inescape=0;
        n=0;
        for (ptr=dialogue; *ptr && *ptr!='\n'; ++ptr) {
            if (*ptr=='{') {
                if (intag==0) tag_start = ptr;
                ++intag;
            }
            else if (*ptr=='}') {
                --intag;
                if (intag == 0) {
                    /* tag parsing - none for now */
                }
            }
            else if (!intag) {
                if (inescape) {
                    if (*ptr == 'N' || *ptr == 'n')
                        allocated[n++] = '\n';
                    else if (*ptr == 'h')
                        allocated[n++] = ' ';
                    inescape=0;
                }
                else {
                    if (*ptr=='\\') {
                        inescape=1;
                    }
                    else {
                        allocated[n++]=*ptr;
                    }
                }
            }
        }
        allocated[n]=0;

        /* skip over what we read */
        ssa = ptr;

        /* remove any trailing newlines (also \r characters) */
        n = strlen(allocated);
        while (n>0 && (allocated[n-1]=='\n' || allocated[n-1]=='\r'))
            allocated[--n]=0;

        /* add this new block */
        realloced_mb = (char*)realloc(multiblock, (multiblock?strlen(multiblock):0) + strlen(allocated) + 2); /* \n + 0 */
        if (realloced_mb) {
            if (multiblock) strcat(realloced_mb, "\n"); else strcpy(realloced_mb, "");
            strcat(realloced_mb, allocated);
            multiblock = realloced_mb;
            free(allocated);
        }
    }

    return multiblock;
}
