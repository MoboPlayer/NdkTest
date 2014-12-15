#include <libavcodec\avcodec.h>
#include <libavformat\avformat.h>
#include <libswscale\swscale.h>

int open_audio(char * file_name, int audio_index) {
	AVFormatContext *av_fc = NULL;
	AVCodecContext *av_cc = NULL;
	int res = 0;

	ffmpeg.av_register_all();

	if (ffmpeg.av_format_open_input(&av_fc, file_name, NULL, NULL) < 0) {
		fprintf("open audio %s failed", file_name);
		return -1;
	}

	if (ffmpeg.av_find_stream_info(av_fc) < 0) {
		fprintf("find stream from %s failed", file_name);
		return -1;
	}

	int audio_index_in_streams = openCodecContext(av_fc,av_cc,audio_index); //需要打开的音频在所有stream中的index


}

static int openCodecContext(AVFormatContext *av_fc,
		AVCodecContext *av_cc, int index) {
	int index_of_audio = 0;
	int stream_index = -1;
	for (int i = 0; i < av_fc->nb_streams; i++) {
		if (av_fc->streams[i]->codec->codec_type == CODEC_TYPE_AUDIO) {
			if (index_of_audio++ == index) {
				stream_index = i;
				break;
			}
		}
	}

	if (!stream_index) {
		fprintf("no corresponding audio stream");
		return stream_index;
	}

	av_cc = av_fc->streams[stream_index]->codec;
	AVCodec *av_codec = NULL;
	av_codec = ffmpeg.avcodec_find_decoder(av_cc->codec_id);

	if (ffmpeg.avcodec_open2() < 0) {
		fprintf("open codec failed");
		return stream_index;
	}
	return stream_index;
}

static int decode_audio_frame(AVFormatContext *av_fc,
		AVCodecContext *av_cc){
	int res=0;
	AVPacket av_packet=NULL;
	AVFrame av_frame=NULL;
	ffmpeg.av_init_packet(&av_packet);

	while(ffmpeg.av_read_frame(av_fc,&av_packet)){
		int got_frame=0;
		av_frame=ffmpeg.avcodec_alloc_frame();
		res=ffmpeg.av_decode_audio4(av_cc,&got_frame,&packet);
		if(res<0){
			fprintf("decode audio failed");
			return -1;
		}
	}


}
