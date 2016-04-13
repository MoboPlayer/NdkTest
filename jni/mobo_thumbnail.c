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
//		fprintf(stderr, "Could not find %s stream in input file '%s'\n",
//				ffmpeg.av_get_media_type_string(type), thumbnail_data->src_filename);
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

void seek_stream(int64_t seek_target, int seek_flags, mobo_thumbnail_data *thumbnail_data){
	seek_target = ffmpeg.  av_rescale_q(seek_target, AV_TIME_BASE_Q,
			thumbnail_data->video_stream->time_base);
	LOG("check_seek seek_target=%lld", seek_target);

	// 根据key frame进行seek
    int seek_any = 1;

	int index = ffmpeg.  av_index_search_timestamp(thumbnail_data->video_stream,seek_target,seek_flags);

	if (index>0) {
		AVIndexEntry *entries= thumbnail_data->video_stream->index_entries;
		int nb_entries= thumbnail_data->video_stream->nb_index_entries;

		//查找key frame index
		do {
			if (index > nb_entries || index <0 ) break;

			if (entries[index].flags & AVINDEX_KEYFRAME) {

				seek_target = entries[index].timestamp;

				if (seek_target < 0)  seek_target = 0;

				seek_any = 0;
				break;
			}

			if (seek_flags & AVSEEK_FLAG_BACKWARD) {//向后seek
				index --;
			} else {
				index ++;
			}

		}while(1);

	}


	if (seek_any)
		seek_flags = seek_flags | AVSEEK_FLAG_ANY;
	else
		seek_flags = seek_flags & ~AVSEEK_FLAG_ANY;

	ffmpeg.avformat_seek_file(thumbnail_data->fmt_ctx, thumbnail_data->video_stream_idx, 0, seek_target,
				seek_target,
				seek_flags);
}

static void ff_log_callback(void*avcl, int level, const char*fmt, va_list vl)
{
    LOG("error is %s",fmt);
}

