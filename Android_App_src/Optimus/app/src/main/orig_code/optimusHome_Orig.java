package com.optimus.rccar;

import android.content.Intent;
import android.opengl.GLSurfaceView;
import android.os.Bundle;
import android.support.v7.app.AppCompatActivity;
import android.view.View;
//import android.widget.Button;
import android.widget.RelativeLayout;
import android.widget.Toast;
import android.widget.ToggleButton;

public class optimusHome extends AppCompatActivity {

    ToggleButton Tbutton;
    ToggleButton Tbutton_siri;
    int reentry_count = 0;
    optManControl optManControl = new optManControl();
    private GLSurfaceView glView;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_optimus_home);
		//RelativeLayout l = (RelativeLayout) findViewById(R.id.MyRelativeLayout);

        glView = new GLSurfaceView(this);           // Allocate a GLSurfaceView
        glView.setRenderer(new MyGLRenderer(this)); // Use a custom renderer
        //this.setContentView(glView);                        // This activity sets to GLSurfaceView
		//l.addView(glView,0);
		//setContentView(l);
		
        Tbutton_siri = (ToggleButton)findViewById(R.id.toggleButton3);
        Tbutton_siri.setOnClickListener(new View.OnClickListener(){

            @Override
            public void onClick(View v) {
                if(Tbutton_siri.isChecked()) {
                    //TODO: Do something
                    Toast.makeText(optimusHome.this, "SIRI Toggle Button is on", Toast.LENGTH_LONG).show();
                }
                else {
                    //TODO: Do something
                    Toast.makeText(optimusHome.this, "SIRI Toggle button is Off", Toast.LENGTH_LONG).show();
                }
            }

        });

        Tbutton = (ToggleButton)findViewById(R.id.toggleButton);
        Tbutton.setOnClickListener(new View.OnClickListener(){

            @Override
            public void onClick(View v) {
                if(!Tbutton_siri.isChecked()){
                    Toast.makeText(optimusHome.this, "Siri Button is 'OFF', Lets use BT!", Toast.LENGTH_LONG).show();
                    if(Tbutton.isChecked()) {
                        Intent I = new Intent(optimusHome.this, btDeviceList.class);
                        //Intent I = new Intent(optimusHome.this, optManControl.class);
                        startActivity(I);
                        Toast.makeText(optimusHome.this, "Toggle Button is on", Toast.LENGTH_LONG).show();
                    }
                    else {
                        Toast.makeText(optimusHome.this, "Toggle button is Off", Toast.LENGTH_LONG).show();
                    }
                }
                else{
                    Toast.makeText(optimusHome.this, "Siri Button is 'ON', Please turn it 'OFF' to use this option!!!", Toast.LENGTH_LONG).show();
                }
            }

        });

        //btn = (Button)findViewById(R.id.button5);
        //btn.setOnClickListener(new View.OnClickListener() {
            //@Override
            //public void onClick(View v) {

                //if(Tbutton.isChecked())
                //{
                    //Intent I = new Intent(OptimusHome.this, deviceList.class);
                    //I.putExtras();
                    //startActivity(I);
                    //Toast.makeText(OptimusHome.this, "Button is on", Toast.LENGTH_LONG).show();
                    //Tbutton.setChecked(false);
                //}
                //else {
                  //  Toast.makeText(Optimus_home.this, "Toggle button is Off", Toast.LENGTH_LONG).show();
                //}
          //  }
        //});
    }

    @Override
    protected void onResume() {
        reentry_count +=1;
        glView.onResume();
        if(reentry_count > 1){
            Tbutton.setChecked(false);
            Tbutton_siri.setChecked(false);
            Toast.makeText(optimusHome.this, "Toggle and Toggle_SIRI buttons are being set to Off now...", Toast.LENGTH_LONG).show();
        }

        super.onResume();
    }

    @Override
    protected void onDestroy() {
        optManControl.CloseBTConnection();
        Toast.makeText(optimusHome.this, "Closing BT Connection when main Activity is destroyed!!!", Toast.LENGTH_LONG).show();

        super.onDestroy();
    }

    // Call back when the activity is going into the background
    @Override
    protected void onPause() {
        super.onPause();
        glView.onPause();
    }
}
