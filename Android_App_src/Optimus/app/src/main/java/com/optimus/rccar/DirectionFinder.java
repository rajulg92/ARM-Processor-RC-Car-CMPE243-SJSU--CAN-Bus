package com.optimus.rccar;

import android.bluetooth.BluetoothSocket;
import android.os.AsyncTask;

import com.google.android.gms.maps.model.LatLng;

import org.json.JSONArray;
import org.json.JSONException;
import org.json.JSONObject;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.io.UnsupportedEncodingException;
import java.net.MalformedURLException;
import java.net.Proxy;
import java.net.URL;
import java.net.URLEncoder;
import java.util.ArrayList;
import java.util.List;

//import static com.optimus.rccar.optManControl.btSocket;
import static processing.core.PApplet.println;

/**
 * Created by Parimal on 10/22/2017.
 */

public class DirectionFinder {

    private static final String DIRECTION_URL_API = "https://maps.googleapis.com/maps/api/directions/json?";
    private static final String GOOGLE_API_KEY = "AIzaSyDQl4ILgoF4120YjAtgoeAkOAMmn-1KRQY";
    private DirectionFinderListener listener;
    private String origin;
    private String destination;
    private static double [][]LatLang_arr = new double[50][2];
    private static int chckptscount = 0;
    static BluetoothSocket btSocket_df = null;

    public DirectionFinder(DirectionFinderListener listener, String origin, String destination) {
        this.listener = listener;
        this.origin = origin;
        this.destination = destination;
    }

    public void execute() throws UnsupportedEncodingException {
        listener.onDirectionFinderStart();
        new DownloadRawData().execute(createUrl());
    }

    private String createUrl() throws UnsupportedEncodingException {
        String urlOrigin = URLEncoder.encode(origin, "utf-8");
        String urlDestination = URLEncoder.encode(destination, "utf-8");
        println("route>>origin:51:"+ urlOrigin);
        println("route>>destination:52:"+ urlDestination);
        return DIRECTION_URL_API + "origin=" + urlOrigin + "&destination=" + urlDestination + "&mode=walking" +"&key=" + GOOGLE_API_KEY;
    }

    private class DownloadRawData extends AsyncTask<String, Void, String> {

        @Override
        protected String doInBackground(String... params) {
            String link = params[0];
            try {
                URL url = new URL(link);
                println("route>>link:63:"+ link);
                InputStream is = url.openConnection(Proxy.NO_PROXY).getInputStream();
                StringBuffer buffer = new StringBuffer();
                BufferedReader reader = new BufferedReader(new InputStreamReader(is));

                String line;
                while ((line = reader.readLine()) != null) {
                    buffer.append(line + "\n");
                    //println("route>>line:71:"+ line);
                }

                return buffer.toString();

            } catch (MalformedURLException e) {
                e.printStackTrace();
            } catch (IOException e) {
                e.printStackTrace();
            }
            return null;
        }

        @Override
        protected void onPostExecute(String res) {
            try {
                parseJSon(res);
            } catch (JSONException e) {
                e.printStackTrace();
            }
        }
    }