//use ffmpeg generate file's thumbnail at gen_second(second)
int gen_thumbnail(const char *file, int gen_second, int gen_IDR_frame, mobo_thumbnail_data *thumbnail_data) {
	int ret = 0;
	AVPacket packet;
	int got_frame = 0;
	int64_t seek_target = 0;
	int seek_times = 0;
	thumbnail_data->fmt_ctx = NULL;
//	ffmpeg.av_log_set_level(48);
//	ffmpeg.av_log_set_callback(ff_log_callback);

	thumbnail_data->src_filename = file;
	/* register all formats and codecs */
	ffmpeg.av_register_all();
	ffmpeg.avformat_network_init();
	LOG("gen_thumbnail---file-path=%s", thumbnail_data->src_filename);

	thumbnail_data->fmt_ctx = NULL;
	thumbnail_data->fmt_ctx = ffmpeg.avformat_alloc_context();
//	thumbnail_data->fmt_ctx->max_analyze_duration2 = 1000000;
//	thumbnail_data->fmt_ctx->probesize2 = 1000000;

	/* open input file, and allocate format context */
	if (!thumbnail_data->fmt_ctx || (ret = ffmpeg.avformat_open_input(&(thumbnail_data->fmt_ctx), thumbnail_data->src_filename, NULL, NULL))
			< 0) {
		fprintf(stderr, "Could not open source file %s\n", thumbnail_data->src_filename);
		char error[500];
		ffmpeg.av_strerror(ret, error, 500);
		LOG("gen_thumbnail---error=%s", error);
		return -1;
	}

	/* retrieve stream information */
	if (ffmpeg.avformat_find_stream_info(thumbnail_data->fmt_ctx, NULL) < 0) {
		fprintf(stderr, "Could not find stream information\n");
		return -2;
	}

	if (open_codec_context(&(thumbnail_data->video_stream_idx), thumbnail_data->fmt_ctx, AVMEDIA_TYPE_VIDEO)
			>= 0) {
		thumbnail_data->video_stream = thumbnail_data->fmt_ctx->streams[thumbnail_data->video_stream_idx];
		thumbnail_data->video_dec_ctx = thumbnail_data->video_stream->codec;
	} else {
		ret = -5;
		goto end;
	}

	/* dump input information to stderr */
	ffmpeg.av_dump_format(thumbnail_data->fmt_ctx, 0, thumbnail_data->src_filename, 0);

	if(!thumbnail_data->video_dec_ctx || thumbnail_data->video_dec_ctx->pix_fmt < 0){
		ret = -1;
		goto end;
	}

	if(thumbnail_data->video_dec_ctx->pix_fmt < 0)
		goto end;

	if (!thumbnail_data->video_stream) {
		fprintf(stderr,
				"Could not find audio or video stream in the input, aborting\n");
		ret = -3;
		goto end;
	}

	if (!thumbnail_data->frame) {
		fprintf(stderr, "Could not find frame\n");
		ret = -4;
		goto end;
	}

	if (gen_IDR_frame || gen_second > thumbnail_data->fmt_ctx->duration / AV_TIME_BASE)
		gen_second = 0;

reseek:
	if (!gen_IDR_frame) {
		seek_target = (int64_t)gen_second;
		seek_target = seek_target * 1000 * 1000;//AV_TIME_BASE;
		LOG("gen_thumbnail-->gen_second=%d,seek_target=%lld",gen_second,seek_target);
		seek_target = ffmpeg.av_rescale_q(seek_target, AV_TIME_BASE_Q,
				thumbnail_data->video_stream->time_base);
	    LOG("gen_thumbnail-->seek_target=%lld",seek_target);
//	ffmpeg.avformat_seek_file(fmt_ctx, thumbnail_data->video_stream_idx, 0, seek_target,
//			seek_target, //INT64_MAX
//			AVSEEK_FLAG_BACKWARD|AVSEEK_FLAG_ANY); //AVSEEK_FLAG_FRAME

		LOG("gen_thumbnail-->gen_second=%d,seek_target=%lld+++time_base->num=%d+++time_base->den=%d",
				gen_second,seek_target, thumbnail_data->video_stream->time_base.num, thumbnail_data->video_stream->time_base.den);

		if (seek_target < 0)
			seek_target = gen_second * 1000;
		int seek_type = AVSEEK_FLAG_BACKWARD;
		if(thumbnail_data->need_key_frame){
			seek_type |= AVSEEK_FLAG_FRAME;
		}
//		ffmpeg.av_seek_frame(thumbnail_data->fmt_ctx, thumbnail_data->video_stream_idx, seek_target,
//				seek_type);

		if(thumbnail_data->need_key_frame || seek_times >= 1)
			ffmpeg.avformat_seek_file(thumbnail_data->fmt_ctx, thumbnail_data->video_stream_idx, 0, seek_target,
						seek_target,
						seek_type);
		else
		    seek_stream(seek_target, seek_type, thumbnail_data);

		seek_times++;
		LOG("gen_thumbnail--->seek finished");

	}

	int got_right_frame = 0;
	int flag = 0;
	double pts;
	while (!got_right_frame && ffmpeg.av_read_frame(thumbnail_data->fmt_ctx, &packet) >= 0) {
		if (packet.stream_index == thumbnail_data->video_stream_idx) {
			ffmpeg.avcodec_decode_video2(thumbnail_data->video_dec_ctx, thumbnail_data->frame, &got_frame,
					&packet);
			if (got_frame) {
//				LOG("gen_thumbnail++dts=%lld++seek_target=%lld,thumbnail_data->need_key_frame=%d", packet.dts, seek_target, thumbnail_data->need_key_frame); //frame->pkt_pts
				if (!gen_IDR_frame && !thumbnail_data->need_key_frame) {
					if (packet.dts != AV_NOPTS_VALUE) {
						pts = packet.dts;
					} else {
						pts = 0;
					}
					int64_t current_time = ffmpeg.av_rescale_q(pts,
							thumbnail_data->video_stream->time_base, AV_TIME_BASE_Q);
					float current_time_f = (float) current_time / AV_TIME_BASE;
					float time_diff = current_time_f
							- gen_second;
//					LOG("gen_thumbnail current_time=%lld,gen_second=%d,current=%2.2f",current_time,gen_second,current_time_f);
//					if(thumbnail_data->frame->key_frame || thumbnail_data->frame->pict_type == AV_PICTURE_TYPE_I)
//						LOG("gen_thumbnail is key frame");
//					LOG("gen_thumbnail-->current_time=%lld----seek_target=%lld----gen_second=%d---time_diff=%f", current_time, seek_target, gen_second, time_diff);
					if (time_diff > -0.03) { //&& time_diff < 0.03
						got_right_frame = 1;
//						LOG("gen_thumbnail keyframe=%d,pict_type=%d",thumbnail_data->frame->key_frame,thumbnail_data->frame->pict_type);
					} else {
						if(time_diff <= -15 && seek_times == 1){
//							LOG("gen_thumbnail goto reseek");
							ffmpeg.av_free_packet(&packet);
							goto reseek;
						}
						else
							continue;
//						continue;
//						if (time_diff >= -10) {
//							ffmpeg.av_frame_free(&(thumbnail_data->frame));
//							thumbnail_data->frame = NULL;
//							thumbnail_data->frame = ffmpeg.av_frame_alloc();
//							if (!thumbnail_data->frame)
//								break;
//							continue;
//						} else { //if (time_diff > 10 || time_diff < -10)
//							got_right_frame = 1;
//						}
					}
				} else {
					if (thumbnail_data->frame->key_frame) {
						got_right_frame = 1;
					}
				}
			}
		}
		ffmpeg.av_free_packet(&packet);
	}
	if (gen_IDR_frame && !got_right_frame) {
		ffmpeg.avcodec_decode_video2(thumbnail_data->video_dec_ctx, thumbnail_data->frame, &got_frame, &packet);
		ffmpeg.av_free_packet(&packet);
	}

	return ret;

	end: if (thumbnail_data->frame) {
		ffmpeg.av_frame_free(&(thumbnail_data->frame));
		thumbnail_data->frame = NULL;
	}

	if (thumbnail_data->video_dec_ctx != NULL) {
		ffmpeg.avcodec_close(thumbnail_data->video_dec_ctx);
		thumbnail_data->video_dec_ctx = NULL;
	}
	if (thumbnail_data->fmt_ctx) {
		ffmpeg.avformat_close_input(&(thumbnail_data->fmt_ctx));
		thumbnail_data->fmt_ctx = NULL;
	}

	return ret;
}


