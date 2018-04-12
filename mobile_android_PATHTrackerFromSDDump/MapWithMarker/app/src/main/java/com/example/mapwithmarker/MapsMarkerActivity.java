package com.example.mapwithmarker;

import android.content.Context;
import android.content.pm.PackageManager;
import android.location.Location;
import android.location.LocationListener;
import android.location.LocationManager;
import android.os.Bundle;
import android.support.v4.app.ActivityCompat;
import android.support.v7.app.AppCompatActivity;
import android.util.Log;

import com.google.android.gms.maps.CameraUpdateFactory;
import com.google.android.gms.maps.GoogleMap;
import com.google.android.gms.maps.OnMapReadyCallback;
import com.google.android.gms.maps.SupportMapFragment;
import com.google.android.gms.maps.UiSettings;
import com.google.android.gms.maps.model.LatLng;
import com.google.android.gms.maps.model.MarkerOptions;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.FileReader;

import android.os.Environment;
import android.view.View;
import android.widget.Button;

/**
 * An activity that displays a Google map with a marker (pin) to indicate a particular location.
 */
public class MapsMarkerActivity extends AppCompatActivity
        implements OnMapReadyCallback {

    static final String TAG = "MapsMarkerActivity";
    LatLng currentLoc = null;
    GoogleMap googleMapObj = null;
    String filename = "gps_locations.csv";
    File file = null;
    FileOutputStream fos = null;
    Location prevLoc = null;

    FileInputStream fis = null;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        // Retrieve the content view that renders the map.
        setContentView(R.layout.activity_maps);
        // Get the SupportMapFragment and request notification
        // when the map is ready to be used.
        SupportMapFragment mapFragment = (SupportMapFragment) getSupportFragmentManager()
                .findFragmentById(R.id.map);
        mapFragment.getMapAsync(this);
    }

    /**
     * Manipulates the map when it's available.
     * The API invokes this callback when the map is ready to be used.
     * This is where we can add markers or lines, add listeners or move the camera. In this case,
     * we just add a marker near Sydney, Australia.
     * If Google Play services is not installed on the device, the user receives a prompt to install
     * Play services inside the SupportMapFragment. The API invokes this method after the user has
     * installed Google Play services and returned to the app.
     */
    @Override
    public void onMapReady(GoogleMap googleMap) {
        // Add a marker in Sydney, Australia,
        // and move the map's camera to the same location.
        googleMapObj = googleMap;
        try {
            if (ActivityCompat.checkSelfPermission(this, android.Manifest.permission.ACCESS_FINE_LOCATION) != PackageManager.PERMISSION_GRANTED && ActivityCompat.checkSelfPermission(this, android.Manifest.permission.ACCESS_COARSE_LOCATION) != PackageManager.PERMISSION_GRANTED) {
                // TODO: Consider calling
                //    ActivityCompat#requestPermissions
                // here to request the missing permissions, and then overriding
                //   public void onRequestPermissionsResult(int requestCode, String[] permissions,
                //                                          int[] grantResults)
                // to handle the case where the user grants the permission. See the documentation
                // for ActivityCompat#requestPermissions for more details.
                return;
            }
            googleMapObj.setMyLocationEnabled(true);
        }catch(Exception e){Log.v(TAG, "exception is " + e);}
        if(googleMapObj != null) {
            googleMapObj.getUiSettings().setZoomControlsEnabled(true);
            currentLoc = new LatLng(0, 0);
            googleMapObj.moveCamera(CameraUpdateFactory.newLatLng(currentLoc));
            googleMapObj.animateCamera(CameraUpdateFactory.zoomTo(100));
            /** read from the file and display markers for GPS */
            try {
                String filepath = Environment.getExternalStorageDirectory() +
                        File.separator + "optimus" + File.separator + "gps_log.txt";
                File fileGPSLog = new File(filepath);
                BufferedReader br = new BufferedReader(new FileReader(fileGPSLog));
                String line;
                int nIdx = 0;
                LatLng currLoc = null;
                while((line = br.readLine()) != null)
                {
                    Log.v(TAG, "read line " + line);
                    String[] tokens = line.split(" ");
                    //latitude = tokens[2]
                    //longitude = tokens[3]
                    //turning angle = tokens[4]
                    double fLat = Double.parseDouble(tokens[2]);
                    double fLong = Double.parseDouble(tokens[3]);
                    double fTurningAngle = Double.parseDouble(tokens[4]);
                    Log.v(TAG, "lat=" + fLat + " " + fLong);
                    currLoc = new LatLng(fLat, fLong);
                    nIdx++;
                    googleMapObj.addMarker(new MarkerOptions().position(currLoc).title(""+nIdx + ";" + fTurningAngle));


                }
                if(currLoc != null) {
                    googleMapObj.moveCamera(CameraUpdateFactory.newLatLng(currLoc));
                    googleMapObj.animateCamera(CameraUpdateFactory.zoomTo(17));
                }
                br.close();
            }catch(Exception e){Log.v(TAG, "file read failed " + e);}

        }
    }
}
