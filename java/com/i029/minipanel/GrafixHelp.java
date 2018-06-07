package com.i029.minipanel;

import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.graphics.Canvas;


import com.i029.minipanel.MicroPanelService;

/**
 * Created by lvh on 6/7/18.
 */

public class GrafixHelp {
    public static final int WIDTH = 128;
    public static final int HEIGHT = 32;
    private Bitmap bmpFrameBuffer = null;
    private Canvas mCanvas = null;

    public GrafixHelp() {
        bmpFrameBuffer = Bitmap.createBitmap(WIDTH, HEIGHT, Bitmap.Config.ARGB_8888);
        mCanvas = new Canvas(bmpFrameBuffer);
    }
    public Canvas getCanvas() {
        return mCanvas;
    }

    public void updateScreen() {
        MicroPanelService server = MicroPanelService.getInstance();
        server.DrawBitMap(0,0, bmpFrameBuffer);
        server.UpdateScreen(0,0,WIDTH -1, HEIGHT - 1);
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