    private void parseJSon(String data) throws JSONException {
        if (data == null)
            return;

        List<Route> routes = new ArrayList<Route>();
        JSONObject jsonData = new JSONObject(data);
        JSONArray jsonRoutes = jsonData.getJSONArray("routes");
        int jsonStepsLength = 0;
        for (int i = 0; i < jsonRoutes.length(); i++) {
            JSONObject jsonRoute = jsonRoutes.getJSONObject(i);
            Route route = new Route();

            JSONObject overview_polylineJson = jsonRoute.getJSONObject("overview_polyline");
            JSONArray jsonLegs = jsonRoute.getJSONArray("legs");
            JSONObject jsonLeg = jsonLegs.getJSONObject(0);

            //TODO: Add arrayoutofBounds check length=50; index=50
            //link:63:https://maps.googleapis.com/maps/api/directions/json?origin=sdd&destination=ddf&key=AIzaSyDQl4ILgoF4120YjAtgoeAkOAMmn-1KRQY
            //java.lang.ArrayIndexOutOfBoundsException: length=50; index=50
            //at com.optimus.rccar.DirectionFinder.parseJSon(DirectionFinder.java:130)
            //at com.optimus.rccar.DirectionFinder.access$100(DirectionFinder.java:31)
            //at com.optimus.rccar.DirectionFinder$DownloadRawData.onPostExecute(DirectionFinder.java:90)
            //at com.optimus.rccar.DirectionFinder$DownloadRawData.onPostExecute(DirectionFinder.java:59)

            JSONArray jsonSteps = jsonLeg.getJSONArray("steps");
            jsonStepsLength = jsonSteps.length();
            println("jsonSteps"+ jsonSteps.length());
            for (int k = 0; k < jsonSteps.length(); k++) {
                JSONObject jsonStep = jsonSteps.getJSONObject(k);
                JSONObject jsonStepEndLocation = jsonStep.getJSONObject("end_location");
                JSONObject jsonStepStartLocation = jsonStep.getJSONObject("start_location");
                route.startstepLocation = new LatLng(jsonStepStartLocation.getDouble("lat"), jsonStepStartLocation.getDouble("lng"));
                route.endstepLocation = new LatLng(jsonStepEndLocation.getDouble("lat"), jsonStepEndLocation.getDouble("lng"));

                if (k==(jsonSteps.length()-1)){
                    LatLang_arr[k][0] = route.endstepLocation.latitude;
                    LatLang_arr[k][1] = route.endstepLocation.longitude;
                    println("route>>startLat:127:"+ LatLang_arr[k][0]);
                    println("route>>startLong:128:"+ LatLang_arr[k][1]);
                }
                else{
                    LatLang_arr[k][0] = route.startstepLocation.latitude;
                    LatLang_arr[k][1] = route.startstepLocation.longitude;
                    println("route>>startLat:133:"+ LatLang_arr[k][0]);
                    println("route>>startLong:134:"+ LatLang_arr[k][1]);
                }
                chckptscount++;
                /*route.chckptscount++;
                route.chckptsLocation =  new LatLng(LatLang_arr[k][0], LatLang_arr[k][1]); //TODO Send the checkpoints to the marker on NavACtivity
                println("Checkpoints-Lat:"+ route.chckptsLocation.latitude);
                println("Checkpoints-Long:"+ route.chckptsLocation.longitude);*/
            }
            //println("Checkpoints-Count:"+ route.chckptscount);

            JSONObject jsonDistance = jsonLeg.getJSONObject("distance");
            JSONObject jsonDuration = jsonLeg.getJSONObject("duration");
            JSONObject jsonEndLocation = jsonLeg.getJSONObject("end_location");
            JSONObject jsonStartLocation = jsonLeg.getJSONObject("start_location");

            route.distance = new Distance(jsonDistance.getString("text"), jsonDistance.getInt("value"));
            route.duration = new Duration(jsonDuration.getString("text"), jsonDuration.getInt("value"));
            route.endAddress = jsonLeg.getString("end_address");
            route.startAddress = jsonLeg.getString("start_address");
            route.startLocation = new LatLng(jsonStartLocation.getDouble("lat"), jsonStartLocation.getDouble("lng"));
            route.endLocation = new LatLng(jsonEndLocation.getDouble("lat"), jsonEndLocation.getDouble("lng"));
            route.points = decodePolyLine(overview_polylineJson.getString("points"));

            println("route>>startLocation:122:"+ route.startLocation.latitude);
            println("route>>startLocation:123:"+ route.endLocation.longitude);
            /*if (i==0){
                LatLang_arr[i][0] = route.startLocation.latitude;
                LatLang_arr[i][1] = route.startLocation.longitude;
                println("route>>startLat:127:"+ LatLang_arr[i][0]);
                println("route>>startLong:128:"+ LatLang_arr[i][1]);
            }
            else{
                LatLang_arr[i][0] = route.endLocation.latitude;
                LatLang_arr[i][1] = route.endLocation.longitude;
                println("route>>startLat:133:"+ LatLang_arr[i][0]);
                println("route>>startLong:134:"+ LatLang_arr[i][1]);
            }*/
            routes.add(route);
        }
        for(int i = 0; i < jsonStepsLength; i++) {
            for (int j = 0; j < 2; j++) {
                println("LatLang_arr:134:" + LatLang_arr[i][j]);
            }
        }

        //public void sendCheckPoints(double[][] chck_pts_list){
            btSocket_df = optManControl.getbtsocket();

            String GeoCords[] = new String[50];
            String checkptlist = Integer.toString(chckptscount);
            for (int i = 0; i < LatLang_arr.length; i++) {
                String Lat = String.valueOf(LatLang_arr[i][0]);
                String Long = String.valueOf(LatLang_arr[i][1]);
                GeoCords[i] = Lat + "," + Long;
                println("geocords:202:"+ GeoCords[i]);
            }
            for(int i = 1; i < LatLang_arr.length; i++){
                checkptlist = checkptlist + ";" + GeoCords[i-1];
            }
            byte[] b = checkptlist.getBytes();
            //println("Bytes:208:"+ b);
            println("checkptlist:209:"+ checkptlist);
            if (btSocket_df != null) {
                println("BTsocket ! NULL");
                try {
                    println("Sending CheckPoints");
                    btSocket_df.getOutputStream().write(b);
                } catch (IOException e) {
                    println("CheckPoints Transmit Error");
                }
            } else {
                println("BTsocket NULL");
            }
        //}
        //new optManControl().sendCheckPoints(LatLang_arr);
        //listener.onDirectionFinderSuccess(routes);
        println("Checkpoints-Count:"+ chckptscount);
        listener.onDirectionFinderSuccess(routes, LatLang_arr, chckptscount);
        chckptscount = 0;
    }

    private List<LatLng> decodePolyLine(final String poly) {
        int len = poly.length();
        int index = 0;
        List<LatLng> decoded = new ArrayList<LatLng>();
        int lat = 0;
        int lng = 0;

        while (index < len) {
            int b;
            int shift = 0;
            int result = 0;
            do {
                b = poly.charAt(index++) - 63;
                result |= (b & 0x1f) << shift;
                shift += 5;
            } while (b >= 0x20);
            int dlat = ((result & 1) != 0 ? ~(result >> 1) : (result >> 1));
            lat += dlat;

            shift = 0;
            result = 0;
            do {
                b = poly.charAt(index++) - 63;
                result |= (b & 0x1f) << shift;
                shift += 5;
            } while (b >= 0x20);
            int dlng = ((result & 1) != 0 ? ~(result >> 1) : (result >> 1));
            lng += dlng;

            decoded.add(new LatLng(
                    lat / 100000d, lng / 100000d
            ));
        }

        return decoded;
    }

    public static BluetoothSocket getbtsocket(){
        return btSocket_df;
    }
}
