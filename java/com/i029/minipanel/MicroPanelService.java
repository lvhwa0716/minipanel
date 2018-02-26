package com.i029.minipanel;

import android.os.Bundle;
import android.util.Log;
import android.os.ServiceManager;
import android.os.IBinder;

public class MicroPanelService {
	
	static final String TAG = "MicroPanelService";

    //private IMicroPanelService microPanelService;

	public static IMicroPanelService getMicroPanelService() {
		IBinder binder = ServiceManager.getService("i029.minipanel");
		if(binder == null) {
			Log.e(TAG, "i029.minipanel not found ");
			return null;
		}
		IMicroPanelService s = IMicroPanelService.Stub.asInterface(binder);
		if( s == null) {
			Log.e(TAG, "Interface cast error ");
		}

		return s;
	}
	
	public static String[] listService() {
		try {
			return ServiceManager.listServices();
		} catch( Exception e) {
		}
		return null;
	}
	
}
