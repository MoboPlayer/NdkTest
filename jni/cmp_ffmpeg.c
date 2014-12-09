/*
 * =====================================================================================
 *
 *       Filename:  cmp_ffmpeg.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  08/18/2014 10:33:10
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  sunjun 
 *   Organization:  
 *
 * =====================================================================================
 */
#include "cmp_ffmpeg.h"


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

	LOG("ffmpeg so loaded.begin = %s%s", lib_path, ffmpeg_filename);

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
	LOG("dlerror %s", dlerror());


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

	if ((error = dlerror()) != NULL)  {
		LOG("!!!can't find sym in ffmpeg.so : %s",error);
		return -1;
	}
	ffmpeg_func_inited = 1;
	return 0;
}