//如果为0为执行成功
int write_png_file(char* file_name,int width,int height,unsigned short* buf)
{
    png_structp png_ptr;
    png_infop info_ptr;
    int number_of_passes;
    png_bytep * row_pointers; //图像的行数组

    FILE *fp = fopen(file_name, "wb");
    if (!fp)
    {
        LOGD("[write_png_file] File %s could not be opened for writing", file_name);
        return -1;
    }

    if(!ffmpeg.  png_create_write_struct)
    	return -11;

    png_ptr = ffmpeg.  png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);

    if (!png_ptr)
    {
        LOGD("[write_png_file] png_create_write_struct failed");
        return -2;
    }

    info_ptr = ffmpeg.  png_create_info_struct(png_ptr);
    if (!info_ptr)
    {
        LOGD("[write_png_file] png_create_info_struct failed");
        return -3;
    }

    if (setjmp(png_jmpbuf(png_ptr)))
    {
        LOGD("[write_png_file] Error during init_io");
        return -4;
    }

    ffmpeg.  png_init_io(png_ptr, fp);

    if (setjmp(png_jmpbuf(png_ptr)))
    {
        LOGD("[write_png_file] Error during writing header");
        return -5;
    }

    ffmpeg.  png_set_IHDR(png_ptr, info_ptr, width, height,
                 8, PNG_COLOR_TYPE_RGB, PNG_INTERLACE_NONE,
                 PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);

    ffmpeg.  png_write_info(png_ptr, info_ptr);

    if (setjmp(png_jmpbuf(png_ptr)))
    {
        LOGD("[write_png_file] Error during writing bytes");
        return -6;
    }

//    LOGD("==png begin png_write_image");

    row_pointers = (png_bytep*) malloc(sizeof(png_bytep) * height);

    unsigned short* buf_ptr = buf;
    unsigned short int mask_1 = 0xF800, mask_2 = 0x7E0, mask_3 = 0x1F;

    int y;
    for (y=0; y<height; y++)
    {
       row_pointers[y] =  (png_byte*) malloc(width*3);

       png_bytep row = row_pointers[y];

       int x;
       for (x=0; x<width; x++)
       {
            png_byte* ptr = &(row[x*3]);

            unsigned short color = *buf_ptr;
            ptr[0] = (unsigned char)((color & mask_1) >> 11) * 8; //R
            ptr[1] =  (unsigned char)((color & mask_2) >> 5) * 4;
            ptr[2] = (unsigned char)(color & mask_3) * 8; //B

            buf_ptr ++;
       }
    }

    ffmpeg.  png_write_image(png_ptr, row_pointers);
