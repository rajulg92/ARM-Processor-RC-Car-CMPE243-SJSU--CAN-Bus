package com.optimus.rccar;

import android.app.ProgressDialog;
import android.content.Intent;
import android.graphics.Color;
import android.location.Address;
import android.location.Geocoder;
import android.location.Location;
import android.os.Handler;
import android.support.v4.app.FragmentActivity;
import android.os.Bundle;
import android.view.View;
import android.widget.Button;
import android.widget.EditText;
import android.widget.TextView;
import android.widget.Toast;
import com.google.android.gms.maps.CameraUpdateFactory;
import com.google.android.gms.maps.GoogleMap;
import com.google.android.gms.maps.OnMapReadyCallback;
import com.google.android.gms.maps.SupportMapFragment;
//import com.google.android.gms.maps.model.BitmapDescriptor;
import com.google.android.gms.maps.model.BitmapDescriptorFactory;
import com.google.android.gms.maps.model.LatLng;
import com.google.android.gms.maps.model.Marker;
import com.google.android.gms.maps.model.MarkerOptions;
import com.google.android.gms.maps.model.Polyline;
import com.google.android.gms.maps.model.PolylineOptions;
//import com.optimus.rccar.DirectionFinderListener;
//import com.optimus.rccar.DirectionFinder;
//import com.optimus.rccar.Route;

import java.io.UnsupportedEncodingException;
import java.util.ArrayList;
import java.util.List;

import static processing.core.PApplet.println;

public class optimusNavActivity extends FragmentActivity implements OnMapReadyCallback, DirectionFinderListener{

    boolean isNavChckPtSet = true;  // TODO: It is set to TRUE for default here. Need to set it according to when only NAV pt is set else returns FALSE.
    private GoogleMap mMap;
    private GPSTracker gpsTracker;
    private Location mlocation;
    double latitude, longitude;

