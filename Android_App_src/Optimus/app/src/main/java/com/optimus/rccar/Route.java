package com.optimus.rccar;

import com.google.android.gms.maps.model.LatLng;
import java.util.List;


/**
 * Created by Parimal on 10/22/2017.
 */

public class Route {
    public Distance distance;
    public Duration duration;
    public String endAddress;
    public LatLng endLocation;
    public LatLng endstepLocation;
    public String startAddress;
    public LatLng startLocation;
    public LatLng startstepLocation;
    //public LatLng chckptsLocation;
    //public int chckptscount;
    public List<LatLng> points;
}
