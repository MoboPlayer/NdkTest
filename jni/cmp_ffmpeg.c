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

ffmpeg_func_t ffmpeg;

int init_ffmpeg_func(const char *lib_path, const char * ffmpeg_filename, ffmpeg_func_t *ffmpeg_func_p)
{
    static int ffmpeg_func_inited = 0;
	void *handle = NULL;
	void * handle_libpng = NULL;
	char *error;
	char path[255];
	char png_path[255];
	path[0]=0;
	png_path[0]=0;

//	LOG("ffmpeg so loaded.begin = %s%s", lib_path, ffmpeg_filename);

	time_t dead_time = time(NULL);

//	if (dead_time > 1388805578) {
//		
//		LOG("Out of date!!!!!!!!!");
//		return 0;
//	}

	if (ffmpeg_func_inited) return 0;
	memset(ffmpeg_func_p, 0, sizeof(ffmpeg_func_t));

	sprintf(path,"%s%s", lib_path, ffmpeg_filename);
	handle = (void *)dlopen (path, RTLD_NOW);
//	LOG("dlerror %s", dlerror());


	//====load ffmpeg lib
	ffmpeg_func_p->av_dup_packet = dlsym(handle, "av_dup_packet");
	FF_FUNC_CHECK(ffmpeg_func_p->av_dup_packet)

	ffmpeg_func_p->av_mallocz = dlsym(handle, "av_mallocz");
	FF_FUNC_CHECK(ffmpeg_func_p->av_mallocz)

	ffmpeg_func_p->avformat_find_stream_info = dlsym(handle, "avformat_find_stream_info");
	FF_FUNC_CHECK(ffmpeg_func_p->avformat_find_stream_info)

	ffmpeg_func_p->avformat_close_input = dlsym(handle, "avformat_close_input");
	FF_FUNC_CHECK(ffmpeg_func_p->avformat_close_input)

	ffmpeg_func_p->av_free = dlsym(handle, "av_free");
	FF_FUNC_CHECK(ffmpeg_func_p->av_free)

	ffmpeg_func_p->av_free_packet = dlsym(handle, "av_free_packet");
	FF_FUNC_CHECK(ffmpeg_func_p->av_free_packet)

	ffmpeg_func_p->av_freep = dlsym(handle, "av_freep");
	FF_FUNC_CHECK(ffmpeg_func_p->av_freep)

	ffmpeg_func_p->av_gettime = dlsym(handle, "av_gettime");
	FF_FUNC_CHECK(ffmpeg_func_p->av_gettime)

	ffmpeg_func_p->av_index_search_timestamp = dlsym(handle, "av_index_search_timestamp");
	FF_FUNC_CHECK(ffmpeg_func_p->av_index_search_timestamp)

	ffmpeg_func_p->av_init_packet = dlsym(handle, "av_init_packet");
	FF_FUNC_CHECK(ffmpeg_func_p->av_init_packet)

	ffmpeg_func_p->av_malloc = dlsym(handle, "av_malloc");
	FF_FUNC_CHECK(ffmpeg_func_p->av_malloc)

	ffmpeg_func_p->avformat_open_input = dlsym(handle, "avformat_open_input");
	FF_FUNC_CHECK(ffmpeg_func_p->avformat_open_input)

	ffmpeg_func_p->av_read_frame = dlsym(handle, "av_read_frame");
	FF_FUNC_CHECK(ffmpeg_func_p->av_read_frame)

	ffmpeg_func_p->av_register_all = dlsym(handle, "av_register_all");
	FF_FUNC_CHECK(ffmpeg_func_p->av_register_all)

	ffmpeg_func_p->av_rescale_q = dlsym(handle, "av_rescale_q");
	FF_FUNC_CHECK(ffmpeg_func_p->av_rescale_q)

	ffmpeg_func_p->av_rescale = dlsym(handle, "av_rescale");
	FF_FUNC_CHECK(ffmpeg_func_p->av_rescale)

	ffmpeg_func_p->av_seek_frame = dlsym(handle, "av_seek_frame");
	FF_FUNC_CHECK(ffmpeg_func_p->av_seek_frame)

	ffmpeg_func_p->avpicture_free = dlsym(handle, "avpicture_free");
	FF_FUNC_CHECK(ffmpeg_func_p->avpicture_free)

	ffmpeg_func_p->avcodec_find_decoder = dlsym(handle, "avcodec_find_decoder");
	FF_FUNC_CHECK(ffmpeg_func_p->avcodec_find_decoder)

	ffmpeg_func_p->avcodec_open2 = dlsym(handle, "avcodec_open2");
	FF_FUNC_CHECK(ffmpeg_func_p->avcodec_open2)

	ffmpeg_func_p->avcodec_flush_buffers = dlsym(handle, "avcodec_flush_buffers");
	FF_FUNC_CHECK(ffmpeg_func_p->avcodec_flush_buffers)

	ffmpeg_func_p->avcodec_alloc_frame = dlsym(handle, "avcodec_alloc_frame");
	FF_FUNC_CHECK(ffmpeg_func_p->avcodec_alloc_frame)

	ffmpeg_func_p->avcodec_decode_video2 = dlsym(handle, "avcodec_decode_video2");
	FF_FUNC_CHECK(ffmpeg_func_p->avcodec_decode_video2)

	ffmpeg_func_p->avpicture_alloc = dlsym(handle, "avpicture_alloc");
	FF_FUNC_CHECK(ffmpeg_func_p->avpicture_alloc)

	ffmpeg_func_p->avcodec_close = dlsym(handle, "avcodec_close");
	FF_FUNC_CHECK(ffmpeg_func_p->avcodec_close)

	ffmpeg_func_p->avformat_seek_file = dlsym(handle, "avformat_seek_file");
	FF_FUNC_CHECK(ffmpeg_func_p->avformat_seek_file)

	ffmpeg_func_p->avcodec_version = dlsym(handle, "avcodec_version");
	FF_FUNC_CHECK(ffmpeg_func_p->avcodec_version)

	ffmpeg_func_p->avformat_version = dlsym(handle, "avformat_version");
	FF_FUNC_CHECK(ffmpeg_func_p->avformat_version)

	ffmpeg_func_p->avutil_version = dlsym(handle, "avutil_version");
	FF_FUNC_CHECK(ffmpeg_func_p->avutil_version)

	ffmpeg_func_p->avio_size = dlsym(handle, "avio_size");
	FF_FUNC_CHECK(ffmpeg_func_p->avio_size)

	ffmpeg_func_p->avcodec_decode_audio3 = dlsym(handle, "avcodec_decode_audio3");
	FF_FUNC_CHECK(ffmpeg_func_p->avcodec_decode_audio3)

	ffmpeg_func_p->avcodec_decode_audio4 = dlsym(handle, "avcodec_decode_audio4");
	FF_FUNC_CHECK(ffmpeg_func_p->avcodec_decode_audio4)

	ffmpeg_func_p->av_dict_get = dlsym(handle, "av_dict_get");
	FF_FUNC_CHECK(ffmpeg_func_p->av_dict_get)

	ffmpeg_func_p->avcodec_get_name = dlsym(handle, "avcodec_get_name");
	FF_FUNC_CHECK(ffmpeg_func_p->avcodec_get_name)

	ffmpeg_func_p->swr_alloc_set_opts = dlsym(handle, "swr_alloc_set_opts");
	FF_FUNC_CHECK(ffmpeg_func_p->swr_alloc_set_opts)

	ffmpeg_func_p->swr_init = dlsym(handle, "swr_init");
	FF_FUNC_CHECK(ffmpeg_func_p->swr_init);

	ffmpeg_func_p->swr_free = dlsym(handle, "swr_free");
	FF_FUNC_CHECK(ffmpeg_func_p->swr_free);

	ffmpeg_func_p->av_get_bytes_per_sample = dlsym(handle, "av_get_bytes_per_sample");
	FF_FUNC_CHECK(ffmpeg_func_p->av_get_bytes_per_sample);

	ffmpeg_func_p->avcodec_get_frame_defaults = dlsym(handle, "avcodec_get_frame_defaults");
	FF_FUNC_CHECK(ffmpeg_func_p->avcodec_get_frame_defaults);

	ffmpeg_func_p->av_frame_unref = dlsym(handle, "av_frame_unref");
	FF_FUNC_CHECK(ffmpeg_func_p->av_frame_unref);

	ffmpeg_func_p->av_get_channel_layout_nb_channels = dlsym(handle, "av_get_channel_layout_nb_channels");
	FF_FUNC_CHECK(ffmpeg_func_p->av_get_channel_layout_nb_channels);

	ffmpeg_func_p->av_get_default_channel_layout = dlsym(handle, "av_get_default_channel_layout");
	FF_FUNC_CHECK(ffmpeg_func_p->av_get_default_channel_layout);

	ffmpeg_func_p->av_frame_get_channels = dlsym(handle, "av_frame_get_channels");
	FF_FUNC_CHECK(ffmpeg_func_p->av_frame_get_channels);

	ffmpeg_func_p->av_samples_get_buffer_size = dlsym(handle, "av_samples_get_buffer_size");
	FF_FUNC_CHECK(ffmpeg_func_p->av_samples_get_buffer_size);

	ffmpeg_func_p->avcodec_decode_subtitle2 = dlsym(handle, "avcodec_decode_subtitle2");
	FF_FUNC_CHECK(ffmpeg_func_p->avcodec_decode_subtitle2);

	ffmpeg_func_p->avsubtitle_free = dlsym(handle, "avsubtitle_free");
	FF_FUNC_CHECK(ffmpeg_func_p->avsubtitle_free);

	ffmpeg_func_p->av_frame_alloc = dlsym(handle, "av_frame_alloc");
	FF_FUNC_CHECK(ffmpeg_func_p->av_frame_alloc);

	ffmpeg_func_p->av_frame_free = dlsym(handle, "av_frame_free");
	FF_FUNC_CHECK(ffmpeg_func_p->av_frame_free);

	ffmpeg_func_p->av_strerror = dlsym(handle, "av_strerror");
	FF_FUNC_CHECK(ffmpeg_func_p->av_strerror);

	ffmpeg_func_p->av_find_best_stream = dlsym(handle, "av_find_best_stream");
	FF_FUNC_CHECK(ffmpeg_func_p->av_find_best_stream);

	ffmpeg_func_p->av_get_media_type_string = dlsym(handle, "av_get_media_type_string");
	FF_FUNC_CHECK(ffmpeg_func_p->av_get_media_type_string);

	ffmpeg_func_p->av_dump_format = dlsym(handle, "av_dump_format");
	FF_FUNC_CHECK(ffmpeg_func_p->av_dump_format);

	ffmpeg_func_p->sws_getCachedContext = dlsym(handle, "sws_getCachedContext");
	FF_FUNC_CHECK(ffmpeg_func_p->sws_getCachedContext);

	ffmpeg_func_p->sws_scale = dlsym(handle, "sws_scale");
	FF_FUNC_CHECK(ffmpeg_func_p->sws_scale);

	ffmpeg_func_p->avformat_alloc_output_context2 = dlsym(handle, "avformat_alloc_output_context2");
	FF_FUNC_CHECK(ffmpeg_func_p->avformat_alloc_output_context2);

	ffmpeg_func_p->avformat_new_stream = dlsym(handle, "avformat_new_stream");
	FF_FUNC_CHECK(ffmpeg_func_p->avformat_new_stream);

	ffmpeg_func_p->avcodec_copy_context = dlsym(handle, "avcodec_copy_context");
	FF_FUNC_CHECK(ffmpeg_func_p->avcodec_copy_context);

	ffmpeg_func_p->avio_open = dlsym(handle, "avio_open");
	FF_FUNC_CHECK(ffmpeg_func_p->avio_open);

	ffmpeg_func_p->avformat_write_header = dlsym(handle, "avformat_write_header");
	FF_FUNC_CHECK(ffmpeg_func_p->avformat_write_header);

	ffmpeg_func_p->av_rescale_q_rnd = dlsym(handle, "av_rescale_q_rnd");
	FF_FUNC_CHECK(ffmpeg_func_p->av_rescale_q_rnd);

	ffmpeg_func_p->av_interleaved_write_frame = dlsym(handle, "av_interleaved_write_frame");
	FF_FUNC_CHECK(ffmpeg_func_p->av_interleaved_write_frame);

	ffmpeg_func_p->avio_close = dlsym(handle, "avio_close");
	FF_FUNC_CHECK(ffmpeg_func_p->avio_close);

	ffmpeg_func_p->av_write_trailer = dlsym(handle, "av_write_trailer");
	FF_FUNC_CHECK(ffmpeg_func_p->av_write_trailer);

	ffmpeg_func_p->avformat_free_context = dlsym(handle, "avformat_free_context");
	FF_FUNC_CHECK(ffmpeg_func_p->avformat_free_context);

	ffmpeg_func_p->avio_seek_time = dlsym(handle, "avio_seek_time");
	FF_FUNC_CHECK(ffmpeg_func_p->avio_seek_time);

	ffmpeg_func_p->avformat_network_init = dlsym(handle, "avformat_network_init");
	FF_FUNC_CHECK(ffmpeg_func_p->avformat_network_init);

	ffmpeg_func_p->avio_skip = dlsym(handle, "avio_skip");
	FF_FUNC_CHECK(ffmpeg_func_p->avio_skip);

	ffmpeg_func_p->avformat_alloc_context = dlsym(handle, "avformat_alloc_context");
	FF_FUNC_CHECK(ffmpeg_func_p->avformat_alloc_context);

	ffmpeg_func_p->avio_flush = dlsym(handle, "avio_flush");
	FF_FUNC_CHECK(ffmpeg_func_p->avio_flush);

	ffmpeg_func_p->av_usleep = dlsym(handle, "av_usleep");
	FF_FUNC_CHECK(ffmpeg_func_p->av_usleep);

	ffmpeg_func_p->avio_read = dlsym(handle, "avio_read");
	FF_FUNC_CHECK(ffmpeg_func_p->avio_read);

	ffmpeg_func_p->avio_write = dlsym(handle, "avio_write");
	FF_FUNC_CHECK(ffmpeg_func_p->avio_write);

	sprintf(png_path,"%s%s", lib_path, "libpng.so");
	handle_libpng =dlopen (png_path, RTLD_LAZY);
	ffmpeg_func_p->png_create_write_struct =dlsym(handle_libpng, "png_create_write_struct");
	FF_FUNC_CHECK(ffmpeg_func_p->png_create_write_struct);
	ffmpeg_func_p->png_create_info_struct =dlsym(handle_libpng, "png_create_info_struct");
	FF_FUNC_CHECK(ffmpeg_func_p->png_create_info_struct);
	ffmpeg_func_p->png_init_io =dlsym(handle_libpng, "png_init_io");
	FF_FUNC_CHECK(ffmpeg_func_p->png_init_io);
	ffmpeg_func_p->png_set_IHDR =dlsym(handle_libpng, "png_set_IHDR");
	FF_FUNC_CHECK(ffmpeg_func_p->png_set_IHDR);
	ffmpeg_func_p->png_write_image =dlsym(handle_libpng, "png_write_image");
	FF_FUNC_CHECK(ffmpeg_func_p->png_write_image);
	ffmpeg_func_p->png_write_end =dlsym(handle_libpng, "png_write_end");
	FF_FUNC_CHECK(ffmpeg_func_p->png_write_end);
	ffmpeg_func_p->png_write_info =dlsym(handle_libpng, "png_write_info");
	FF_FUNC_CHECK(ffmpeg_func_p->png_write_info);
	ffmpeg_func_p->png_set_shift =dlsym(handle_libpng, "png_set_shift");
	FF_FUNC_CHECK(ffmpeg_func_p->png_set_shift);
	ffmpeg_func_p->png_set_sBIT =dlsym(handle_libpng, "png_set_sBIT");
	FF_FUNC_CHECK(ffmpeg_func_p->png_set_sBIT);


	//////speed
    ffmpeg_func_p->avfilter_register_all = dlsym(handle, "avfilter_register_all");
    FF_FUNC_CHECK(ffmpeg_func_p->avfilter_register_all);
    ffmpeg_func_p->avfilter_get_by_name = dlsym(handle, "avfilter_get_by_name");
    FF_FUNC_CHECK(ffmpeg_func_p->avfilter_get_by_name);
    ffmpeg_func_p->avfilter_graph_alloc = dlsym(handle, "avfilter_graph_alloc");
    FF_FUNC_CHECK(ffmpeg_func_p->avfilter_graph_alloc);
    ffmpeg_func_p->avfilter_graph_create_filter = dlsym(handle, "avfilter_graph_create_filter");
    FF_FUNC_CHECK(ffmpeg_func_p->avfilter_graph_create_filter);
    ffmpeg_func_p->avfilter_graph_config = dlsym(handle, "avfilter_graph_config");
    FF_FUNC_CHECK(ffmpeg_func_p->avfilter_graph_config);
    ffmpeg_func_p->av_buffersrc_write_frame = dlsym(handle, "av_buffersrc_write_frame");
    FF_FUNC_CHECK(ffmpeg_func_p->av_buffersrc_write_frame);
    ffmpeg_func_p->av_buffersink_get_frame = dlsym(handle, "av_buffersink_get_frame");
    FF_FUNC_CHECK(ffmpeg_func_p->av_buffersink_get_frame);
    ffmpeg_func_p->avfilter_graph_free = dlsym(handle, "avfilter_graph_free");
    FF_FUNC_CHECK(ffmpeg_func_p->avfilter_graph_free);
    ffmpeg_func_p->avfilter_inout_alloc = dlsym(handle, "avfilter_inout_alloc");
    FF_FUNC_CHECK(ffmpeg_func_p->avfilter_inout_alloc);
    ffmpeg_func_p->av_get_sample_fmt_name = dlsym(handle, "av_get_sample_fmt_name");
    FF_FUNC_CHECK(ffmpeg_func_p->av_get_sample_fmt_name);
    ffmpeg_func_p->av_get_channel_layout_string = dlsym(handle, "av_get_channel_layout_string");
    FF_FUNC_CHECK(ffmpeg_func_p->av_get_channel_layout_string);
    ffmpeg_func_p->avfilter_inout_free = dlsym(handle, "avfilter_inout_free");
    FF_FUNC_CHECK(ffmpeg_func_p->avfilter_inout_free);
    ffmpeg_func_p->avfilter_graph_parse_ptr = dlsym(handle, "avfilter_graph_parse_ptr");
    FF_FUNC_CHECK(ffmpeg_func_p->avfilter_graph_parse_ptr);
    ffmpeg_func_p->av_strdup = dlsym(handle, "av_strdup");
    FF_FUNC_CHECK(ffmpeg_func_p->av_strdup);
    ffmpeg_func_p->av_frame_free = dlsym(handle, "av_frame_free");
    FF_FUNC_CHECK(ffmpeg_func_p->av_frame_free);
    ffmpeg_func_p->av_int_list_length_for_size = dlsym(handle, "av_int_list_length_for_size");
    FF_FUNC_CHECK(ffmpeg_func_p->av_int_list_length_for_size);
    ffmpeg_func_p->av_opt_set_bin = dlsym(handle, "av_opt_set_bin");
    FF_FUNC_CHECK(ffmpeg_func_p->av_opt_set_bin);

	//////speed
	if ((error = dlerror()) != NULL)  {
		LOG("!!!can't find sym in ffmpeg.so : %s",error);
		return -1;
	}
	ffmpeg_func_inited = 1;
	return 0;
}
