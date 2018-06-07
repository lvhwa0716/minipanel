package com.i029.minipanel;

import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.graphics.Canvas;


import com.i029.minipanel.MicroPanelService;

/**
 * Created by lvh on 6/7/18.
 */

public class GrafixHelp {
    public static final int OLED_WIDTH = 128;
    public static final int OLED_HEIGHT = 32;
    private Bitmap bmpFrameBuffer = null;
    private Canvas mCanvas = null;
    private int offset_x = 0;
    private int offset_y = 0;

    public GrafixHelp(int x, int y , int w , int h) throws Exception {

		if((x < 0 ) || (y < 0) || (x >= OLED_WIDTH) || (y >= OLED_HEIGHT)) {
			throw new Exception("0 =< x < 127  &&  0 =< y < 31");
		}

		if( ( w < 0 ) || (h < 0) ) {
			throw new Exception("w and h must gt(>) 0 ");
		}

		if(  ( (x + w) > OLED_WIDTH ) 
			|| ( (y + h) > OLED_HEIGHT ) ) {
			throw new Exception("oled size 128*32 , check parameters ");
		}

        bmpFrameBuffer = Bitmap.createBitmap(w, h, Bitmap.Config.ARGB_8888);
		offset_x = x;
		offset_y = y;

        mCanvas = new Canvas(bmpFrameBuffer);
    }
    public Canvas getCanvas() {
        return mCanvas;
    }

    public void updateScreen() {
        MicroPanelService server = MicroPanelService.getInstance();
        server.DrawBitMap(offset_x, offset_y, bmpFrameBuffer);
        server.UpdateScreen(0,0,OLED_WIDTH -1, OLED_HEIGHT - 1);
    }

    public void wakeUp() {
        MicroPanelService server = MicroPanelService.getInstance();
        server.Wakeup();
    }

    public void sleep() {
        MicroPanelService server = MicroPanelService.getInstance();
        server.Sleep(0);
    }
    public void setBrightness(int level) {
        MicroPanelService server = MicroPanelService.getInstance();
        server.Brightness(level);
    }
}
