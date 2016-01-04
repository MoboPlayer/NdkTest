package com.clov4r.moboplayer.android.nil.codec.activity;

import java.util.List;

import android.app.Activity;
import android.content.ComponentName;
import android.content.Intent;
import android.content.pm.PackageInfo;
import android.content.pm.PackageManager;
import android.net.Uri;
import android.util.Log;

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
public class OpenBroswer {
	static final String uc_pkg = "com.UCMobile";// com.uc.browser.UCMobileApp
	static final String uc_class_name = "com.UCMobile.main.UCMobile";
	static final String qihu_360_pkg = "com.qihoo.browser";// .component.BrowserApplicationContext
	static final String baidu_pkg = "com.baidu.browser.apps";// BdApplication
	static final String hai_tun_pkg = "mobi.mgeek.TunnyBrowser.Browser";
	static final String chrome_pkg = "com.google.android.apps.chrome.";// ChromeMobileApplication
	static final String qq_pkg = "com.tencent.android.qqdownloader";// com.tencent.mtt.browser.setting.managespace.ManageSpaceActivity

	public static void openUrlByBroswer(Activity act, String url) {
		checkApp(act, url);
	}

	private static int startUC(Activity act, String url) {
		try {
			startApp(uc_pkg, uc_class_name, act, url);
		} catch (Exception e) {
			return 0;
		}
		return 1;
	}

	private static void startApp(String pkg_name, String class_name,
			Activity act, String url) throws Exception {
		Intent intent = new Intent(Intent.ACTION_VIEW);
		intent.setData(Uri.parse(url));
		if (pkg_name != null && class_name != null) {
			ComponentName component = new ComponentName(pkg_name, class_name);
			intent.setComponent(component);
		}
		act.startActivity(intent);

	}

	private static void startApp(Intent intent, Activity act, String url) {
		try {
			intent.setData(Uri.parse(url));
			act.startActivity(intent);
		} catch (Exception e) {
			e.printStackTrace();
		}
	}

	private static void checkApp(Activity act, String url) {
		List<PackageInfo> list = act.getPackageManager().getInstalledPackages(
				PackageManager.GET_PERMISSIONS);
		for (PackageInfo packageInfo : list) {
			String pkgName = packageInfo.packageName;
			Log.e("OpenBroswer", "pkgName is " + pkgName);
			if (pkgName.contains(uc_pkg) || pkgName.contains(qihu_360_pkg)
					|| pkgName.contains(baidu_pkg)
					|| pkgName.contains(hai_tun_pkg)
					|| pkgName.contains(chrome_pkg) || pkgName.contains(qq_pkg)) {
				Intent intent = act.getPackageManager()
						.getLaunchIntentForPackage(pkgName);
				intent.setAction(Intent.ACTION_VIEW);
				startApp(intent, act, url);
				return;
			} else if (pkgName.contains("com.android.browser")) {
				Intent intent = act.getPackageManager()
						.getLaunchIntentForPackage(pkgName);
				intent.setAction(Intent.ACTION_VIEW);
				startApp(intent, act, url);
				return;
			}
		}
		Intent intent = new Intent(Intent.ACTION_VIEW);
		intent.setAction(Intent.ACTION_VIEW);
		intent.setData(Uri.parse(url));
		act.startActivity(intent);
	}
}
