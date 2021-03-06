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

import java.io.File;
import java.io.FileOutputStream;

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

        LocationManager locationManager = (LocationManager) getSystemService(Context.LOCATION_SERVICE);
        LocationListener locationListener = new LocationListener() {
            @Override
            public void onLocationChanged(Location location) {
                LatLng loc = new LatLng(location.getLatitude(), location.getLongitude());
                Log.v(TAG, "location latittude=" + location.getLatitude() + " long=" + location.getLongitude());
                //if(prevLoc != null && location.distanceTo(prevLoc) > 0.5)
                {
                    /** greater that so many meters, log ! */
                    String latLong = "" + loc.latitude + " " + loc.longitude + "\n";
                    try {
                        fos.write(latLong.getBytes());
                        fos.flush();
                    }catch(Exception e){Log.v(TAG, "exception " + e);}
                }
                currentLoc = loc;
                prevLoc = location;
                if(googleMapObj != null) {

                    googleMapObj.moveCamera(CameraUpdateFactory.newLatLng(currentLoc));
                    //googleMapObj.animateCamera(CameraUpdateFactory.zoomTo(100));
                }
            }

            @Override
            public void onStatusChanged(String provider, int status, Bundle extras) {
                Log.v(TAG, "status changed " + provider + "status=" + status);
            }

            @Override
            public void onProviderEnabled(String provider) {
                Log.v(TAG, "provider enabled " + provider);

            }

            @Override
            public void onProviderDisabled(String provider) {
                Log.v(TAG, "provider disabled " + provider);
            }
        };
        if (ActivityCompat.checkSelfPermission(this, android.Manifest.permission.ACCESS_FINE_LOCATION) != PackageManager.PERMISSION_GRANTED
                && ActivityCompat.checkSelfPermission(this, android.Manifest.permission.ACCESS_COARSE_LOCATION) != PackageManager.PERMISSION_GRANTED) {
            Log.v(TAG, "not enough GPS permissions");
            //    ActivityCompat#requestPermissions
            // here to request the missing permissions, and then overriding
            //   public void onRequestPermissionsResult(int requestCode, String[] permissions,
            //                                          int[] grantResults)
            // to handle the case where the user grants the permission. See the documentation
            // for ActivityCompat#requestPermissions for more details.

            return;
        }
        locationManager.requestLocationUpdates(LocationManager.GPS_PROVIDER, 1000, 1, locationListener);
        Log.v(TAG, "location listener registered");

        /** create the file */
        try
        {
            String dir = Environment.getExternalStorageDirectory() + File.separator + "";
            file = new File(dir, filename);
            fos = new FileOutputStream(file, true);
        }catch(Exception e){Log.v(TAG, "exception occured " + e);}

        Button resetDump = (Button)findViewById(R.id.reset_dump);
        resetDump.setOnClickListener(new View.OnClickListener()
            {
                public void onClick(View v)
                {
                    //recreate the file
                    try
                    {
                        file.delete();
                        String dir = Environment.getExternalStorageDirectory() + File.separator + "";
                        file = new File(dir, filename);
                        fos = new FileOutputStream(file, true);
                    }catch(Exception e){Log.v(TAG, "exception occured " + e);}

                }
            }
        );

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
            googleMapObj.setMyLocationEnabled(true);
        }catch(Exception e){Log.v(TAG, "exception is " + e);}
        if(googleMapObj != null) {
            currentLoc = new LatLng(0, 0);
            googleMapObj.moveCamera(CameraUpdateFactory.newLatLng(currentLoc));
            googleMapObj.animateCamera(CameraUpdateFactory.zoomTo(100));
        }
    }
}
