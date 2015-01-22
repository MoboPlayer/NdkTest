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
#include "mobo_open_subtitle.h"

#define DEFAULT_SUBTITLE_SIZE   2000

#define MAX_TEXT_CHAR_LENGTH  1024
#define MAX_SUBTITLES 10

extern ffmpeg_func_t ffmpeg;

sub_data_p g_sub_p[MAX_SUBTITLES];
char *sj_get_raw_text_from_ssa(const char *ssa);

static int open_codec_context(sub_data_p sub_p, int index,
		enum AVMediaType type) {
	int ret, stream_index = -1;
	AVStream *st;
	AVCodecContext *dec_ctx = NULL;
	AVCodec *dec = NULL;

	AVFormatContext *fmt_ctx = sub_p->fmt_ctx;

	int i, j;
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
	if ((ret = ffmpeg.avcodec_open2(dec_ctx, dec, NULL)) < 0) {
		return ret;
	}

	return 0;
}

int time_in_subtitle(AVSubtitle *subtitle, int time) {
	long long start_time = subtitle->pts / (AV_TIME_BASE / 1000)
			+ subtitle->start_display_time;
	long long end_time = subtitle->pts / (AV_TIME_BASE / 1000)
			+ subtitle->end_display_time;
//    LOGI("start time:%lld end_time %lld",start_time,end_time);
	if (time >= start_time && time <= end_time) {
		return 1;
	}
	return 0;
}

int time_lower_subtitle(AVSubtitle *subtitle, int time) {
	long long start_time = subtitle->pts / (AV_TIME_BASE / 1000)
			+ subtitle->start_display_time;

	if (time < start_time) {
		return 1;
	}
	return 0;
}

int time_greater_subtitle(AVSubtitle *subtitle, int time) {
	long long end_time = subtitle->pts / (AV_TIME_BASE / 1000)
			+ subtitle->end_display_time;

	if (time > end_time) {
		return 1;
	}
	return 0;
}

