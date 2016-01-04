
CMP_LOC_FUNC long   cmp_get_play_time(cmp_hwnd hwnd) {
    CMPCoreData * coredata = (CMPCoreData*) hwnd;
    if (coredata == NULL) {
        LOG("cmp_get_play_time RETURN null");
        return 0;
    }

    if (coredata->reference_time == 0) {
        return 0;
    }

    if (coredata->pasue_time )
    {
        return coredata->pasue_time -  coredata->reference_time;
    }

    return (ffmpeg_func.  av_gettime() / 1000 - coredata->reference_time)/2;

}

void cmp_play(cmp_hwnd hwnd)
{
    //设定当前时间，
    if (!hwnd)return;
    if (hwnd->pasue_time) {
        //从暂定切换到开始播放
        long pasue_cost_time = ffmpeg_func.  av_gettime()/1000 - hwnd->pasue_time ;
        hwnd->reference_time += pasue_cost_time;
        hwnd->pasue_time = 0;
    } else {
        hwnd->reference_time =  ffmpeg_func.  av_gettime()/1000;
    }

    start_audio();
}
void check_seek(CMPCoreData * coredata)
{
    long time_diff = (coredata->seek_pos / 1000) - cmp_get_play_time(coredata);
    coredata->reference_time -=time_diff;
}

void audio_decodec_thread(){

    coredata->audio_clock = av_q2d(coredata->audio_st->time_base) *1000* audio_pkt->pts;

    buff_time = data_size *1000 /( n*coredata->audio_st->codec->sample_rate);

    curr_time = cmp_get_play_time(coredata);
    if ((coredata->audio_clock + buff_time )< curr_time) {
        //音频滞后，则不输出
        coredata->audio_clock = coredata->audio_clock + buff_time;
        break;
    }

    int delay = coredata->audio_clock - (curr_time+50);
    if (coredata->audio_output_count < 5 && delay<3000) delay =0;

    if (delay>15000) {
        //LOG("audio_decodec_thread DELAY = %d",delay);
        break;
    }

    CMP_ThreadDelay(delay);

    decode audio...;
}

void video_decodec_thread(){

    begin_time = (pts * 1000) * av_q2d(coredata->video_st->time_base);

    long curr_time,sleep_time;
    do {
        curr_time = cmp_get_play_time(coredata);
        sleep_time =  begin_time - curr_time;

        if (coredata->video_seek_flag > 0) { //如果seek，则直接输出当前帧
            sleep_time = 0;
            coredata->video_seek_flag --;
        }

        if (sleep_time>4) {
            CMP_ThreadDelay(5);
            sleep_time =sleep_time - 5;
            //LOGD("save_picture: sleeping!!!!!!!!!!!!!!!!!!!!!!!!!!!!:%d",sleep_time);
        }

    } while(sleep_time >10 && !coredata->exit);

    decode video...;
}
