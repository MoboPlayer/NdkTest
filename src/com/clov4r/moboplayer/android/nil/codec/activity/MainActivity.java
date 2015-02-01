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
import java.io.FileNotFoundException;
import java.io.IOException;
import java.io.InputStream;

import android.content.Intent;
import android.net.Uri;
import android.os.Bundle;
import android.os.Environment;
import android.support.v7.app.ActionBarActivity;
import android.util.Log;
import android.view.Menu;
import android.view.MenuItem;
import android.widget.TextView;

import com.clov4r.moboplayer.android.nil.codec.R;
import com.clov4r.moboplayer.android.nil.codec.SubtitleJni;

public class MainActivity extends ActionBarActivity {
	final String videoName = Environment.getExternalStorageDirectory()
			+ "/Movies/[奥黛丽·赫本系列01：罗马假日].Roman.Holiday.1953.DVDRiP.X264.2Audio.AAC.HALFCD-NORM.Christian.mkv";

	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_main);
		SubtitleJni h = new SubtitleJni();
		TextView tv = (TextView) this.findViewById(R.id.hello);
		this.getFilesDir().getParent();
		String libpath = getFilesDir().getParent() + "/lib/";
		String libname = "libffmpeg_armv7_neon.so";
		String filePath = Environment.getExternalStorageDirectory()
				+ "/Movies/03181751_1684.MP4";// videoName;Gone.srt//Godzilla.srt//output_file_low.mkv

		Log.e("params", libpath + " " + filePath);
		StringBuffer sb = new StringBuffer();
		h.loadFFmpegLibs(libpath, libname);

		int count = h.isSubtitleExits(filePath);

		sb.append("字幕个数：" + count + "\n");
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

		tv.setText(sb.toString());

		h.closeSubtitle(0);

		Intent intent = new Intent(Intent.ACTION_VIEW);
		intent.setDataAndType(
				Uri.parse("http://27.221.44.43/67732D42DC34A840C9CBC9594E/0300010E0054C96DBA3EF603BAF2B16135A553-86F1-7270-8753-BBB5274B597B.flv"),
				"video/*");
		startActivity(intent);
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
}