int binary_search(AVSubtitle *a, int key, int n) {
	int left = 0, right = n - 1, mid = 0;

//    LOGE("time:%d ",key);//
	while (left <= right) {
		mid = (left + right) / 2;
//        LOGI("mid:%d left:%d right:%d",mid,left,right);//

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

void close_subtitle(int subtiltle_index) {
	if (g_sub_p[subtiltle_index]) {
		int i; // error: 'for' loop initial declarations are only allowed in C99 or C11 mode
		for (i = 0; i < g_sub_p[subtiltle_index]->subtitle_size; i++) {
			AVSubtitle *sp = &(g_sub_p[subtiltle_index]->subtitles_array[i]);
			ffmpeg.avsubtitle_free(sp);
		}
		ffmpeg.av_freep(g_sub_p[subtiltle_index]);
	}
}

char *get_subtitle_ontime(int cur_time, int subtiltle_index) {
	if (!g_sub_p[subtiltle_index]) {
		return NULL;
	}
	int array_index = -1;
//    array_index = cur_time;
	char *subtitle_text = NULL;
	sub_data_p sub_p = g_sub_p[subtiltle_index];
	array_index = binary_search(sub_p->subtitles_array, cur_time,
			sub_p->subtitle_index);

	if (array_index == -1)
		array_index = binary_search(sub_p->subtitles_array, cur_time + 500,
				sub_p->subtitle_index);

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

char *get_subtitle_ontime_2(int cur_time, int subtiltle_index, int time_diff) {
	if (!g_sub_p[subtiltle_index]) {
		return NULL;
	}
	int ret = 0;
	char *subtitle_text = NULL;
	int64_t seek_target = 0, seek_target_prev = 0, seek_target_after = 0;
	AVPacket packet;
//	AVSubtitle sub = NULL;
//	ffmpeg.av_malloc(sizeof(AVSubtitle));

	AVSubtitle *subtitle = ffmpeg.av_malloc(sizeof(*subtitle));
	sub_data_p sub_data = g_sub_p[subtiltle_index];
	ffmpeg.av_init_packet(&packet);

	int prev_time = cur_time - time_diff;
	get_rescale_time(cur_time, &seek_target, sub_data);
	if (cur_time > time_diff)
		get_rescale_time(prev_time, &seek_target_prev, sub_data);
	get_rescale_time(cur_time + time_diff, &seek_target_after, sub_data);

	seek_target_prev/=1000;
	seek_target/=1000;
	seek_target_after/=1000;

	ffmpeg.avformat_seek_file(sub_data->fmt_ctx, sub_data->stream->index,
			seek_target_prev, seek_target, seek_target_after,
			AVSEEK_FLAG_BACKWARD | AVSEEK_FLAG_ANY);
//	LOG("open_subtitle-->cur_time=%d+++time_diff=%d", cur_time, time_diff);

	int got_frame = 0;
	while (!got_frame && ffmpeg.av_read_frame(sub_data->fmt_ctx, &packet) >= 0) {
		if (packet.stream_index == sub_data->stream->index) {
			ret = ffmpeg.avcodec_decode_subtitle2(sub_data->dec_ctx, subtitle,
					&got_frame, &packet);
			if (ret < 0) {
				ffmpeg.avsubtitle_free(subtitle);
				break;
			}

			if (got_frame && subtitle->num_rects) {
				AVSubtitleRect *ffregion = subtitle->rects[0];
				if (ffregion) {
					if (ffregion->type == SUBTITLE_TEXT) {
						subtitle_text = ffregion->text;
					} else if (ffregion->type == SUBTITLE_ASS) {
						subtitle_text = sj_get_raw_text_from_ssa(ffregion->ass);
					}
					break;
				}
			}
		}
		ffmpeg.av_free_packet(&packet);
	}

	return subtitle_text;
}

void get_rescale_time(int cur_time, int64_t *res_time, sub_data_p sub_data) {
	*res_time = cur_time * AV_TIME_BASE;
	*res_time = ffmpeg.av_rescale_q(*res_time, AV_TIME_BASE_Q,
			sub_data->stream->time_base);
}

int subtitle_read_decode(sub_data_p sub_p) {
	int ret = 0;
	int got_frame = 0;

	AVPacket pkt;
	ffmpeg.av_init_packet(&pkt);

	sub_p->subtitle_size = DEFAULT_SUBTITLE_SIZE;
	sub_p->subtitle_index = 0;
	sub_p->subtitles_array = ffmpeg.av_mallocz(
			sub_p->subtitle_size * sizeof(AVSubtitle));

	AVSubtitle *subtitle = NULL;

	if (!sub_p->subtitles_array) {
		return -1;
	}

	while (ffmpeg.av_read_frame(sub_p->fmt_ctx, &pkt) >= 0) {
		if (pkt.stream_index == sub_p->stream->index) {
			if (sub_p->subtitle_index >= sub_p->subtitle_size) {
				sub_p->subtitle_size *= 2;
				AVSubtitle *temp_p = (AVSubtitle *) realloc(
						(void *) sub_p->subtitles_array,
						sub_p->subtitle_size * sizeof(AVSubtitle));
				if (!temp_p) {
					ffmpeg.av_freep(sub_p->subtitles_array);
					return -2;
				} else {
					sub_p->subtitles_array = temp_p;
				}
			}

			subtitle = &sub_p->subtitles_array[sub_p->subtitle_index];

			ret = ffmpeg.avcodec_decode_subtitle2(sub_p->dec_ctx, subtitle,
					&got_frame, &pkt);
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

int open_subtitle(const char *file, int stream_index, int subtiltle_index) {
	AVFormatContext *fmt_ctx = NULL;
	int ret = 0;
	g_sub_p[subtiltle_index] = ffmpeg.av_mallocz(sizeof(SubtitleData));
	sub_data_p sub_p = g_sub_p[subtiltle_index];

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
		ffmpeg.av_freep(g_sub_p[subtiltle_index]);
	}
	return ret;
}

int open_subtitle_2(const char *file, int stream_index, int subtiltle_index) {
	AVFormatContext *fmt_ctx = NULL;
	int ret = 0;
	g_sub_p[subtiltle_index] = ffmpeg.av_mallocz(sizeof(SubtitleData));
	sub_data_p sub_p = g_sub_p[subtiltle_index];

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

	return ret;
}

void close_subtitle_2(int subtiltle_index) {
	sub_data_p sub_p = g_sub_p[subtiltle_index];
	ffmpeg.avcodec_close(sub_p->dec_ctx);
	ffmpeg.avformat_close_input(&sub_p->fmt_ctx);
	ffmpeg.av_freep(sub_p);
}

int is_subtitle_exits(const char *file) {
	AVFormatContext *fmt_ctx = NULL;

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

	int i, num = 0;
	for (i = 0; i < fmt_ctx->nb_streams; i++) {
		if (fmt_ctx->streams[i]->codec->codec_type == AVMEDIA_TYPE_SUBTITLE) {
			num++;
		}
	}
	ffmpeg.avformat_close_input(&fmt_ctx);
	return num;
}

int get_subtitle_type(int subtiltle_index) {
	if (!g_sub_p[subtiltle_index]) {
		return -1;
	}
	sub_data_p sub_p = g_sub_p[subtiltle_index];
	return sub_p->subtitles_array->rects[0]->type;

}
char *sj_get_sa_info(const char *file, char *text) {
	AVFormatContext *fmt_ctx = NULL;

	ffmpeg.av_register_all();

	/* open input file, and allocate format context */
	if (ffmpeg.avformat_open_input(&fmt_ctx, file, NULL, NULL) < 0) {
		fprintf(stderr, "Could not open source file %s\n", file);
		return NULL;
	}

	/* retrieve stream information */
	if (ffmpeg.avformat_find_stream_info(fmt_ctx, NULL) < 0) {
		fprintf(stderr, "Could not find stream information\n");
		return NULL;
	}
	int size = MAX_TEXT_CHAR_LENGTH;

	int subtitle_i = 0;
	int audio_i = 0, i;
	for (i = 0; i < fmt_ctx->nb_streams; i++) {

		if (fmt_ctx->streams[i]->codec->codec_type == AVMEDIA_TYPE_SUBTITLE) {
			subtitle_i++;
			AVCodecContext *sCodec = fmt_ctx->streams[i]->codec;
			AVDictionaryEntry *lang = NULL;
			char subtitleTitle[100];
			char *subtitleLanguage = NULL;
			char *subtitleCodecName = NULL;

			lang = ffmpeg.av_dict_get(fmt_ctx->streams[i]->metadata, "title",
					NULL, 0);
			if (lang != NULL) {
				strcpy(subtitleTitle, lang->value);
			} else {
				sprintf(subtitleTitle, "Unknown %d", subtitle_i);
				LOG("sj_get_sa_info subtitleTitle = %s", subtitleTitle);
			}

			lang = ffmpeg.av_dict_get(fmt_ctx->streams[i]->metadata, "language",
					NULL, 0);
			if (lang != NULL) {
				subtitleLanguage = lang->value;
			} else {
				subtitleLanguage = "Unknown";
			}
			subtitleCodecName = ffmpeg.avcodec_get_name(sCodec->codec_id);
			if (subtitleCodecName == NULL) {
				subtitleCodecName = "Unknown";
			}

			if (text && size)
				snprintf(text, size, "%ss;%s;%s;%s\n", text, subtitleTitle,
						subtitleLanguage, subtitleCodecName);
		}
	}
	ffmpeg.avformat_close_input(&fmt_ctx);
	return text;
}
//char *get_subtitle_language(int subtiltle_index){
////
////	char result[100] = "hellooooo ";
//////	char *temp = "hello";
//////	strcpy(result,temp);
////	return result;
//
//	if (!g_sub_p[subtiltle_index]) {
//			return NULL;
//	}
//	sub_data_p sub_p = g_sub_p[subtiltle_index];
//	AVFormatContext *fmt_ctx = sub_p->fmt_ctx;
//
//    int size = MAX_TEXT_CHAR_LENGTH;
//
//	char result[100];
//	int i;
//	for(i = 0; i < fmt_ctx->nb_streams; i++) {
//
//		if (fmt_ctx->streams[i]->codec->codec_type == AVMEDIA_TYPE_SUBTITLE) {
//			AVCodecContext *sCodec = fmt_ctx->streams[i]->codec;
//			AVDictionaryEntry *lang = NULL;
////			char subtitleTitle[100];
//
//			lang = ffmpeg.av_dict_get(fmt_ctx->streams[i]->metadata, "title", NULL, 0);
//			if (lang != NULL) {
////				strcat(text,lang->value);
////				strcat(text,"\n");
//				strcpy(result, lang->value);
//				break;
//			} else {
//
////				strcat(text,"\n");
////				LOG("sj_get_sa_info subtitleTitle = %s", subtitleTitle);
//			}
//
//
//
//
////			if (text && size)
////				snprintf(text,size,"%ss%s\n",
////						text,
////						subtitleTitle
////						);
//		}
//	}
//
//	return NULL;
//}

char *sj_get_raw_text_from_ssa(const char *ssa) {
	int n, intag, inescape;
	char *multiblock = NULL, *realloced_mb;
	char *allocated;
	const char *dialogue, *ptr, *tag_start;

	if (!ssa)
		return NULL;

	/* turns out some SSA packets have several blocks, each on a single line, so loop */
	while (ssa) {
		dialogue = strstr(ssa, "Dialogue:");
		if (!dialogue)
			break;

		ptr = dialogue;

		char *temp = ptr;
		int nn = 9;
		for (n = 0; n < 9; ++n) {
			temp = strchr(temp, ',');
			if (!temp)
				break;
			++temp;
		}

		if (n == 9) {
			nn = 9;
		} else {
			nn = 4;
		}
		for (n = 0; n < nn; ++n) {
			ptr = strchr(ptr, ',');
			if (!ptr)
				return NULL;
			++ptr;
		}
		dialogue = ptr;
		allocated = strdup(dialogue);

		/* find all "{...}" tags - the following must work for UTF-8 */
		intag = inescape = 0;
		n = 0;
		for (ptr = dialogue; *ptr && *ptr != '\n'; ++ptr) {
			if (*ptr == '{') {
				if (intag == 0)
					tag_start = ptr;
				++intag;
			} else if (*ptr == '}') {
				--intag;
				if (intag == 0) {
					/* tag parsing - none for now */
				}
			} else if (!intag) {
				if (inescape) {
					if (*ptr == 'N' || *ptr == 'n')
						allocated[n++] = '\n';
					else if (*ptr == 'h')
						allocated[n++] = ' ';
					inescape = 0;
				} else {
					if (*ptr == '\\') {
						inescape = 1;
					} else {
						allocated[n++] = *ptr;
					}
				}
			}
		}
		allocated[n] = 0;

		/* skip over what we read */
		ssa = ptr;

		/* remove any trailing newlines (also \r characters) */
		n = strlen(allocated);
		while (n > 0 && (allocated[n - 1] == '\n' || allocated[n - 1] == '\r'))
			allocated[--n] = 0;

		/* add this new block */
		realloced_mb = (char*) realloc(multiblock,
				(multiblock ? strlen(multiblock) : 0) + strlen(allocated) + 2); /* \n + 0 */
		if (realloced_mb) {
			if (multiblock)
				strcat(realloced_mb, "\n");
			else
				strcpy(realloced_mb, "");
			strcat(realloced_mb, allocated);
			multiblock = realloced_mb;
			free(allocated);
		}
	}

	return multiblock;
}
