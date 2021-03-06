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
import android.graphics.Bitmap;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Paint;
import android.graphics.PorterDuffXfermode;
import android.graphics.Rect;
import android.graphics.RectF;
import android.graphics.PorterDuff.Mode;
import android.os.Bundle;
import android.os.Environment;
import android.support.v4.graphics.drawable.RoundedBitmapDrawable;
import android.support.v4.graphics.drawable.RoundedBitmapDrawableFactory;
import android.util.Log;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.Button;
import android.widget.ImageView;
import android.widget.ImageView.ScaleType;
import android.widget.LinearLayout;
import android.widget.Toast;

import com.clov4r.moboplayer.android.nil.codec.ScreenShotLibJni;
import com.clov4r.moboplayer.android.nil.codec.SubtitleJni;
import com.clov4r.moboplayer.android.nil.codec.ScreenShotLibJni.OnBitmapCreatedListener;

public class MoboThumbnailTestActivity extends Activity {
	String videoName = "/sdcard/Movies/静流-恋文.mkv";// [阳光电影www.ygdy8.com].安妮：纽约奇缘.BD.720p.中英双字幕.rmvb
	//rtsp://192.168.42.1/tmp/fuse_d/share/2015-01-01-16-50-47.MP4 

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
		SubtitleJni h = new SubtitleJni();
		String libpath = getFilesDir().getParent() + "/lib/";
		String libname = "libffmpeg.so";// libffmpeg_armv7_neon.so
		h.loadFFmpegLibs(libpath, libname);

		Button button = new Button(this);
		layout.addView(button);
		layout.addView(imageView);
		button.setText("截图");
		setContentView(layout);
		button.setOnClickListener(mOnClickListener);
		
	}

	Bitmap bitmap = null;
	int flag = 1;
	int time = 165;//纽约奇缘第3600 + 1200 + 57秒花屏
	OnClickListener mOnClickListener = new OnClickListener() {

		@Override
		public void onClick(View v) {
			// TODO Auto-generated method stub
			// ScreenShotLibJni.getInstance().setOnBitmapCreatedListener(
			// mOnBitmapCreatedListener);
			// if (flag++ % 2 == 0)
			// bitmap=ScreenShotLibJni.getInstance().getScreenShot(videoName,
			// "/sdcard/test.png", 1, 150, 150);//
			// else
//			 bitmap=ScreenShotLibJni.getInstance().getIDRFrameThumbnail(videoName,
//			 "/sdcard/test.png", 0, 0);

			// time += 10;
//			 bitmap=ScreenShotLibJni.getInstance().getScreenShot(videoName,
//			 "/sdcard/test.png", time, 1280, 720);//

			if(bitmap!=null){
				imageView.setImageBitmap(null);
				bitmap.recycle();
			}

			
//			long currentTime = System.currentTimeMillis();
//			Log.e("", "getKeyFrameScreenShot---current=" + currentTime);
//			bitmap = ScreenShotLibJni.getInstance().getKeyFrameScreenShot_2(
//					videoName, "/sdcard/test.png", time, 1280, 720);

//			currentTime = System.currentTimeMillis();
//			Log.e("", "getKeyFrameScreenShot---current=" + currentTime);
//			
//			currentTime = System.currentTimeMillis();
//			Log.e("", "getIDRFrameThumbnail---current=" + currentTime);
//			bitmap = ScreenShotLibJni.getInstance().getIDRFrameThumbnail(
//					videoName, "/sdcard/test.png", 1280, 720);
//
//			currentTime = System.currentTimeMillis();
			Log.e("", "gen time = " + time);

			// videoName
			// ="/sdcard/电影/[欧美][预告][长发公主][高清RMVB][1280&times;720][中文字幕].rmvb";
			//
			for(int i = 0;i< 20; i++){
				final int index = i;
				new Thread(){
					@Override
					public void run(){
						 bitmap=ScreenShotLibJni.getInstance().getScreenShot(videoName,
								 "/sdcard/test_"+ index +".png", time, 1280, 720);//
					}
				}.start();
				time += 3;
			}
			 
//			 RoundedBitmapDrawable roundedBitmapDrawable = RoundedBitmapDrawableFactory.create(getResources(), bitmap);
//			 roundedBitmapDrawable.setCornerRadius(6); //设置圆角半径为正方形边长的一半
//			 roundedBitmapDrawable.setAntiAlias(true);
//			 imageView.setImageDrawable(roundedBitmapDrawable);
			 
//			 Bitmap tmpBitmap = getRoundedCornerBitmap(bitmap, 6);
//			 bitmap.recycle();
//			 imageView.setImageBitmap(tmpBitmap);

			// layout.addView(imageView);
			imageView.setScaleType(ScaleType.CENTER_CROP);
			imageView.setImageBitmap(bitmap);
			// Intent intent=new Intent();
			// intent.setComponent(new
			// ComponentName("com.clov4r.moboplayer.android.nil",
			// "com.clov4r.moboplayer.android.nil.MainInterface"));
			//
			// intent.setData(Uri.parse(videoName));
			// startActivity(intent);
		}
	};
	
    public Bitmap getRoundedCornerBitmap(Bitmap bitmap, float roundPx) {
		try {
			Bitmap output = Bitmap.createBitmap(bitmap.getWidth(),
					bitmap.getHeight(), Bitmap.Config.ARGB_8888);
			Canvas canvas = new Canvas(output);
			final Paint paint = new Paint();
			final Rect rect = new Rect(0, 0, bitmap.getWidth(),
					bitmap.getHeight());
			final RectF rectF = new RectF(new Rect(0, 0, bitmap.getWidth(),
					bitmap.getHeight()));
			paint.setAntiAlias(true);
			canvas.drawARGB(0, 0, 0, 0);
			paint.setColor(Color.BLACK);
			canvas.drawRoundRect(rectF, roundPx, roundPx, paint);
			paint.setXfermode(new PorterDuffXfermode(Mode.SRC_IN));
			final Rect src = new Rect(0, 0, bitmap.getWidth(),
					bitmap.getHeight());

			canvas.drawBitmap(bitmap, src, rect, paint);
			return output;
		} catch (Exception e) {
			return bitmap;
		}
	}

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
			Toast.makeText(MoboThumbnailTestActivity.this, "截图失败",
					Toast.LENGTH_SHORT).show();
		}
	};
}
