package com.optimus.rccar;

import android.Manifest;
import android.app.Activity;
import android.app.ProgressDialog;
import android.content.Context;
import android.content.Intent;
import android.content.pm.PackageManager;
import android.graphics.Color;
import android.graphics.Typeface;
import android.location.Address;
import android.location.Criteria;
import android.location.Geocoder;
import android.location.Location;
//import android.location.LocationManager;
import android.location.LocationManager;
import android.os.Build;
import android.os.Handler;
import android.support.annotation.NonNull;
import android.support.v4.app.ActivityCompat;
import android.support.v4.app.FragmentActivity;
import android.os.Bundle;
import android.support.v4.content.ContextCompat;
import android.view.View;
import android.widget.Button;
import android.widget.EditText;
import android.widget.TextView;
import android.widget.Toast;

import com.google.android.gms.location.LocationRequest;
import com.google.android.gms.location.LocationServices;
import com.google.android.gms.maps.CameraUpdateFactory;
import com.google.android.gms.maps.GoogleMap;
import com.google.android.gms.common.api.GoogleApiClient;
import com.google.android.gms.maps.OnMapReadyCallback;
import com.google.android.gms.maps.SupportMapFragment;
import com.google.android.gms.location.LocationListener;
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

public class optimusNavActivity extends FragmentActivity implements OnMapReadyCallback, LocationListener, DirectionFinderListener, GoogleMap.OnMarkerClickListener, GoogleMap.OnMarkerDragListener{

