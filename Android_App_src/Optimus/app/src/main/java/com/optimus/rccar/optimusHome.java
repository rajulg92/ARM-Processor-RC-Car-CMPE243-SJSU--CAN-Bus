package com.optimus.rccar;

import android.app.Activity;
import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothSocket;
import android.content.Intent;
import android.os.Bundle;
import android.media.MediaPlayer;
//import android.support.v7.app.AppCompatActivity;
import android.view.View;
import android.widget.Button;
import android.widget.Toast;
import android.widget.ToggleButton;
import static processing.core.PApplet.println;


public class optimusHome extends Activity {

    public static final int BT_DISCONNECTED = 0;
    public static final int BT_START = 1;
    public static final int BT_CONNECTING = 2;
    public static final int BT_CONNECTED = 3;

    private BluetoothSocket btsocket_bkp = null;    //TODO: Issues in revoking socket connection APP crash
    static ToggleButton Tbutton_power;
    static int curr_bt_btn_state;
    static Button btn_bt, btn_nav, btn_dash;
    static View layout_view;
    static boolean ismanualbtdisconnect = false;
    //int reentry_count = 0;
    optManControl optManControl = new optManControl();

    /*---------------------------------------*/
     //BLUETOOTH Auto Connect VARIABLES
    private BluetoothAdapter myBluetooth = null;
    public static String EXTRA_ADDRESS = "device_address";
    //String address = "00:18:91:D9:14:24"; //OUR BLE CHIP
    String address = "00:14:03:06:03:85";   //RAJ BLE CHIP
    /*---------------------------------------*/

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_optimus_home);

        /*---------------------------------------*/
        //if the device has bluetooth
        myBluetooth = BluetoothAdapter.getDefaultAdapter();

        if(myBluetooth == null)
        {
            //Show a mensag. that the device has no bluetooth adapter
            Toast.makeText(getApplicationContext(), "Bluetooth Device Not Available", Toast.LENGTH_LONG).show();

            //finish apk
            finish();
        }
        else if(!myBluetooth.isEnabled())
        {
            //Ask to the user turn the bluetooth on
            Intent turnBTon = new Intent(BluetoothAdapter.ACTION_REQUEST_ENABLE);
            startActivityForResult(turnBTon,1);
        }
        /*---------------------------------------*/

        optimusHome prevActivity = (optimusHome)getLastNonConfigurationInstance();
        if(prevActivity!= null) {
            // So the orientation did change
            // Restore some field for example
            this.btsocket_bkp = prevActivity.btsocket_bkp;
        }

        //call the widgets
        btn_bt = (Button) findViewById(R.id.bt_btn);
        btn_nav = (Button) findViewById(R.id.nav_btn);
        btn_dash = (Button) findViewById(R.id.dash_btn);
        layout_view = (View)findViewById(R.id.MyRelativeLayout);
        final MediaPlayer mp = MediaPlayer.create(this, R.raw.vroom);
        //final MediaPlayer mp = MediaPlayer.create(this, R.raw.vroom2);

        /*---------------------------------------*/
        //AUTO-CONNECT BT
        // Make an intent to start next activity.
        Intent i = new Intent(optimusHome.this, optManControl.class);
        SetBTiconState(BT_START);
        i.putExtra(EXTRA_ADDRESS, address); //this will be received at optManControl (class) Activity
        startActivity(i);
         /*---------------------------------------*/

        // Set the content to appear under the system bars so that the content
        // doesn't resize when the system bars hide and show.
        layout_view.setSystemUiVisibility(
                View.SYSTEM_UI_FLAG_LAYOUT_STABLE
                | View.SYSTEM_UI_FLAG_LAYOUT_HIDE_NAVIGATION
                | View.SYSTEM_UI_FLAG_LAYOUT_FULLSCREEN
                | View.SYSTEM_UI_FLAG_HIDE_NAVIGATION // hide nav bar
                | View.SYSTEM_UI_FLAG_FULLSCREEN // hide status bar
                | View.SYSTEM_UI_FLAG_IMMERSIVE);


        btn_bt.setOnClickListener(new View.OnClickListener(){
            @Override
            public void onClick(View v) {
                if(curr_bt_btn_state == BT_DISCONNECTED){
                    SetBTiconState(BT_START);
                    //Intent I = new Intent(optimusHome.this, btDeviceList.class);
                    Intent I = new Intent(optimusHome.this, optManControl.class);
                    I.putExtra(EXTRA_ADDRESS, address);
                    startActivity(I);
                    //Toast.makeText(optimusHome.this, "BT Conn. Started", Toast.LENGTH_LONG).show();
                }
                else if(curr_bt_btn_state == BT_CONNECTED){
                    ismanualbtdisconnect = true;
                    //optManControl.CloseBTConnection(btsocket_bkp);
                    btdisconnected();
                    //SetBTiconState(BT_DISCONNECTED);
                    //Toast.makeText(optimusHome.this, "BT Conn. Stopped", Toast.LENGTH_LONG).show();
                }
                else{
                    //Do Nothing
                    Toast.makeText(optimusHome.this, "!Wrong BT State!", Toast.LENGTH_LONG).show();
                }
            }
        });

        btn_nav.setOnClickListener(new View.OnClickListener(){
            @Override
            public void onClick(View v) {
                if (optManControl.isBtConnected) {
                    //Switching to Navigation Activity.
                    Intent I = new Intent(optimusHome.this, optimusNavActivity.class);
                    startActivity(I);
                }else {
                    Toast.makeText(optimusHome.this, "Can't switch to NAV \nBT Not Connected!!! \nPlease pair your device FIRST!", Toast.LENGTH_LONG).show();
                }
            }
        });

        //Call the Toggle Button Widget
        Tbutton_power = (ToggleButton) findViewById(R.id.pwr_btn);
        Tbutton_power.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                //DONE: Move this Power button logic to NAVIGATION PAGE and enable the check of "isNavChckPtSet"
                if(optimusNavActivity.isNavChckPtSet) {
                    if (Tbutton_power.isChecked()) {
                        //Play VROOM Sound
                        mp.start();
                        optManControl.turnOnOptimus();      //method to send TRANSMISSION-ON Message to RC CAR
                        Toast.makeText(optimusHome.this, "Toggle Power Button is on", Toast.LENGTH_LONG).show();
                    } else /*if (!Tbutton_power.isChecked())*/ {
                        optManControl.turnOffOptimus();   //method to send TRANSMISSION-OFF Message to RC CAR
                        Toast.makeText(optimusHome.this, "Toggle Power button is Off", Toast.LENGTH_LONG).show();
                    }
                }else {
                    Toast.makeText(optimusHome.this, "Can't power RC CAR \nNAV Points Not Set!!! \nPlease Set NAV points FIRST!", Toast.LENGTH_LONG).show();
                }
            }
        });

        btn_dash.setOnClickListener(new View.OnClickListener(){
            @Override
            public void onClick(View v) {
                //Switching to Dashboard Activity.
                Intent I = new Intent(optimusHome.this, optimusDashboard.class);
                startActivity(I);
            }
        });
    }

    /*@Override
    protected void onResume() {
        *//*reentry_count +=1;
        if(reentry_count > 1){
            optManControl.CloseBTConnection();
            SetBTiconState(BT_DISCONNECTED);
            Toast.makeText(optimusHome.this, "BT button is being set to Off now...", Toast.LENGTH_LONG).show();
        }*//*
        layout_view.setSystemUiVisibility(
                View.SYSTEM_UI_FLAG_LAYOUT_STABLE
                        | View.SYSTEM_UI_FLAG_LAYOUT_HIDE_NAVIGATION
                        | View.SYSTEM_UI_FLAG_LAYOUT_FULLSCREEN
                        | View.SYSTEM_UI_FLAG_HIDE_NAVIGATION // hide nav bar
                        | View.SYSTEM_UI_FLAG_FULLSCREEN // hide status bar
                        | View.SYSTEM_UI_FLAG_IMMERSIVE);
        super.onResume();
    }*/

    @Override
    protected void onPause() {
        btsocket_bkp = DirectionFinder.getbtsocket();
        if(btsocket_bkp != null){
            println("OnPause()!!!");
        }
        super.onPause();
    }

    @Override
    protected void onStop() {
        btsocket_bkp = DirectionFinder.getbtsocket();
        if(btsocket_bkp != null){
            println("OnStop()!!!");
        }
        super.onStop();
    }

    @Override
    public Object onRetainNonConfigurationInstance() {
        btsocket_bkp = DirectionFinder.getbtsocket();
        if (btsocket_bkp != null) { // Check that the object exists
            println("onRetainNonConfigurationInstance()!!!");
            return (btsocket_bkp);
        }
        return super.onRetainNonConfigurationInstance();
    }

    @Override
    protected void onDestroy() {
        /*optManControl.CloseBTConnection(btsocket_bkp);
        SetBTiconState(BT_DISCONNECTED);*/
        btdisconnected();
        //optManControl optManControl = new optManControl();
        //optManControl.unregisterBTReceiver();
        println("Closing BT Connection when main Activity is destroyed!!!");
        super.onDestroy();
    }

    public static void btconnecting(){
        println("Icon Connecting!!!");
        SetBTiconState(BT_CONNECTING);
    }

    public static void btconnected(){
        println("Icon Connected!!!");
        SetBTiconState(BT_CONNECTED);
    }

    public static void btdisconnected(){
        println("Icon Disconnected!!!");
        SetBTiconState(BT_DISCONNECTED);
        optimusHome optimusHome = new optimusHome();
        if(!ismanualbtdisconnect) {
            //Broadcasting BT Disconnect to all activities
            //Disconnect NAV
            optimusNavActivity optimusNavActivity = new optimusNavActivity();
            optimusNavActivity.nav_btdisconnect();
            //Disconnect DASHBOARD
            optimusDashboard optimusDashboard = new optimusDashboard();
            optimusDashboard.dashboard_btdisconnect();
            //Reconnection atempted
            optimusHome.reconnectbt(true);
        }
        else
            println("Icon Disconnected manually!!!");
            optimusHome.reconnectbt(false);
            ismanualbtdisconnect = false;
    }

    public static void SetBTiconState(int bt_btn_state){
        curr_bt_btn_state = bt_btn_state;
        switch (bt_btn_state) {
            case BT_START:
                btn_bt.setBackgroundResource(R.drawable.bton);
                break;
            case BT_CONNECTING:
                btn_bt.setBackgroundResource(R.drawable.btconng);
                break;
            case BT_CONNECTED:
                btn_bt.setBackgroundResource(R.drawable.btconcd);
                break;
            case BT_DISCONNECTED:
                btn_bt.setBackgroundResource(R.drawable.btoff);
                break;
        }
    }

    private void reconnectbt(boolean flag){
        if(flag) {
            optManControl.CloseBTConnection(btsocket_bkp);
            if (!optManControl.isBtConnected && !ismanualbtdisconnect) {
                //if the device has bluetooth
                myBluetooth = BluetoothAdapter.getDefaultAdapter();

                if (myBluetooth == null) {
                    //Show a mensag. that the device has no bluetooth adapter
                    Toast.makeText(getApplicationContext(), "Bluetooth Device Not Available", Toast.LENGTH_LONG).show();

                    //finish apk
                    finish();
                } else if (!myBluetooth.isEnabled()) {
                    //Ask to the user turn the bluetooth on
                    Intent turnBTon = new Intent(BluetoothAdapter.ACTION_REQUEST_ENABLE);
                    startActivityForResult(turnBTon, 1);
                }
            }
        }else {
            optManControl.CloseBTConnection(null);
        }
    }
}
