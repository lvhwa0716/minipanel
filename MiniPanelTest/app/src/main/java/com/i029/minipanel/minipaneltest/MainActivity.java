package com.i029.minipanel.minipaneltest;

import android.app.Activity;
import android.os.Bundle;
import android.os.RemoteException;
import android.util.Log;
import android.view.View;
import android.widget.Button;
import android.widget.EditText;
import android.widget.Toast;

import com.i029.minipanel.IMicroPanelService;
import com.i029.minipanel.MicroPanelService;

public class MainActivity extends Activity implements View.OnClickListener {
    static final String TAG = "MiniPanelTest";
    static final int left = 0;
    static final int top = 0;
    static final int width = 127;
    static final int height = 31;

    private IMicroPanelService microPanelService;
    private EditText p1;
    private EditText p2;
    private EditText p3;
    private EditText p4;
    private EditText Result;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        p1 = (EditText)findViewById(R.id.editTextP1);
        p2 = (EditText)findViewById(R.id.editTextP2);
        p3 = (EditText)findViewById(R.id.editTextP3);
        p4 = (EditText)findViewById(R.id.editTextP4);
        Result = p4;

        Button btn= (Button)findViewById(R.id.buttonSleep);
        btn.setOnClickListener(this);
        btn= (Button)findViewById(R.id.buttonClearAll);
        btn.setOnClickListener(this);
        btn= (Button)findViewById(R.id.buttonDrawPixel);
        btn.setOnClickListener(this);
        btn= (Button)findViewById(R.id.buttonFillRect);
        btn.setOnClickListener(this);
        btn= (Button)findViewById(R.id.buttonHLine);
        btn.setOnClickListener(this);
        btn= (Button)findViewById(R.id.buttonDrawString);
        btn.setOnClickListener(this);
        btn= (Button)findViewById(R.id.buttonLine);
        btn.setOnClickListener(this);
        btn= (Button)findViewById(R.id.buttonReadPixel);
        btn.setOnClickListener(this);
        btn= (Button)findViewById(R.id.buttonRect);
        btn.setOnClickListener(this);
        btn= (Button)findViewById(R.id.buttonSetColorBlack);
        btn.setOnClickListener(this);
        btn= (Button)findViewById(R.id.buttonSetColorWhite);
        btn.setOnClickListener(this);
        btn= (Button)findViewById(R.id.buttonVLine);
        btn.setOnClickListener(this);
        btn= (Button)findViewById(R.id.buttonWakeup);
        btn.setOnClickListener(this);
        btn= (Button)findViewById(R.id.buttonSetBrightness);
        btn.setOnClickListener(this);



        String[] services = MicroPanelService.listService();
        if(services != null) {
            for(String s : services) {
                Log.e(TAG, s);
            }
        }
        microPanelService = MicroPanelService.getMicroPanelService();

        if (microPanelService == null) {
            Log.e(TAG, "MicroPanelService service is not initialized!");
            return ;
        }
    }

    @Override
    public void onClick(View arg0)
    {
        if(microPanelService == null) {
            Toast.makeText(this, "MicroPanelServer not found", Toast.LENGTH_LONG).show();
            return ;
        }
        try {
            switch (arg0.getId())
            {
                case R.id.buttonSleep:
                    microPanelService.Sleep(Integer.parseInt(p1.getText().toString()));
                    break;
                case R.id.buttonClearAll:
                    microPanelService.Brightness(Integer.parseInt(p1.getText().toString()));
                    break;

                case R.id.buttonDrawPixel:
                    microPanelService.DrawPixel(
                            Integer.parseInt(p1.getText().toString()),
                            Integer.parseInt(p2.getText().toString()),
                            Integer.parseInt(p3.getText().toString())
                    );
                    break;

                case R.id.buttonFillRect:
                    microPanelService.FillRect(
                            Integer.parseInt(p1.getText().toString()),
                            Integer.parseInt(p2.getText().toString()),
                            Integer.parseInt(p3.getText().toString()),
                            Integer.parseInt(p4.getText().toString())
                    );
                    break;

                case R.id.buttonHLine:
                    microPanelService.HLine(
                            Integer.parseInt(p1.getText().toString()),
                            Integer.parseInt(p2.getText().toString()),
                            Integer.parseInt(p3.getText().toString())
                    );
                    break;

                case R.id.buttonDrawString:
                    microPanelService.DrawString(
                            Integer.parseInt(p1.getText().toString()),
                            Integer.parseInt(p2.getText().toString()),
                            p3.getText().toString()
                    );
                    break;

                case R.id.buttonLine:
                    microPanelService.Line(
                            Integer.parseInt(p1.getText().toString()),
                            Integer.parseInt(p2.getText().toString()),
                            Integer.parseInt(p3.getText().toString()),
                            Integer.parseInt(p4.getText().toString())
                    );
                    break;

                case R.id.buttonReadPixel:
                    int c = microPanelService.ReadPixel(
                            Integer.parseInt(p1.getText().toString()),
                            Integer.parseInt(p2.getText().toString())
                    );
                    Result.setText( Integer.toString(c));
                    break;

                case R.id.buttonRect:
                    microPanelService.Rect(
                            Integer.parseInt(p1.getText().toString()),
                            Integer.parseInt(p2.getText().toString()),
                            Integer.parseInt(p3.getText().toString()),
                            Integer.parseInt(p4.getText().toString())
                    );
                    break;

                case R.id.buttonSetColorBlack:
                    microPanelService.SetColor(0);
                    break;

                case R.id.buttonSetColorWhite:
                    microPanelService.SetColor(1);
                    break;

                case R.id.buttonVLine:
                    microPanelService.VLine(
                            Integer.parseInt(p1.getText().toString()),
                            Integer.parseInt(p2.getText().toString()),
                            Integer.parseInt(p3.getText().toString())
                    );
                    break;

                case R.id.buttonWakeup:
                    microPanelService.Wakeup();
                    break;

                case R.id.buttonSetBrightness:
                    microPanelService.Brightness(Integer.parseInt(p1.getText().toString()));
                    break;
                default:
                    Toast.makeText(this, "ID=" + arg0.getId() + "not done", Toast.LENGTH_LONG).show();
                    break;
            }
            microPanelService.UpdateScreen(left, top, width, height);
        } catch (Exception e) {
            Toast.makeText(this, "Some Error Raised see log", Toast.LENGTH_LONG).show();
            e.printStackTrace();
        }
    }
}
