package com.optimus.rccar;

import android.app.Activity;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.IntentFilter;
import android.os.Bundle;
import android.os.Handler;
import android.os.Looper;
import android.os.Message;
//import android.support.v7.app.AppCompatActivity;
import android.util.JsonReader;
import android.util.Log;
import android.view.Menu;
import android.view.MenuItem;
import android.bluetooth.BluetoothSocket;
import android.content.Intent;
import android.widget.Toast;
import android.app.ProgressDialog;
import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.os.AsyncTask;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.util.Arrays;
import java.util.UUID;
import static processing.core.PApplet.print;
import static processing.core.PApplet.println;
import static processing.core.PApplet.split;
import static processing.core.PApplet.trim;

public class optManControl extends Activity {

    public static final int MSG_TYPE_BT_SENSOR = 1;
    public static final int MSG_TYPE_BT_COMPASS = 2;
    public static final int MSG_TYPE_BT_GPS = 3;

    boolean running = false;
    String address = null;
    private ProgressDialog progress;
    BluetoothAdapter myBluetooth = null;
    private static BluetoothSocket btSocket = null;
    public static boolean isBtConnected = false;
    RecvdOptimusData recvdOptimusData = null;
    ImageDrawer imageDrawer = new ImageDrawer();
    IntentFilter filter = new IntentFilter();

    String TAG = "optManControl";
    //SPP UUID. Look for it
    static final UUID myUUID = UUID.fromString("00001101-0000-1000-8000-00805F9B34FB");

    @Override
    protected void onCreate(Bundle savedInstanceState){
        super.onCreate(savedInstanceState);

        Intent newint = getIntent();
        address = newint.getStringExtra(btDeviceList.EXTRA_ADDRESS); //receive the address of the bluetooth device

        filter.addAction(BluetoothDevice.ACTION_ACL_CONNECTED);
        filter.addAction(BluetoothDevice.ACTION_ACL_DISCONNECT_REQUESTED);
        filter.addAction(BluetoothDevice.ACTION_ACL_DISCONNECTED);
        this.registerReceiver(BTReceiver, filter);

        /*optManControl prevActivity = (optManControl)getLastNonConfigurationInstance();
        if(prevActivity!= null) {
            // So the orientation did change
            // Restore some field for example
            this.btSocket = prevActivity.btSocket;
        }*/

        new ConnectBT().execute(); //Call the class to connect
    }

    private void Disconnect(BluetoothSocket bt_socket) {
        if (bt_socket != null) //If the btSocket is busy
        {
            try {
                bt_socket.close(); //close connection
            } catch (IOException e) {
                println("BT Disconnect Error");
            }
        }
        else {
            if (btSocket != null) //If the btSocket is busy
            {
                try {
                    btSocket.close(); //close connection
                } catch (IOException e) {
                    println("Orig BT Disconnect Error");
                }
            }
            else
                println("btSocket null");
        }
        finish(); //return to the first layout
    }

    public void CloseBTConnection(BluetoothSocket bt_socket){
        isBtConnected = false;
        running = false;
        Disconnect(bt_socket);
    }

    public void turnOffOptimus() {
        if (btSocket != null) {
            try {
                print("------------------TURNING OFF ----------------");
                btSocket.getOutputStream().write("TROFF".toString().getBytes());
            } catch (IOException e) {
                msg("TurnOff Transmit Error");
            }
        }
    }

    public void turnOnOptimus() {
        if (btSocket != null) {
            try {
                print("------------------TURNING ON ----------------");
                btSocket.getOutputStream().write("TRON".toString().getBytes());
            } catch (IOException e) {
                msg("TurnOn Transmit Error");
            }
        } else {
            println("BTsocket NULL");
        }
    }

