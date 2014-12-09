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
		HelloJni h = new HelloJni();
		TextView  tv = (TextView) this.findViewById(R.id.hello);
        
        String libpath = "/data/data/com.example.hellojni/lib/";
        String libname = "libffmpeg_armv7_neon.so";
        String filePath =  Environment.getExternalStorageDirectory()+"/Godzilla.srt";
        Log.e("params", libpath+" "+filePath);
        tv.append( h.stringFromJNI(libpath,libname,filePath,0,47) );
        Toast.makeText(this, tv.getText().toString(), Toast.LENGTH_LONG).show();
				
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
