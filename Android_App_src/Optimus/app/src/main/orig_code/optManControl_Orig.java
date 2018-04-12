package com.optimus.rccar;

import android.annotation.SuppressLint;
import android.app.AlertDialog;
import android.content.DialogInterface;
import android.os.Bundle;
import android.os.Handler;
import android.os.Looper;
import android.os.Message;
import android.support.v7.app.AppCompatActivity;
import android.view.Menu;
import android.view.MenuItem;

import android.bluetooth.BluetoothSocket;
import android.content.Intent;
import android.view.View;
import android.widget.Button;
import android.widget.SeekBar;
import android.widget.TextView;
import android.widget.Toast;
import android.app.ProgressDialog;
import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.os.AsyncTask;
//import android.bluetooth.ScoSocket;

import java.io.IOException;
import java.io.UnsupportedEncodingException;
import java.util.Arrays;
import java.util.UUID;
import android.widget.ToggleButton;

import static processing.core.PApplet.println;
import static processing.core.PApplet.split;
import static processing.core.PApplet.trim;

public class optManControl extends AppCompatActivity {

    public static final int MSG_TYPE_BT_SENSOR = 1;
    public static final int MSG_TYPE_BT_COMPASS = 2;
    public static final int MSG_TYPE_BT_GPS = 3;

    Button btnDis, btn_nav;
    ToggleButton Tbutton_power;
    //Button btnOn, btnOff, btnDis;
    SeekBar brightness;
    TextView lumn;
    String address = null;
    //private ScoSocket mConnectedSco;
    private ProgressDialog progress;
    BluetoothAdapter myBluetooth = null;
    BluetoothSocket btSocket = null;
    private boolean isBtConnected = false;
    optimusNavActivity optimusNavActivity;
    //AlertDialog.Builder dialog;
    //AlertDialog OptionDialog;
    RecvdOptimusData recvdOptimusData = null;
    ImageDrawer imageDrawer = new ImageDrawer();

    //SPP UUID. Look for it
    static final UUID myUUID = UUID.fromString("00001101-0000-1000-8000-00805F9B34FB");

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        Intent newint = getIntent();
        address = newint.getStringExtra(btDeviceList.EXTRA_ADDRESS); //receive the address of the bluetooth device

        //view of the optmanControl
        setContentView(R.layout.activity_optman_control);

        //call the widgets
        btnDis = (Button) findViewById(R.id.button4);
        btn_nav = (Button) findViewById(R.id.button5);
        brightness = (SeekBar) findViewById(R.id.seekBar);
        lumn = (TextView) findViewById(R.id.lumn);

        new ConnectBT().execute(); //Call the class to connect

        //Call the Toggle Button Widget
        Tbutton_power = (ToggleButton) findViewById(R.id.toggleButton2);
        Tbutton_power.setOnClickListener(new View.OnClickListener() {

            @Override
            public void onClick(View v) {
                //TODO: Move this Power button logic to NAVIGATION PAGE and enable the check of "isNavChckPtSet"
                if ((Tbutton_power.isChecked()) /*&& (optimusNavActivity.isNavChckPtSet)*/) {
                    turnOnOptimus();      //method to send TRANSMISSION-ON Message to RC CAR
                    Toast.makeText(optManControl.this, "Toggle Power Button is on", Toast.LENGTH_LONG).show();
                } else if (!Tbutton_power.isChecked()) {
                    turnOffOptimus();   //method to send TRANSMISSION-OFF Message to RC CAR
                    Toast.makeText(optManControl.this, "Toggle Power button is Off", Toast.LENGTH_LONG).show();
                } else {
                    Toast.makeText(optManControl.this, "Can't power RC, NAV Points Not Set!!!, Please Set NAV points First!", Toast.LENGTH_LONG).show();
                }
            }

        });

