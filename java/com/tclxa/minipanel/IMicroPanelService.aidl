package com.tclxa.minipanel;

interface IMicroPanelService {
	void Sleep(int level);
	void Wakeup();
	void Brightness(int b);
	void UpdateScreen(int x, int y, int w, int h);
	void DrawString(int x, int y, in String str);

	void DrawBitmap(int x, int y, int bmp_w, int bmp_h, int bmp_pitch , int bmp_bpp, in byte[] bmp);
	void SetColor(int color);
	void ClearAll();
	void DrawPixel(int x, int y, int color);
	int ReadPixel(int x, int y);

	void HLine(int x1, int x2, int y); // x1 < x2
	void VLine(int x, int y1, int y2); // y1 < y2
	void Line(int x1, int y1, int x2, int y2);
	void Rect(int x, int y, int w, int h);
	void FillRect(int x, int y, int w, int h);
	
	// add more here
	
}