    private Button btnFindPath, btn_dashboard;
    private EditText etOrigin;
    private EditText etDestination;
    private List<Marker> originMarkers = new ArrayList<>();
    private List<Marker> destinationMarkers = new ArrayList<>();
    private List<Polyline> polylinePaths = new ArrayList<>();
    private ProgressDialog progressDialog;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_optimus__nav_);

        btnFindPath = (Button) findViewById(R.id.btnFindPath);
        btn_dashboard = (Button) findViewById(R.id.btn_dashboard);
        etOrigin = (EditText) findViewById(R.id.etOrigin);
        etDestination = (EditText) findViewById(R.id.etDestination);

        btnFindPath.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                sendRequest();
            }
        });
        btn_dashboard.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                //Switching to Dashboard Activity.
                Intent I = new Intent(optimusNavActivity.this, optimusDashboard.class);
                startActivity(I);
            }
        });

        gpsTracker = new GPSTracker(getApplicationContext());
        mlocation = gpsTracker.getLocation();
        if (mlocation != null) {
            if (gpsTracker.canGetLocation()) {
                latitude = mlocation.getLatitude();
                longitude = mlocation.getLongitude();
            } else {
                gpsTracker.showSettingsAlert();
            }
        } else {
            Toast.makeText(optimusNavActivity.this, "Can't resolve Location Value", Toast.LENGTH_LONG).show();
        }

        // Obtain the SupportMapFragment and get notified when the map is ready to be used.
        SupportMapFragment mapFragment = (SupportMapFragment) getSupportFragmentManager()
                .findFragmentById(R.id.map);
        mapFragment.getMapAsync(this);
    }

    private void sendRequest() {
        String origin = etOrigin.getText().toString();
        //origin = "37.3360008,-121.8847221";
        String destination = etDestination.getText().toString();
        //destination = "37.4032078,-121.9720297";
        println("route>>origin:"+ origin);
        println("route>>destination:"+ destination);
        if (origin.isEmpty()) {
            Toast.makeText(this, "Please enter origin address!", Toast.LENGTH_SHORT).show();
            return;
        }
        if (destination.isEmpty()) {
            Toast.makeText(this, "Please enter destination address!", Toast.LENGTH_SHORT).show();
            return;
        }

        try {
            new DirectionFinder(this, origin, destination).execute();
        } catch (UnsupportedEncodingException e) {
            e.printStackTrace();
        }
    }

    /**
     * Manipulates the map once available.
     * This callback is triggered when the map is ready to be used.
     * This is where we can add markers or lines, add listeners or move the camera. In this case,
     * we just add a marker near Sydney, Australia.
     * If Google Play services is not installed on the device, the user will be prompted to install
     * it inside the SupportMapFragment. This method will only be triggered once the user has
     * installed Google Play services and returned to the app.
     */
    @Override
    public void onMapReady(GoogleMap googleMap) {
        mMap = googleMap;

        // Add a marker in Sydney and move the camera
        //LatLng sydney = new LatLng(-34, 151);
        //Instatntiate the class LatLng
        LatLng mylocation = new LatLng(latitude, longitude);
        //LatLng nextTargetlocation = new LatLng(37.339426, -121.891169);
        //Instantiate the class GeoCoder
        //Geocoder geocoder = new Geocoder(getApplicationContext());
        try{
            /*List<Address> addressList = geocoder.getFromLocation(latitude, longitude, 1);
            String plc_holder_str = addressList.get(0).getLocality()+",";
            plc_holder_str += addressList.get(0).getCountryName();*/

            MarkerOptions marker =  new MarkerOptions().position(mylocation).title("My GPS"/*plc_holder_str*/);
            marker.icon(BitmapDescriptorFactory.fromResource(R.drawable.nav9));
            mMap.addMarker(marker);
            //mMap.moveCamera(CameraUpdateFactory.newLatLng(mylocation));
            //mMap.animateCamera(CameraUpdateFactory.zoomTo(18.0f));
            mMap.moveCamera(CameraUpdateFactory.newLatLngZoom(mylocation, 18.0f));
            //mMap.setMapType(GoogleMap.MAP_TYPE_SATELLITE);
            //mMap.setMyLocationEnabled(true);
            /*mMap.addPolyline(new PolylineOptions().add(
                    mylocation,
                    new LatLng(37.339426, -125.891169),
                    new LatLng(37.339426, -123.891169),
                    nextTargetlocation
                )
                .width(10)
                .color(Color.RED)
            );*/
        }catch (Exception e){
            e.printStackTrace();
        }
        //mMap.addMarker(new MarkerOptions().position(mylocation).title("Start Point"));
        //mMap.moveCamera(CameraUpdateFactory.newLatLng(mylocation));
    }

    @Override
    public void onDirectionFinderStart() {
        progressDialog = ProgressDialog.show(this, "Please wait.",
                "Finding direction..!", true);

        if (originMarkers != null) {
            for (Marker marker : originMarkers) {
                marker.remove();
            }
        }

        if (destinationMarkers != null) {
            for (Marker marker : destinationMarkers) {
                marker.remove();
            }
        }

        if (polylinePaths != null) {
            for (Polyline polyline:polylinePaths ) {
                polyline.remove();
            }
        }
    }

    @Override
    public void onDirectionFinderSuccess(List<Route> routes) {
        progressDialog.dismiss();
        polylinePaths = new ArrayList<>();
        originMarkers = new ArrayList<>();
        destinationMarkers = new ArrayList<>();

        for (Route route : routes) {
            mMap.moveCamera(CameraUpdateFactory.newLatLngZoom(route.startLocation, 16));
            ((TextView) findViewById(R.id.tvDuration)).setText(route.duration.text);
            ((TextView) findViewById(R.id.tvDistance)).setText(route.distance.text);

            originMarkers.add(mMap.addMarker(new MarkerOptions()
                    .icon(BitmapDescriptorFactory.fromResource(R.drawable.navstart))
                    .title(route.startAddress)
                    .position(route.startLocation)));
            destinationMarkers.add(mMap.addMarker(new MarkerOptions()
                    .icon(BitmapDescriptorFactory.fromResource(R.drawable.navend))
                    .title(route.endAddress)
                    .position(route.endLocation)));

            PolylineOptions polylineOptions = new PolylineOptions().
                    geodesic(true).
                    color(Color.BLUE).
                    width(10);

            for (int i = 0; i < route.points.size(); i++)
                polylineOptions.add(route.points.get(i));

            polylinePaths.add(mMap.addPolyline(polylineOptions));
        }
    }
}
