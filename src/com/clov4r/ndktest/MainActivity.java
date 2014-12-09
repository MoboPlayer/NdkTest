package com.clov4r.ndktest;

import android.support.v7.app.ActionBarActivity;
import android.os.Bundle;
import android.os.Environment;
import android.util.Log;
import android.view.Menu;
import android.view.MenuItem;
import android.widget.TextView;
import android.widget.Toast;

public class MainActivity extends ActionBarActivity {

	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_main);
		SubtitleJni h = new SubtitleJni();
		TextView  tv = (TextView) this.findViewById(R.id.hello);
        this.getFilesDir().getParent();
        String libpath = getFilesDir().getParent()+"/lib/";
        String libname = "libffmpeg_armv7_neon.so";
        String filePath =  Environment.getExternalStorageDirectory()+"/Godzilla.srt";
        Log.e("params", libpath+" "+filePath);
        StringBuffer sb = new StringBuffer();
        String temp = h.openFFmpegAndSubtitleFileInJNI(libpath,libname,filePath,0);
        sb.append("open subtitle file :"+temp+"\n");
        for(int i=0;i<100;i++){
        	String str = h.getSubtitleByTime(1000*i);
        	if(str!=null){

        		Log.e("subtitle", str==null?" null ": str);
        		sb.append("time:"+i+"s subtitle:"+str+"\n");
        	}
        }
       
        tv.setText( sb.toString());
//        Toast.makeText(this, tv.getText().toString(), Toast.LENGTH_LONG).show();
				
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
			return true;
		}
		return super.onOptionsItemSelected(item);
	}
}