        btn_nav.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                //Switching to Navigation Activity.
                Intent I = new Intent(optManControl.this, optimusNavActivity.class);
                startActivity(I);
            }
        });

        btnDis.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                Disconnect(); //close connection
            }
        });

        brightness.setOnSeekBarChangeListener(new SeekBar.OnSeekBarChangeListener() {
            @Override
            public void onProgressChanged(SeekBar seekBar, int progress, boolean fromUser) {
                if (fromUser == true) {
                    lumn.setText(String.valueOf(progress/255*100));
                    try {
                        btSocket.getOutputStream().write(String.valueOf(progress).getBytes());
                    } catch (IOException e) {

                    }
                }
            }

            @Override
            public void onStartTrackingTouch(SeekBar seekBar) {

            }

            @Override
            public void onStopTrackingTouch(SeekBar seekBar) {

            }
        });
    }

    /*private void alertView(int messageId) {
        OptionDialog = new AlertDialog.Builder(this).create();
        OptionDialog.setTitle("BT - Connection Progress");
        OptionDialog.setIcon(R.drawable.btconnect);
        OptionDialog.setMessage(optManControl.this.getText(messageId));

        //Optional Method
        //dialog = new AlertDialog.Builder(optManControl.this);
        *//*dialog.setTitle( "BT - Connection Progress" )
                .setIcon(R.drawable.btconnect)
                .setMessage(messageId);*//*
    }*/

    private void Disconnect() {
        if (btSocket != null) //If the btSocket is busy
        {
            try {
                btSocket.close(); //close connection
            } catch (IOException e) {
                msg("BT Disconnect Error");
            }
        }
        finish(); //return to the first layout

    }

    public void CloseBTConnection() {
        Disconnect();
    }

    private void turnOffOptimus() {
        if (btSocket != null) {
            try {
                btSocket.getOutputStream().write("TROFF".toString().getBytes());
            } catch (IOException e) {
                msg("TurnOff Transmit Error");
            }
        }
    }

    private void turnOnOptimus() {
        if (btSocket != null) {
            msg("BTsocket ! NULL");
            try {
                msg("Entering TurnON");
                btSocket.getOutputStream().write("TRON".toString().getBytes());
            } catch (IOException e) {
                msg("TurnOn Transmit Error");
            }
        } else {
            msg("BTsocket NULL");
        }
    }

    public void sendCheckPoints(double[][] chck_pts_list){
        String GeoCords[] = new String[50];
        String checkptlist = null;
        for (int i = 0; i < chck_pts_list.length; i++) {
            String Lat = String.valueOf(chck_pts_list[i][0]);
            String Long = String.valueOf(chck_pts_list[i][1]);
            GeoCords[i] = Lat + "," + Long;
            println("geocords:202:"+ GeoCords[i]);
        }
        for(int i = 1; i < chck_pts_list.length; i++){
            checkptlist = checkptlist + ";" + GeoCords[i-1];
        }
        byte[] b = checkptlist.getBytes();
        println("Bytes:208:"+ b);
        println("checkptlist:209:"+ checkptlist);
        if (btSocket != null) {
            println("BTsocket ! NULL");
            try {
                println("Sending CheckPoints");
                btSocket.getOutputStream().write(b);
            } catch (IOException e) {
                println("CheckPoints Transmit Error");
            }
        } else {
            println("BTsocket NULL");
        }
    }

    // The Handler obtains the data from the Socket
    private final Handler mHandler = new Handler() {
        @Override
        public void handleMessage(Message msg) {
            switch(msg.what){
                case MSG_TYPE_BT_SENSOR: {
                    // Get the data from the msg.obj
                    String strReceived = null;
                    byte[] dataReceived = (byte[]) msg.obj;
                    if(dataReceived != null){
                        // construct a string from the valid bytes in the buffer
                        strReceived = new String(dataReceived, 0, msg.arg1);
                        if (strReceived != null) {
                            strReceived = trim(strReceived);
                            String[] values = split(strReceived, ',');
                            try {
                                msg("Setting Angle and Distance!");
                                imageDrawer.setAngle_Distance(values);
                            } catch (Exception e) {
                                msg("Angle-Distance Set Error");
                            }
                        }
                        else{
                            msg("Null str rcvd!!!");
                        }
                    }
                    else{
                        msg("Null data rcvd!!!");
                    }
                }
                break;
            }
        }
    };

    // fast way to call Toast
    private void msg(String s) {
        Toast.makeText(this.getApplicationContext(), s, Toast.LENGTH_LONG).show();
    }

    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        // Inflate the menu; this adds items to the action bar if it is present.
        getMenuInflater().inflate(R.menu.menu_optimus_man_control, menu);
        return true;
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        // Handle action bar item clicks here. The action bar will
        // automatically handle clicks on the Home/Up button, so long
        // as you specify a parent activity in AndroidManifest.xml.
        int id = item.getItemId();

        //noinspection SimplifiableIfStatement
        if (id == R.id.action_settings) {
            return true;
        }

        return super.onOptionsItemSelected(item);
    }

    private class ConnectBT extends AsyncTask<Void, Void, Void>  // UI thread
    {
        private boolean ConnectSuccess = true; //if it's here, it's almost connected

        @Override
        protected void onPreExecute() {
            //alertView(R.string.title_dev_conn_progress);
            progress = ProgressDialog.show(optManControl.this, "Connecting...", "Please wait!!!");  //show a progress dialog
        }

        @Override
        protected Void doInBackground(Void... devices) //while the progress dialog is shown, the connection is done in background
        {
            try {
                if (btSocket == null || !isBtConnected) {
                    myBluetooth = BluetoothAdapter.getDefaultAdapter();//get the mobile bluetooth device
                    BluetoothDevice dispositivo = myBluetooth.getRemoteDevice(address);//connects to the device's address and checks if it's available
                    btSocket = dispositivo.createInsecureRfcommSocketToServiceRecord(myUUID);//create a RFCOMM (SPP) connection
                    //btSocket = mConnectedSco.createScoSocket();//Create SCO socket connection using either "mConnectedSco" OR "dispositivo" objects.
                    BluetoothAdapter.getDefaultAdapter().cancelDiscovery();
                    btSocket.connect();//start connection
                }
            } catch (IOException e) {
                ConnectSuccess = false;//if the try failed, you can check the exception here
            }
            return null;
        }

        @Override
        protected void onPostExecute(Void result) //after the doInBackground, it checks if everything went fine
        {
            super.onPostExecute(result);
            if (!ConnectSuccess) {
                msg("Connection Failed. Is it a SPP Bluetooth? Try again.");
                finish();
            } else {
                //Start new Thread to listen for data on BT port
                msg("Connected. Let's start a new Thread!!!");
                recvdOptimusData = new RecvdOptimusData();
                new Thread(recvdOptimusData).start();
                msg("Connected.!!!");
                isBtConnected = true;
            }
            progress.dismiss();
            //dialog.create().dismiss();
            //OptionDialog.dismiss();
        }
    }

    private class RecvdOptimusData implements Runnable{
        public void run() {
            if (Looper.myLooper() == null)
            {
                Looper.prepare();
            }
            byte[] buffer = new byte[1024]; // buffer to store the string
            int bytes; // Number of bytes read
            if (btSocket != null) {
                msg("BTsocket ! NULL");
                while (true) {
                    try {
                        msg("Reading Data from BT Socket sent by Optimus");
                        bytes = btSocket.getInputStream().read(buffer);
                        mHandler.obtainMessage(MSG_TYPE_BT_SENSOR, bytes, -1, buffer)
                                .sendToTarget();
                    } catch (IOException e) {
                        msg("BT Rcvd_Msg Read Error");
                    }
                }
            } else {
                msg("BTsocket NULL");
            }
        }
    }
}
