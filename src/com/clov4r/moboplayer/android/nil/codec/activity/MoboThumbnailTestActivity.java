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

import android.app.Activity;
import android.content.ComponentName;
import android.content.Intent;
import android.graphics.Bitmap;
import android.net.Uri;
import android.os.Bundle;
import android.os.Environment;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.Button;
import android.widget.ImageView;
import android.widget.LinearLayout;
import android.widget.Toast;

import com.clov4r.moboplayer.android.nil.codec.ScreenShotLibJni;
import com.clov4r.moboplayer.android.nil.codec.ScreenShotLibJni.OnBitmapCreatedListener;

public class MoboThumbnailTestActivity extends Activity {
	// final String videoName =
	// Environment.getExternalStorageDirectory()+"/dy/ppkard.mp4";
	// final String videoName =
	// Environment.getExternalStorageDirectory()+"/Movies/蒙羞之旅BD中英双字.rmvb";//[奥黛丽·赫本系列01：罗马假日].Roman.Holiday.1953.DVDRiP.X264.2Audio.AAC.HALFCD-NORM.Christian.mkv
	final String videoName = "/sdcard/Movies/01010020_0006.MP4";// /sdcard/Movies/播放失败导致mobo重启.m2ts

	final String img_save_path = Environment.getExternalStorageDirectory()
			+ "/mobo_screen_shot_%d.png";
	int index = 1;
	LinearLayout layout;
	ImageView imageView = null;

	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		layout = new LinearLayout(this);
		layout.setOrientation(LinearLayout.VERTICAL);
		imageView = new ImageView(this);

		Button button = new Button(this);
		layout.addView(button);
		layout.addView(imageView);
		button.setText("截图");
		setContentView(layout);
		button.setOnClickListener(mOnClickListener);
	}

	int flag = 1;
	OnClickListener mOnClickListener = new OnClickListener() {

		@Override
		public void onClick(View v) {
			// TODO Auto-generated method stub
			ScreenShotLibJni.getInstance().setOnBitmapCreatedListener(
					mOnBitmapCreatedListener);
			if (flag++ % 2 == 0)
				ScreenShotLibJni.getInstance().getScreenShot(videoName,
						"/sdcard/test.png", 35, 150, 150);
			else
				ScreenShotLibJni.getInstance().getIDRFrameThumbnail(videoName,
						"/sdcard/test.png", 0, 0);
			// imageView.setImageBitmap(bitmap);
//			 Intent intent=new Intent();
//			 intent.setComponent(new
//			 ComponentName("com.clov4r.moboplayer.android.nil",
//			 "com.clov4r.moboplayer.android.nil.MainInterface"));
//			
//			 intent.setData(Uri.parse(videoName));
//			 startActivity(intent);
		}
	};

	OnBitmapCreatedListener mOnBitmapCreatedListener = new OnBitmapCreatedListener() {
		@Override
		public void onBitmapCreated(final Bitmap bitmap, String fileName,
				String screenshotSavePath) {
			// TODO Auto-generated method stub
			// runOnUiThread(new Runnable() {
			// @Override
			// public void run() {
			// TODO Auto-generated method stub
			layout.removeView(imageView);
			layout.addView(imageView);
			imageView.setImageBitmap(bitmap);
			// }
			// });
		}

		@Override
		public void onBitmapCreatedFailed(String videoPath) {
			// TODO Auto-generated method stub
			Toast.makeText(MoboThumbnailTestActivity.this, "截图失败", Toast.LENGTH_SHORT).show();
		}
	};
}
