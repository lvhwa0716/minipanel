package com.i029.minipanel;

import android.os.Bundle;
import android.util.Log;
import android.os.ServiceManager;
import android.os.IBinder;
import android.os.RemoteException;
import android.graphics.Bitmap;

public class MicroPanelService {
	
	static final String TAG = "MicroPanelService";

	static private MicroPanelService _instance;

	private IMicroPanelService getIMicroPanelService() {
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
	private MicroPanelService() {
	}

	public static synchronized MicroPanelService getInstance() {
		if( _instance== null )
			_instance = new MicroPanelService();
		return _instance;
	}

	public static String[] listService() {
		try {
			return ServiceManager.listServices();
		} catch( Exception e) {
		}
		return null;
	}

	public void Sleep(int level) {
		IMicroPanelService microPanelService = getIMicroPanelService();
		if( microPanelService == null) {
			Log.e(TAG, "Native Service not found Sleep ");
			return ;
		}
		try {
			microPanelService.Sleep( level);
		} catch(RemoteException e) {
		}
	}
	public void Wakeup() {
		IMicroPanelService microPanelService = getIMicroPanelService();
		if( microPanelService == null) {
			Log.e(TAG, "Native Service not found Wakeup ");
			return ;
		}
		try {
			microPanelService.Wakeup();
		} catch(RemoteException e) {
		}
	}
	public void Brightness(int b) {
		IMicroPanelService microPanelService = getIMicroPanelService();
		if( microPanelService == null) {
			Log.e(TAG, "Native Service not found Brightness ");
			return ;
		}
		try {
			microPanelService.Brightness( b);
		} catch(RemoteException e) {
		}
	}
	public void UpdateScreen(int x, int y, int w, int h) {
		IMicroPanelService microPanelService = getIMicroPanelService();
		if( microPanelService == null) {
			Log.e(TAG, "Native Service not found UpdateScreen ");
			return ;
		}
		try {
			microPanelService.UpdateScreen( x,  y,  w,  h);
		} catch(RemoteException e) {
		}
	}
	public void DrawString(int x, int y, java.lang.String str) {
		IMicroPanelService microPanelService = getIMicroPanelService();
		if( microPanelService == null) {
			Log.e(TAG, "Native Service not found DrawString ");
			return ;
		}
		try {
			microPanelService.DrawString( x,  y, str);
		} catch(RemoteException e) {
		}
	}
	public void DrawBitmap(int x, int y, int bmp_w, int bmp_h, int bmp_pitch, int bmp_bpp, byte[] bmp) {
		IMicroPanelService microPanelService = getIMicroPanelService();
		if( microPanelService == null) {
			Log.e(TAG, "Native Service not found DrawBitmap ");
			return ;
		}
		try {
			microPanelService.DrawBitmap(x, y, bmp_w, bmp_h, bmp_pitch, bmp_bpp,  bmp);
		} catch(RemoteException e) {
		}
	}
	public void SetColor(int color) {
		IMicroPanelService microPanelService = getIMicroPanelService();
		if( microPanelService == null) {
			Log.e(TAG, "Native Service not found SetColor ");
			return ;
		}
		try {
			microPanelService.SetColor( color);
		} catch(RemoteException e) {
		}
	}
	public void ClearAll() {
		IMicroPanelService microPanelService = getIMicroPanelService();
		if( microPanelService == null) {
			Log.e(TAG, "Native Service not found ClearAll ");
			return ;
		}
		try {
			microPanelService.ClearAll();
		} catch(RemoteException e) {
		}
	}
	public void DrawPixel(int x, int y, int color) {
		IMicroPanelService microPanelService = getIMicroPanelService();
		if( microPanelService == null) {
			Log.e(TAG, "Native Service not found DrawPixel ");
			return ;
		}
		try {
			microPanelService.DrawPixel(x, y, color);
		} catch(RemoteException e) {
		}
	}
	public int ReadPixel(int x, int y) {
		IMicroPanelService microPanelService = getIMicroPanelService();
		if( microPanelService == null) {
			Log.e(TAG, "Native Service not found ReadPixel ");
			return 1;
		}
		try {
			return microPanelService.ReadPixel(x, y);
		} catch(RemoteException e) {
		}
		return 1;
	}
	public void HLine(int x1, int x2, int y) { // x1 < x2
		IMicroPanelService microPanelService = getIMicroPanelService();
		if( microPanelService == null) {
			Log.e(TAG, "Native Service not found HLine ");
			return ;
		}
		try {
			microPanelService.HLine( x1, x2, y);
		} catch(RemoteException e) {
		}
	}
	

	public void VLine(int x, int y1, int y2) { // y1 < y2
		IMicroPanelService microPanelService = getIMicroPanelService();
		if( microPanelService == null) {
			Log.e(TAG, "Native Service not found VLine ");
			return ;
		}
		try {
			microPanelService.VLine( x, y1, y2);
		} catch(RemoteException e) {
		}
	}
	

	public void Line(int x1, int y1, int x2, int y2) {
		IMicroPanelService microPanelService = getIMicroPanelService();
		if( microPanelService == null) {
			Log.e(TAG, "Native Service not found Line ");
			return ;
		}
		try {
			microPanelService.Line( x1,  y1, x2, y2);
		} catch(RemoteException e) {
		}
	}
	public void Rect(int x, int y, int w, int h) {
		IMicroPanelService microPanelService = getIMicroPanelService();
		if( microPanelService == null) {
			Log.e(TAG, "Native Service not found Rect ");
			return ;
		}
		try {
			microPanelService.Rect(x, y, w, h);
		} catch(RemoteException e) {
		}
	}
	public void FillRect(int x, int y, int w, int h) {
		IMicroPanelService microPanelService = getIMicroPanelService();
		if( microPanelService == null) {
			Log.e(TAG, "Native Service not found FillRect ");
			return ;
		}
		try {
			microPanelService.FillRect(x, y, w, h);
		} catch(RemoteException e) {
		}
	}
	public void FontSize(int w, int h) {
		IMicroPanelService microPanelService = getIMicroPanelService();
		if( microPanelService == null) {
			Log.e(TAG, "Native Service not found FontSize ");
			return ;
		}
		try {
			microPanelService.FontSize( w,  h);
		} catch(RemoteException e) {
		}
	}
	public int SetFont(int lang, String fontFile) {
		IMicroPanelService microPanelService = getIMicroPanelService();
		if( microPanelService == null) {
			Log.e(TAG, "Native Service not found FontSize ");
			return -200;
		}
		try {
			return microPanelService.FontFile( lang, fontFile);
		} catch(RemoteException e) {
		}
		return -100;
	}

	/* must not scale
		{
                BitmapFactory.Options opt = new BitmapFactory.Options();
                opt.inPreferredConfig = Bitmap.Config.ARGB_8888;
                opt.inDensity = 0;
                opt.inTargetDensity = 0;
                opt.inScreenDensity = 0;
                opt.inScaled = false;
                Bitmap bmp = BitmapFactory.decodeResource(getResources(), R.drawable.logo,opt);

                DrawBitMap(Integer.parseInt(p1.getText().toString()), // dest x
                        Integer.parseInt(p1.getText().toString()), // dest y
                        bmp);
                bmp.recycle();
		}
	*/
	public void DrawBitMap(int x , int y, Bitmap img) {
        int width, height;
        height = img.getHeight();
        width = img.getWidth();

        int []pixels = new int[width * height];
        byte []target_pixels = new byte[width * height];

        img.getPixels(pixels, 0, width, 0, 0, width, height);

        for(int i = 0; i < height; i++)  {
            for(int j = 0; j < width; j++) {
                int grey = pixels[width * i + j];

                int red = ((grey  & 0x00FF0000 ) >> 16);
                int green = ((grey & 0x0000FF00) >> 8);
                int blue = (grey & 0x000000FF);

                grey = (int)((float) red * 0.3 + (float)green * 0.59 + (float)blue * 0.11);

                target_pixels[width * i + j] = (byte)grey;
            }
        }

		IMicroPanelService microPanelService = getIMicroPanelService();
		if( microPanelService == null) {
			Log.e(TAG, "Native Service not found DrawBitmap ");
			return ;
		}
		try {
			microPanelService.DrawBitmap(
                x,
                y,
                width,
                height,
                width,
                8,
                target_pixels
        	);
		} catch(RemoteException e) {
		}

        
    }	
}
