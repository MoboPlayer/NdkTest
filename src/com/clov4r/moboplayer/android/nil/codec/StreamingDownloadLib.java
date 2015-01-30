package com.clov4r.moboplayer.android.nil.codec;

import java.util.HashMap;

import com.clov4r.moboplayer.android.nil.library.DataSaveLib;

import android.content.Context;
import android.os.AsyncTask;
import android.util.SparseArray;

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
public class StreamingDownloadLib {
	private static StreamingDownloadLib mStreamingDownloadLib = null;
	private HashMap<Integer, StreamingDownloadData> dataMap = null;
	private Context mContext = null;
	private DataSaveLib mDataSaveLib = null;
	private Object lockObj = new Object();

	public StreamingDownloadLib(Context con) {
		mContext = con;
		mDataSaveLib = new DataSaveLib(con,
				DataSaveLib.name_of_streaming_download_info, true);
		dataMap = (HashMap) mDataSaveLib.readData();
		if (dataMap == null)
			dataMap = new HashMap<Integer, StreamingDownloadData>();
	}

	public static StreamingDownloadLib getInstance(Context con) {
		if (mStreamingDownloadLib == null)
			mStreamingDownloadLib = new StreamingDownloadLib(con);
		return mStreamingDownloadLib;
	}

	MoboDownloadListener mMoboDownloadListener = null;

	public void setDownloadListener(MoboDownloadListener listener) {
		mMoboDownloadListener = listener;
	}

	public int startDownload(String streamingUrl, String fileSavePath) {
		int key = getKeyOf(streamingUrl, fileSavePath);
		StreamingDownloadData downloadData = null;
		if (dataMap.containsKey(key)) {
			downloadData = dataMap.get(key);
		} else {
			downloadData = new StreamingDownloadData();
			downloadData.streamingUrl = streamingUrl;
			downloadData.fileSavePath = fileSavePath;
			downloadData.id = key;
		}

		new DownloadLib(downloadData).execute((Void) null);
		return key;
	}

	public void pauseDownload(int downloadId) {
		synchronized (lockObj) {
			nativePauseDownload(downloadId);
		}
	}

	public void resumeDownload(int downloadId) {
		synchronized (lockObj) {
			nativeResumeDownload(downloadId);
		}
	}

	public void stopDownload(int downloadId) {
		synchronized (lockObj) {
			nativeStopDownload(downloadId);
		}
	}

	public void onDownloadProgressChanged(int id, int streamIndex, long pts,
			int currentTime) {
		synchronized (lockObj) {
			if (dataMap.containsKey(id)) {
				StreamingDownloadData downloadData = dataMap.get(id);
				// downloadData.finishSize = size;
				downloadData.stm_index_pts_map.put(streamIndex, pts);
				downloadData.currentTime = currentTime;
				mMoboDownloadListener.onDownloadProgressChanged(downloadData,
						currentTime);
			}
		}
	}

	public void onDownloadFinished(int id) {
		synchronized (lockObj) {
			if (dataMap.containsKey(id)) {
				StreamingDownloadData downloadData = dataMap.get(id);
				downloadData.isFinished = true;
				mMoboDownloadListener.onDownloadFinished(downloadData);
			}
		}
	}

	public void onDownloadFailed(int id) {
		synchronized (lockObj) {
			if (dataMap.containsKey(id)) {
				StreamingDownloadData downloadData = dataMap.get(id);
				downloadData.isDownloadFailed = true;
				mMoboDownloadListener.onDownloadFailed(downloadData);
			}
		}
	}

	public int getCurrentTime(int id) {
		if (dataMap.containsKey(id)) {
			StreamingDownloadData downloadData = dataMap.get(id);
			return downloadData.currentTime;
		}
		return 0;
	}

	public int getDuration(int id) {
		return nativeGetDuration(id);
	}

	private int getKeyOf(String url, String path) {
		return (url + path).hashCode();
	}

	public native void nativeStartDownload(String streamingUrl,
			String fileSavePath, int id, Long[] ptsArray);

	public native void nativePauseDownload(int downloadId);

	public native void nativeResumeDownload(int downloadId);

	public native void nativeStopDownload(int downloadId);

	public native int nativeGetDuration(int downloadId);

	private class DownloadLib extends AsyncTask<Void, Integer, Void> {
		StreamingDownloadData mStreamingDownloadData = null;

		public DownloadLib(StreamingDownloadData data) {
			mStreamingDownloadData = data;
		}

		@Override
		protected Void doInBackground(Void... params) {
			// TODO Auto-generated method stub
			nativeStartDownload(mStreamingDownloadData.streamingUrl,
					mStreamingDownloadData.fileSavePath,
					mStreamingDownloadData.id,
					mStreamingDownloadData.getPtsArray());
			return null;
		}

		@Override
		protected void onProgressUpdate(Integer... progresses) {

		}

		@Override
		protected void onPostExecute(Void params) {

		}
	}

	public class StreamingDownloadData {
		public int id;
		public String streamingUrl;
		public String fileSavePath;
		public int progress;
		public long finishSize;
		/** 当前下载到的时间 **/
		public int currentTime;
		public boolean isFinished;
		public boolean isDownloadFailed;
		/** 视频中每个stream与pts的对应关系 **/
		SparseArray<Long> stm_index_pts_map = new SparseArray<Long>();

		Long[] getPtsArray() {
			if (stm_index_pts_map.size() == 0)
				return null;
			else {
				Long[] resArray = new Long[stm_index_pts_map.size()];
				for (int i = 0; i < stm_index_pts_map.size(); i++)
					resArray[i] = stm_index_pts_map.get(i);
				return resArray;
			}
		}
	}

	public interface MoboDownloadListener {
		public void onDownloadProgressChanged(StreamingDownloadData data,
				int currentTime);

		public void onDownloadFinished(StreamingDownloadData data);

		public void onDownloadFailed(StreamingDownloadData data);
	}

}
