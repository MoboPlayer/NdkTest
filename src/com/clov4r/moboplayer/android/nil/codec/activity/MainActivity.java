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
package com.clov4r.moboplayer.android.nil.codec.activity;

import java.io.File;
import java.io.InputStream;
import java.nio.ByteBuffer;

import android.app.Activity;
import android.content.Intent;
import android.graphics.Bitmap;
import android.os.Bundle;
import android.os.Environment;
import android.os.Handler;
import android.os.Message;
import android.util.Log;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.Button;
import android.widget.ImageView;
import android.widget.TextView;

import com.clov4r.moboplayer.android.nil.codec.R;
import com.clov4r.moboplayer.android.nil.codec.SubtitleJni;

public class MainActivity extends Activity implements OnClickListener {
	// final String videoName = Environment.getExternalStorageDirectory()
	// +
	// "/Movies/";
	// 
	final String videoName = "/sdcard/movie/roman.mkv";// [老友记].Friends.S01E01.The.One.Where.It.AII.Began.mkv
	TextView tv = null;
	ImageView imageView;
	Button exchange;

	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_main);
		SubtitleJni h = new SubtitleJni();
		tv = (TextView) this.findViewById(R.id.hello);
		imageView = (ImageView) findViewById(R.id.imageView);
		exchange = (Button) findViewById(R.id.exchange);
		exchange.setOnClickListener(this);
		this.getFilesDir().getParent();
		String libpath = getFilesDir().getParent() + "/lib/";
		String libname = "libffmpeg.so";// libffmpeg_armv7_neon.so
		String filePath = Environment.getExternalStorageDirectory()
				+ "/Movies/output_file_low.mkv";// videoName;Gone.srt//Godzilla.srt//output_file_low.mkv

		Log.e("params", libpath + " " + filePath);
		StringBuffer sb = new StringBuffer();
		h.loadFFmpegLibs(libpath, libname);

		// int count = h.isSubtitleExits(filePath);

		// sb.append("字幕个数：" + count + "\n");
		// for(int i=0;i<count;i++) {
		// sb.append("第"+i+"个字幕："+h.getSubtitleLanguage(filePath, i)+"\n");
		// }

		// int temp = h.openSubtitleFile(filePath,0,0);
		// sb.append("open subtitle file :"+(temp<0?"失败":"成功")+"\n");
		// sb.append("getSubtitleType :"+h.getSubtitleType(0)+"\n");
		// sb.append("getSubtitleLanguage :"+h.getSubtitleLanguageInfo(filePath)+"\n");
		// /*
		// * 读取300秒内的字幕，每500毫秒读取一次
		// */
		// for(int i=0,j=0;i<300;i++){
		// String str = h.getSubtitleByTime(i*1000,0);
		// if(str!=null){
		// if(!sb.toString().endsWith("subtitle:"+str+"\n")){
		// Log.e("subtitle", str==null?" null ": str);
		// sb.append("index:"+(++j)+" time:"+i/2+"s subtitle:"+str+"\n");
		// }
		//
		//
		// }
		// }

		// tv.setText(sb.toString());
		//
		// h.closeSubtitle(0);

		int numOfSubtitle = SubtitleJni.getInstance()
				.isSubtitleExits(videoName);
		if (numOfSubtitle > 0) {
			int flag = SubtitleJni.getInstance().openSubtitleFile_2(videoName,
					0);
			int type = SubtitleJni.getInstance().getSubtitleType(0);
			Log.e("", ""+type);
			// mThread.start();
			// getSubtitle();
			mThread.start();
		}
	}

	public void onDestroy() {
		super.onDestroy();
	}

	Thread mThread = new Thread() {
		@Override
		public void run() {
			// getSubtitle();
			getTextSubtitle();
		}
	};

	public void getTextSubtitle() {
		int timeBegin = 0;
		int timeEnd = 3 * 60 * 1000;
		for (int i = timeBegin; i < timeEnd; i += 1000) {
			// tv.setText(getSubtitle(i));
			Log.e("", "" + getSubtitle(i));
			try {
				Thread.sleep(100);
			} catch (InterruptedException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
		}
	}

	public void getSubtitle() {
		int timeBegin = 0;
		int timeEnd = 3 * 60 * 1000;
		int type = SubtitleJni.getInstance().getSubtitleType(0);
		Log.e("", "" + type);
		ByteBuffer byteBuffer = ByteBuffer.allocateDirect(1024 * 1024);
		for (int t = timeBegin; t < timeEnd; t += 1000) {
			// Log.e("testMobo", "150204 - t =" + t + "subtitle = "
			// + getSubtitle(t));

			Bitmap bitmap = SubtitleJni.getInstance().getImageSubtitleByTime(t,
					byteBuffer);
			if (bitmap != null) {
				// ImageSpan imgSpan = new ImageSpan(this, bitmap);
				// SpannableString spanString = new SpannableString("icon");
				// spanString.setSpan(imgSpan, 0, 4,
				// Spannable.SPAN_EXCLUSIVE_EXCLUSIVE);
				// tv.setText(spanString);
				// imageView.setImageBitmap(bitmap);
				Message msg = new Message();
				msg.obj = bitmap;
				mHandler.sendMessage(msg);
				try {
					Thread.sleep(1000);
				} catch (InterruptedException e) {
					// TODO Auto-generated catch block
					e.printStackTrace();
				}
				// break;
			} else
				imageView.setBackgroundResource(R.drawable.ic_launcher);
			// try {
			// sleep(50);
			// } catch (InterruptedException e) {
			// // TODO Auto-generated catch block
			// e.printStackTrace();
			// }
		}

	}

	Handler mHandler = new Handler() {
		@Override
		public void handleMessage(Message msg) {
			Bitmap bitmap = (Bitmap) msg.obj;
			imageView.setImageBitmap(bitmap);
		}
	};

	protected String getSubtitle(int currentTime) {
		return SubtitleJni.getInstance().getSubtitleByTime_2(currentTime);
	}

	private boolean isUtf8Encode(String filePath) {
		if (!filePath.endsWith("srt")) {
			return true;
		}
		InputStream in = null;
		try {
			File file = new File(filePath);
			in = new java.io.FileInputStream(file);
			byte[] b = new byte[3];
			in.read(b);
			in.close();
			if (b[0] == -17 && b[1] == -69 && b[2] == -65)
				return true;
			else
				return false;
		} catch (Exception e) {
			e.printStackTrace();
			return true;
		} finally {
			if (in != null) {
				try {
					in.close();
				} catch (Exception e) {
				}
			}
		}
	}

	void load() {
		String libpath = getFilesDir().getParent() + "/lib/";
		String libname = "libffmpeg_armv7_neon.so";
		SubtitleJni.getInstance().loadFFmpegLibs(libpath, libname);
	}

	@Override
	public boolean onCreateOptionsMenu(Menu menu) {
		// Inflate the menu; this adds items to the action bar if it is present.
		getMenuInflater().inflate(R.menu.main, menu);
		return true;
	}

	@Override
	public boolean onOptionsItemSelected(MenuItem item) {
		// Handle action bar item clicks here. The action bar will
		// automatically handle clicks on the Home/Up button, so long
		// as you specify a parent activity in AndroidManifest.xml.
		int id = item.getItemId();
		if (id == R.id.action_settings) {
			Intent intent = new Intent(this, MoboThumbnailTestActivity.class);
			startActivity(intent);
		} else if (id == R.id.action_download) {
			Intent intent = new Intent(this, StreamingDownloadActivity.class);
			startActivity(intent);
		}
		return super.onOptionsItemSelected(item);
	}

	int index = 0;

	@Override
	public void onClick(View v) {
		// TODO Auto-generated method stub
		if (exchange == v) {

			SubtitleJni.getInstance().closeSubtitle(0);
		}
	}
}