    /*public void sendCheckPoints(double[][] chck_pts_list){
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
    }*/

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
            optimusHome.btconnecting();
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
                        /*for(int i=0;i<50000;i++)
                        println("i"+i);*/
                        btSocket.connect();//start connection
                }
            } catch (IOException e) {
                println("exception"+e);
                //if the device has bluetooth
                myBluetooth = BluetoothAdapter.getDefaultAdapter();

                if (myBluetooth == null) {
                    //Show a mensag. that the device has no bluetooth adapter
                    Toast.makeText(getApplicationContext(), "Bluetooth Device Not Available", Toast.LENGTH_LONG).show();

                    //finish apk
                    finish();
                }
                if(!myBluetooth.isEnabled())
                {
                    //Ask to the user turn the bluetooth on
                    Intent turnBTon = new Intent(BluetoothAdapter.ACTION_REQUEST_ENABLE);
                    startActivityForResult(turnBTon,1);
                }
                ConnectSuccess = false;//if the try failed, you can check the exception here
            }
            return null;
        }

        @Override
        protected void onPostExecute(Void result) //after the doInBackground, it checks if everything went fine
        {
            super.onPostExecute(result);
            if (!ConnectSuccess) {
                optimusHome.btdisconnected();
                isBtConnected = false;
                msg("Connection Failed. Is it a SPP Bluetooth? Try again!");
                finish();
            } else {
                //Start new Thread to listen for data on BT port
                msg("Connected");
                optimusHome.btconnected();
                isBtConnected = true;
                Intent I = new Intent(optManControl.this, optimusHome.class);
                I.setFlags(Intent.FLAG_ACTIVITY_CLEAR_TOP | Intent.FLAG_ACTIVITY_SINGLE_TOP);
                startActivity(I);
                running = true;
                recvdOptimusData = new RecvdOptimusData();
                //startThread(recvdOptimusData);
                new Thread(recvdOptimusData).start();
                if(!running){
                    isBtConnected = false;
                }
            }
            if(null!= progress)
                progress.dismiss();
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
            Log.v(TAG, "btSocket " + btSocket);
            if (btSocket != null) {
                msg("BTsocket ! NULL");
                InputStream isFromSocket = null;
                try {
                    isFromSocket = btSocket.getInputStream();
                    while (true) {
                        try {
                            msg("Reading Data from BT Socket sent by Optimus");
                            Log.v("optManControl", "reading data");
                            JsonReader jsonReader = new JsonReader(new InputStreamReader(isFromSocket, "UTF-8"));
                            Log.v("optManControl", "got JsonReader" + jsonReader.peek().toString());
                            jsonReader.setLenient(true);
                            jsonReader.beginObject();
                            Log.v("optManControl", "got JsonReader" + jsonReader.peek().toString());
                            /**
                             * ﻿#define JSON_ID_COMPASS_HEADING "heading"
                             #define JSON_ID_COMPASS_BEARING "bearing"
                             #define JSON_ID_TURNING_ANGLE   "turning"
                             #define JSON_ID_DIST_TO_DEST    "dist_to_dest"
                             #define JSON_ID_DEST_REACHED    "dest_reach_stat"
                             #define JSON_ID_CHECKPOINT_ID   "checkpoint_id"
                             #define JSON_ID_GPS_LOCK_STAT   "gps_lock"
                             */
                            boolean bGpsLock = false;
                            double fCompassHeading = 0.0;
                            double fCompassBearing = 0.0;
                            int nTurningAngle = 0;
                            double fDistToDest = 0.0;
                            boolean bDestReachStat = false;
                            int nCheckpointID = 0;
                            int nCheckpoints = 0;
                            int obstacle_info_track_vals[] = new int[12];
                            Arrays.fill(obstacle_info_track_vals, 0);

                            while(jsonReader.hasNext())
                            {
                                Log.v("optManControl", "hasNext");
                                String idStr = jsonReader.nextName();
                                Log.v("optManControl", "idStr " + idStr);
                                if(idStr.equals("heading"))
                                {
                                    Log.v("optManControl", "here");
                                    fCompassHeading = jsonReader.nextDouble();
                                    Log.v("optManControl", "fCompassHeading = " + fCompassHeading);
                                    //optimusDashboard.setMsDebugInfo(heading_Str);
                                }
                                else if(idStr.equals("bearing"))
                                {
                                    fCompassBearing = jsonReader.nextDouble();
                                }
                                else if(idStr.equals("turning"))
                                {
                                    nTurningAngle = jsonReader.nextInt();
                                }
                                else if(idStr.equals("dist_to_dest"))
                                {
                                    fDistToDest = jsonReader.nextDouble();
                                }
                                else if(idStr.equals("dest_reach_stat"))
                                {
                                    bDestReachStat = jsonReader.nextInt() == 0 ? false : true;
                                }
                                else if(idStr.equals("checkpoint_id"))
                                {
                                    nCheckpointID = jsonReader.nextInt();
                                }
                                else if(idStr.equals("gps_lock"))
                                {
                                    bGpsLock = jsonReader.nextInt() == 0 ? false : true;
                                }
                                else if(idStr.contains("lidar_sect"))
                                {
                                    int track = jsonReader.nextInt();
                                    int sector = Integer.parseInt(("" +idStr.substring(10)));
                                    Log.v(TAG, "sector " + sector + " track: " + track + " ");
                                    obstacle_info_track_vals[sector] = track;
                                }
                                else if(idStr.equals("gps_cp_count"))
                                {
                                    nCheckpoints = jsonReader.nextInt();
                                }
                                else
                                {
                                    Log.v(TAG, "skipping value");
                                    jsonReader.skipValue();
                                }
                                Log.v(TAG, "value consumed");
                            }
                            jsonReader.endObject();
                            optimusDashboard.setMsDebugInfo(
                                    "heading: " + fCompassHeading + "\n" +
                                    "bearing: " + fCompassBearing + "\n" +
                                    "turning: " + nTurningAngle + "\n" +
                                    "dist_to_dest: " + fDistToDest + "\n" +
                                    "dest_reach_stat: " + bDestReachStat + "\n" +
                                    "lap_status: " + nCheckpointID + "/" + nCheckpoints + "\n" +
                                    "gps_lock: " + bGpsLock + "\n"
                            );
                            LidarMap mapL = null;
                            if(optimusDashboard.getDashBoardActivity() != null) {
                                mapL = (LidarMap) optimusDashboard.getDashBoardActivity().findViewById(R.id.lidar_map);
                            }

                            if(mapL != null)
                                mapL.reportObstacle(obstacle_info_track_vals);

                            //if(mapL != null)
                              //  mapL.clearCanvasAndRedrawSectors();

//                            for(int i = 0; i < 12 && (mapL != null); i++)
//                            {
//                                Log.v(TAG, "mapL=" + mapL + "obs = " + obstacle_info_track_vals[i]);
//                                if(obstacle_info_track_vals[i] != 0)
//                                    mapL.reportObstacle(i, obstacle_info_track_vals[i]);
//                            }
                        } catch (IOException e) {
                            msg("BT Rcvd_Msg Read Error");
                            Log.v("optManControl", "Exception " + e);
                        }
                    }

                }catch(Exception e){Log.v("Exception", "Exception is " + e);}
            } else {
                msg("BTsocket NULL");
            }
        }
    }

    public static BluetoothSocket getbtsocket(){
        return btSocket;
    }

    //The BroadcastReceiver that listens for bluetooth broadcasts
    private final BroadcastReceiver BTReceiver = new BroadcastReceiver() {
        @Override
        public void onReceive(Context context, Intent intent) {
            String action = intent.getAction();

            if (BluetoothDevice.ACTION_ACL_CONNECTED.equals(action)) {
                //Do something if connected
                Toast.makeText(getApplicationContext(), "BT Connected", Toast.LENGTH_SHORT).show();
            }
            else if (BluetoothDevice.ACTION_ACL_DISCONNECT_REQUESTED.equals(action)) {
                //Device is about to disconnect
                Toast.makeText(getApplicationContext(), "BT Disconnection requested!!!", Toast.LENGTH_SHORT).show();
            }
            else if (BluetoothDevice.ACTION_ACL_DISCONNECTED.equals(action)) {
                //Device has disconnected
                optimusHome.btdisconnected();
                isBtConnected = false;
                println("BT Disconnected");
            }
            else{
                //Do Nothing
            }
        }
    };

   /* public void unregisterBTReceiver(){
        //Unregister Receiver
        this.unregisterReceiver(BTReceiver);
    }*/
    /*@Override
    public Object onRetainNonConfigurationInstance() {
        if (btSocket != null) // Check that the object exists
            return(btSocket);
        return super.onRetainNonConfigurationInstance();
    }*/
}
