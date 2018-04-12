package com.optimus.rccar;

import com.google.android.gms.maps.model.LatLng;

import java.util.ArrayList;
import java.util.List;
import com.optimus.rccar.Route;

/**
 * Created by Parimal on 10/22/2017.
 */

public interface DirectionFinderListener {
    void onDirectionFinderStart();
    void onDirectionFinderSuccess(List<Route> route, double [][]LatLang_arr, int chckptscount);
}