    static boolean isNavChckPtSet = false;  // TODO: It is set to TRUE for default here. Need to set it according to when only NAV pt is set else returns FALSE.
    private GoogleMap mMap;
    private GPSTracker gpsTracker;
    private Location mlocation;
    private GoogleApiClient client;
    private Location lastlocation;
    private Marker currlocationmarker;
    private Marker defaultmarker;
    public static final int REQUEST_LOCATION_CODE = 99;
    double end_latitude, end_longitude, latitude, longitude;
    static double gmap_latitude, gmap_longitude;
    static boolean isGPSLatLngSet = false;
    private Button btnFindPath, btn_dashboard;
    //private EditText etOrigin;
    //private EditText etDestination;
    private List<Marker> originMarkers = new ArrayList<>();
    private List<Marker> destinationMarkers = new ArrayList<>();
    private List<Marker> checkpointMarkers = new ArrayList<>();
    private List<Polyline> polylinePaths = new ArrayList<>();
    private ProgressDialog progressDialog;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_optimus__nav_);

        /*if(Build.VERSION.SDK_INT >= Build.VERSION_CODES.M){
            checkLocationPermission();
        }*/

        btnFindPath = (Button) findViewById(R.id.btnFindPath);
        //btn_dashboard = (Button) findViewById(R.id.btn_dashboard);
        TextView tvdi = (TextView)findViewById(R.id.tvDistance);
        TextView tvdu = (TextView)findViewById(R.id.tvDuration);
        Typeface custom_font = Typeface.createFromAsset(getAssets(),  "fonts/Vollkorn-Semibold.ttf");
        tvdi.setTypeface(custom_font);
        tvdu.setTypeface(custom_font);
        btnFindPath.setTypeface(custom_font);

        //etOrigin = (EditText) findViewById(R.id.etOrigin);
        //etDestination = (EditText) findViewById(R.id.etDestination);

        btnFindPath.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                mMap.clear();
                /*MarkerOptions markerOptions = new MarkerOptions();
                markerOptions.position(new LatLng(end_latitude, end_longitude));
                //markerOptions.draggable(true);
                //markerOptions.icon(BitmapDescriptorFactory.fromResource(R.drawable.nav6));
                mMap.addMarker(markerOptions);*/
                sendRequest();
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

    /*@Override
    public void onRequestPermissionsResult(int requestCode, @NonNull String[] permissions, @NonNull int[] grantResults) {
        switch (requestCode){
            case REQUEST_LOCATION_CODE:
                if(grantResults.length > 0 && grantResults[0] == PackageManager.PERMISSION_GRANTED){
                    //permission is granted
                    if(ContextCompat.checkSelfPermission(this, Manifest.permission.ACCESS_FINE_LOCATION) == PackageManager.PERMISSION_GRANTED){
                        if(client == null){
                            buildGoogleApiClient();
                        }
                        mMap.setMyLocationEnabled(true);
                    }
                }
                else{
                    Toast.makeText(this, "Permission Denied!", Toast.LENGTH_LONG).show();
                }
                return;
        }
        //super.onRequestPermissionsResult(requestCode, permissions, grantResults);
    }*/

    private void sendRequest() {
        isNavChckPtSet = true;
        //String origin = etOrigin.getText().toString();
        //origin = "37.3360008,-121.8847221";
        //String destination = etDestination.getText().toString();
        //destination = "37.4032078,-121.9720297";

        String origin =Double.toString(gmap_latitude)+","+Double.toString(gmap_longitude);
        String destination =Double.toString(end_latitude)+","+Double.toString(end_longitude);
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
            isNavChckPtSet = false;
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
        //LatLng mylocation = new LatLng(latitude, longitude);
        //Instantiate the class LatLng
        if (ContextCompat.checkSelfPermission(this, Manifest.permission.ACCESS_FINE_LOCATION)
                == PackageManager.PERMISSION_GRANTED) {
           // buildGoogleApiClient();
            mMap.setMyLocationEnabled(true);
            //TODO: TAKE THE DATA FROM OPTIMUS OF CURRENT GPS LOCATION OF CAR
//            if(!isGPSLatLngSet){
//                Toast.makeText(optimusNavActivity.this, "CAR - GPS coords, not received,\nsetting DEFAULT coords", Toast.LENGTH_LONG).show();
//                gmap_latitude = 37.336759;
//                gmap_longitude = -121.880982;
//            }else {
//                isGPSLatLngSet = false;
//                println("Using acquired GPS Coords::203:: Lat:" + gmap_latitude +"Long:" + gmap_longitude);
//            }
            //TODO; test commented
            LocationManager locationManager = (LocationManager)
                    getSystemService(Context.LOCATION_SERVICE);
            Criteria criteria = new Criteria();

            Location location = locationManager.getLastKnownLocation(locationManager
                    .getBestProvider(criteria, false));
            gmap_latitude = location.getLatitude();
            gmap_longitude = location.getLongitude();
            LatLng mylocation = new LatLng(gmap_latitude, gmap_longitude);
            MarkerOptions marker =  new MarkerOptions().position(mylocation);
            marker.icon(BitmapDescriptorFactory.fromResource(R.drawable.nav6));
            defaultmarker = mMap.addMarker(marker);
            mMap.setMapType(GoogleMap.MAP_TYPE_HYBRID);
            mMap.moveCamera(CameraUpdateFactory.newLatLngZoom(mylocation, 18.0f));

        }
        //Instantiate the class GeoCoder
        //Geocoder geocoder = new Geocoder(getApplicationContext());
        mMap.setOnMarkerDragListener(this);
        mMap.setOnMarkerClickListener(this);
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
    public void onDirectionFinderSuccess(List<Route> routes, double [][]LatLang_arr, int chckptscount) {
        progressDialog.dismiss();
        polylinePaths = new ArrayList<>();
        originMarkers = new ArrayList<>();
        destinationMarkers = new ArrayList<>();
        //MarkerOptions options = new MarkerOptions();
        ArrayList<LatLng> latlngs = new ArrayList<>();

        /*for(int i=0; i<route.chckptscount; i++) {
            checkpointMarkers.add(mMap.addMarker(new MarkerOptions()
                    .icon(BitmapDescriptorFactory.defaultMarker(BitmapDescriptorFactory.HUE_BLUE))
                    .position(route.chckptsLocation)));
        }*/

        defaultmarker.remove();
        mMap.clear();
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

            PolylineOptions polylineOptions = new PolylineOptions()
                    .geodesic(true)
                    .color(Color.BLUE)
                    .width(10);

            for (int i = 0; i < route.points.size(); i++)
                polylineOptions.add(route.points.get(i));

            polylinePaths.add(mMap.addPolyline(polylineOptions));
        }

        for(int i=1; i<(chckptscount-1); i++) {
            //for (int j = 0; j < 2; j++) {
                latlngs.add(new LatLng(LatLang_arr[i][0], LatLang_arr[i][1]));
                /*mMap.addMarker(new MarkerOptions()
                        .anchor(0.5f, 0.5f)
                        .icon(BitmapDescriptorFactory.fromResource(R.drawable.nav6))
                        .position(new LatLng(LatLang_arr[i][j], LatLang_arr[i][j])));*/
                println("LatLang_arr_lat:289:" + LatLang_arr[i][0] + "LatLang_arr_long:289:" + LatLang_arr[i][1]);
           // }
        }
        for (LatLng point : latlngs) {
            mMap.addMarker(new MarkerOptions()
                    //.icon(BitmapDescriptorFactory.fromResource(R.drawable.nav6))
                    .icon(BitmapDescriptorFactory.defaultMarker(BitmapDescriptorFactory.HUE_BLUE))
                    .position(point));
        }
    }

    protected synchronized void buildGoogleApiClient(){
        client = new GoogleApiClient.Builder(this)
                .addApi(LocationServices.API)
                .build();
        client.connect();
    }

	@Override
    public void onLocationChanged(Location location) {
        lastlocation = location;

        if(currlocationmarker != null){
            currlocationmarker.remove();
        }
        Toast.makeText(optimusNavActivity.this, "Last Loc Latitude:"+lastlocation.getLatitude()+"Last Loc Longitude:"+lastlocation.getLongitude(), Toast.LENGTH_LONG).show();
        LatLng latLng = new LatLng(location.getLatitude(), location.getLongitude());
        MarkerOptions marker =  new MarkerOptions().position(latLng).title("My GPS"/*plc_holder_str*/);
                //.icon(BitmapDescriptorFactory.defaultMarker(BitmapDescriptorFactory.HUE_BLUE));
                //.icon(BitmapDescriptorFactory.fromResource(R.drawable.nav6));

        currlocationmarker = mMap.addMarker(marker);
        mMap.moveCamera(CameraUpdateFactory.newLatLngZoom(latLng, 2.0f));
    }

    public boolean checkLocationPermission(){
        if(ContextCompat.checkSelfPermission(this,Manifest.permission.ACCESS_FINE_LOCATION) != PackageManager.PERMISSION_GRANTED){
            if(ActivityCompat.shouldShowRequestPermissionRationale(this, Manifest.permission.ACCESS_FINE_LOCATION)){
                ActivityCompat.requestPermissions(this, new String[]{Manifest.permission.ACCESS_FINE_LOCATION}, REQUEST_LOCATION_CODE);
            }
            else{
                ActivityCompat.requestPermissions(this, new String[]{Manifest.permission.ACCESS_FINE_LOCATION}, REQUEST_LOCATION_CODE);
            }
            return false;
        }
        else
            return true;
    }

    @Override
    public boolean onMarkerClick(Marker marker) {
        marker.setDraggable(true);
        return false;
    }

    @Override
    public void onMarkerDragStart(Marker marker) {

    }

    @Override
    public void onMarkerDrag(Marker marker) {

    }

    @Override
    public void onMarkerDragEnd(Marker marker) {
        end_latitude = marker.getPosition().latitude;
        end_longitude = marker.getPosition().longitude;
    }

    public static void setOptimusGPSLatLng(double car_latitude, double car_longitude){
        isGPSLatLngSet = true;
        gmap_latitude = car_latitude;
        gmap_longitude = car_longitude;
    }

    public void nav_btdisconnect(){
        finish();
    }
}