//    LOGD("==png end png_write_image");


    if (setjmp(png_jmpbuf(png_ptr)))
    {
        LOGD("[write_png_file] Error during end of write");
    }

    ffmpeg.  png_write_end(png_ptr, NULL);

    for (y=0; y<height; y++)
       free(row_pointers[y]);
    free(row_pointers);

    fclose(fp);
    LOGD("==png png_write_image finished");
}

AVPicture *get_rgb24_picture(const char *file, int gen_second, int *width,
		int *height, int gen_IDR_frame, mobo_thumbnail_data *thumbnail_data, int dst_img_format) {
	int ret = 0;
	thumbnail_data->frame = ffmpeg.av_frame_alloc();
//	thumbnail_data->fmt_ctx = (AVFormatContext *) malloc(sizeof(AVFormatContext));
//    memset(thumbnail_data->fmt_ctx, 0, sizeof(AVFormatContext));
	if (!thumbnail_data->frame) {
		return NULL;
	}

	ret = gen_thumbnail(file, gen_second, gen_IDR_frame, thumbnail_data);

	if (ret < 0 || thumbnail_data->video_dec_ctx->pix_fmt < 0) {
		if (thumbnail_data->frame)
			ffmpeg.av_frame_free(&(thumbnail_data->frame));
		LOG("get_rgb24_picture gen_thumbnail failed ret=%d", ret);
		return NULL;
	}

	if (*width <= 0 || *height <= 0) {
		*width = (thumbnail_data->video_dec_ctx->width);
		*height = (thumbnail_data->video_dec_ctx->height);
	}
//	LOG("gen_thumbnail---width=%d,height=%d...", *width, *height);
	LOG("get_rgb24_picture 1");
	ret = ffmpeg.avpicture_alloc(&(thumbnail_data->picture), dst_img_format, *width, *height);//AV_PIX_FMT_RGB565 AV_PIX_FMT_ARGB
	LOG("get_rgb24_picture after alloc");
	if (ret < 0) {
		if (thumbnail_data->frame)
			ffmpeg.av_frame_free(&(thumbnail_data->frame));
		LOG("get_rgb24_picture avpicture_alloc failed ret=%d", ret);
		return NULL;
	}

	thumbnail_data->sws_context = ffmpeg.sws_getCachedContext(thumbnail_data->sws_context, thumbnail_data->video_dec_ctx->width,
			thumbnail_data->video_dec_ctx->height, thumbnail_data->video_dec_ctx->pix_fmt, *width, *height,
			dst_img_format, SWS_FAST_BILINEAR, NULL, NULL, NULL);//AV_PIX_FMT_RGB565  AV_PIX_FMT_ARGB
	LOG("get_rgb24_picture after init context");

	ffmpeg.sws_scale(thumbnail_data->sws_context, (const uint8_t **) thumbnail_data->frame->data,
			thumbnail_data->frame->linesize, 0, thumbnail_data->video_dec_ctx->height, thumbnail_data->picture.data,
			thumbnail_data->picture.linesize);
	LOG("get_rgb24_picture after scale");

	if (thumbnail_data->frame) {
		ffmpeg.av_frame_free(&(thumbnail_data->frame));
		thumbnail_data->frame = NULL;
	}
	if (thumbnail_data->video_dec_ctx) {
		ffmpeg.avcodec_close(thumbnail_data->video_dec_ctx);
		thumbnail_data->video_dec_ctx = NULL;
	}
	if (thumbnail_data->fmt_ctx) {
		ffmpeg.avformat_close_input(&(thumbnail_data->fmt_ctx));
		thumbnail_data->fmt_ctx = NULL;
	}

	AVPicture *p = &(thumbnail_data->picture);
	LOG("get_rgb24_picture finished");
	return p;
}

void free_avpicture(AVPicture *picture) {
	ffmpeg.avpicture_free(picture);
}

