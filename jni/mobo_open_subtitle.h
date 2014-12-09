
#include "cmp_ffmpeg.h"


#ifndef SUB_FFMPEG_H_
#define SUB_FFMPEG_H_

typedef struct SubtitleData {
    AVFormatContext     *fmt_ctx;
    AVCodecContext      *dec_ctx;
    AVStream            *stream;
    AVSubtitle          *subtitles_array;
    int                 subtitle_index;
    int                 subtitle_size;
}SubtitleData;

typedef SubtitleData *sub_data_p;


void init_ffmpeg(const char *lib_path, const char * ffmpeg_filename);
int open_subtitle(const char *file, int stream_index);
char *get_subtitle_ontime(int cur_time);
void close_subtitle();
#endif
