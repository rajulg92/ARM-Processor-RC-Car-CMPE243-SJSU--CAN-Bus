package com.optimus.rccar;

import android.Manifest;
import android.app.AlertDialog;
import android.app.Service;
import android.content.Context;
import android.content.DialogInterface;
import android.content.Intent;
import android.content.pm.PackageManager;
import android.location.Location;
import android.location.LocationListener;
import android.location.LocationManager;
import android.os.Bundle;
import android.os.IBinder;
import android.provider.Settings;
import android.support.v4.app.ActivityCompat;
import android.support.v4.content.ContextCompat;
import android.widget.Toast;

import java.security.Permission;

import static processing.core.PApplet.println;

/**
 * Created by Parimal on 10/22/2017.
 */

public class GPSTracker extends Service implements LocationListener {

    private final Context context;
    private static final long MIN_DISTANCE_FOR_UPDATES = 10;
    private static final long MIN_TIME_BTW_UPDATES = 1000*60*1;
    boolean isGPSEnabled = false;
    boolean isNetworkEnabled = false;
    boolean canGetLocation = false;

    Location location;
    protected LocationManager locationManager;

    public GPSTracker(Context context){
        this.context = context;
    }

    public Location getLocation(){
        canGetLocation = false;
        try{
            locationManager = (LocationManager) context.getSystemService(LOCATION_SERVICE);
            isGPSEnabled = locationManager.isProviderEnabled(LocationManager.GPS_PROVIDER);
            isNetworkEnabled = locationManager.isProviderEnabled(LocationManager.NETWORK_PROVIDER);
            println("GPS STATUS:"+ isGPSEnabled);

            if(ContextCompat.checkSelfPermission(context, Manifest.permission.ACCESS_FINE_LOCATION) == PackageManager.PERMISSION_GRANTED
                    || ContextCompat.checkSelfPermission(context, Manifest.permission.ACCESS_COARSE_LOCATION) == PackageManager.PERMISSION_GRANTED){
                if(isGPSEnabled){
                    if(location == null){
                        locationManager.requestLocationUpdates(LocationManager.GPS_PROVIDER, MIN_TIME_BTW_UPDATES, MIN_DISTANCE_FOR_UPDATES, this);
                        if(locationManager != null){
                            location = locationManager.getLastKnownLocation(LocationManager.GPS_PROVIDER);
                            canGetLocation = true;
                            println("GPS Lat-Long 60:" + location.getLatitude()+ location.getLongitude());
                        }
                    }
                    else
                    {
                        println("65:location!= null");
                    }
                }

                //If Location is not found from GPS then it will be found from Network
                if(location == null){
                    if(isNetworkEnabled){
                        locationManager.requestLocationUpdates(LocationManager.NETWORK_PROVIDER, MIN_TIME_BTW_UPDATES, MIN_DISTANCE_FOR_UPDATES, this);
                        if(locationManager != null){
                            location = locationManager.getLastKnownLocation(LocationManager.NETWORK_PROVIDER);
                            println("Network Lat-Long 75:" + location.getLatitude()+ location.getLongitude());
                            canGetLocation = true;
                        }
                    }
                }
                else
                {
                    println("82:location!= null");
                }
            }
            else{
                //TODO: Check how to ask for User permissions.
                //ActivityCompat.requestPermissions(this, new String[]{Manifest.permission.ACCESS_FINE_LOCATION}, 200);
                println("Verification issue!");
            }
        }catch (Exception ex){
            ex.printStackTrace();
        }
        return location;
    }

    public boolean canGetLocation(){
        return  this.canGetLocation;
    }
    public void onLocationChanged(Location location){

    }
    public void onStatusChanged(String Provider, int status, Bundle extras){

    }
    public void onProviderEnabled(String Provider){

    }
    public void onProviderDisabled(String Provider){

    }
    public IBinder onBind(Intent arg0){
        return null;
    }

    public void showSettingsAlert(){
        AlertDialog.Builder alertDialog = new AlertDialog.Builder(context);

        //Setting Dialog Title
        alertDialog.setTitle("GPS - Settings");

        //Setting Dialog messages
        alertDialog.setMessage("GPS is not enabled, mind Switching GPS ON?");

        //On Pressing Settings Menu
        alertDialog.setPositiveButton("Settings", new DialogInterface.OnClickListener(){
            public void onClick(DialogInterface dialog, int which){
                Intent intent = new Intent(Settings.ACTION_LOCATION_SOURCE_SETTINGS);
                context.startActivity(intent);
            }
        });

        //On pressing Cancel Button
        alertDialog.setNegativeButton("Cancel", new DialogInterface.OnClickListener(){
            public void onClick(DialogInterface dialog, int which){
                dialog.cancel();
            }
        });

        //Showing Alert Message
        alertDialog.show();
    }
}
