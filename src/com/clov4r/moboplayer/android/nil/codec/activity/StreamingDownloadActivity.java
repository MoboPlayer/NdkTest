package com.clov4r.moboplayer.android.nil.codec.activity;

import java.io.File;

import android.app.Activity;
import android.os.Bundle;
import android.os.Environment;
import android.os.Handler;
import android.os.Message;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.Button;
import android.widget.SeekBar;
import android.widget.TextView;
import android.widget.Toast;

import com.clov4r.moboplayer.android.nil.codec.R;
import com.clov4r.moboplayer.android.nil.codec.StreamingDownloadManager;
import com.clov4r.moboplayer.android.nil.codec.StreamingDownloadManager.MoboDownloadListener;
import com.clov4r.moboplayer.android.nil.codec.StreamingDownloadManager.StreamingDownloadData;

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
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
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
public class StreamingDownloadActivity extends Activity {
	final String url = // "rtmp://183.62.232.213/fileList/video/flv/1/test.flv";
	"http://27.221.44.43/67732F5E9B83B83379D5B74AF9/0300010E0054C96DBA3EF603BAF2B16135A553-86F1-7270-8753-BBB5274B597B.flv";
	String savePath = Environment.getExternalStorageDirectory()
			.getAbsolutePath() + File.separator;
	TextView text_view;
	SeekBar seek_bar;
	Button button, button_2;

	StreamingDownloadManager mStreamingDownloadManager=null;

	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_download);
		text_view = (TextView) findViewById(R.id.text_view);
		seek_bar = (SeekBar) findViewById(R.id.seek_bar);
		button = (Button) findViewById(R.id.button);
		button_2 = (Button) findViewById(R.id.button_2);
		button.setOnClickListener(mOnClickListener);
		button_2.setOnClickListener(mOnClickListener);

		savePath += "mobo_download_test.mkv";
		mStreamingDownloadManager = StreamingDownloadManager.getInstance(this);
		mStreamingDownloadManager.setDownloadListener(mMoboDownloadListener);
		// mStreamingDownloadLib.startDownload(url, savePath);
		seek_bar.setMax(1);
	}

	public void onDestroy() {
		super.onDestroy();
		mStreamingDownloadManager.stopAll();
	}
	final int msg_progress_changed = 111;
	final int msg_download_finished = 112;
	final int msg_download_failed = 113;
	Handler mHandler = new Handler() {
		public void handleMessage(Message msg) {
			Object obj = msg.obj;
			StreamingDownloadData streamingData;
			// if(obj instanceof StreamingDownloadData)
			streamingData = (StreamingDownloadData) obj;
			switch (msg.what) {
			case msg_progress_changed:
				if (seek_bar.getMax() == 1) {
					int duration = mStreamingDownloadManager
							.getDurationOf(streamingData.id);
					seek_bar.setMax(duration);
					Toast.makeText(StreamingDownloadActivity.this,
							"时长是：" + duration, Toast.LENGTH_LONG).show();
				}
				seek_bar.setProgress(streamingData.currentTime);
				break;
			case msg_download_finished:
				Toast.makeText(StreamingDownloadActivity.this,
						streamingData.fileSavePath + " has download finished",
						Toast.LENGTH_LONG).show();
				break;
			case msg_download_failed:
				Toast.makeText(StreamingDownloadActivity.this,
						streamingData.fileSavePath + " has download finished",
						Toast.LENGTH_LONG).show();
				break;
			}
		}
	};

	MoboDownloadListener mMoboDownloadListener = new MoboDownloadListener() {

		@Override
		public void onDownloadProgressChanged(StreamingDownloadData data,
				int currentTime) {
			// TODO Auto-generated method stub
			sendMessage(msg_progress_changed, data);
		}

		@Override
		public void onDownloadFinished(StreamingDownloadData data) {
			// TODO Auto-generated method stub
			sendMessage(msg_download_finished, data);
		}

		@Override
		public void onDownloadFailed(StreamingDownloadData data) {
			// TODO Auto-generated method stub
			sendMessage(msg_download_failed, data);
		}
	};

	/** 0:停止；1，正在下载；-1，暂停状态 **/
	int downloadFlag = 0;
	int downloadId;
	OnClickListener mOnClickListener = new OnClickListener() {

		@Override
		public void onClick(View v) {
			// TODO Auto-generated method stub
			if (v == button_2) {
				if (downloadFlag != 0) {
					mStreamingDownloadManager.stopDownload(downloadId);
//					mStreamingDownloadManager.removeDownload(downloadId, false);
					button.setText("stoped");
					downloadFlag = 0;
				}
			} else if (v == button) {
				if (downloadFlag == 0) {
					downloadId = mStreamingDownloadManager.startDownload(url,
							savePath);
					button.setText("downloading...");
					downloadFlag = 1;
				} else if (downloadFlag == 1) {
					button.setText("pausing...");
					mStreamingDownloadManager.pauseDownload(downloadId);
					downloadFlag = -1;
				} else if (downloadFlag == -1) {
					button.setText("downloading...");
					mStreamingDownloadManager.resumeDownload(downloadId);
					downloadFlag = 1;
				}
			}
		}
	};

	void sendMessage(int what, Object obj) {
		Message msg = mHandler.obtainMessage();
		msg.what = what;
		msg.obj = obj;
		mHandler.sendMessage(msg);
	}
}
