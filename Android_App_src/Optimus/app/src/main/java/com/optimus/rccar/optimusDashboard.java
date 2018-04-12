package com.optimus.rccar;

import android.app.Activity;
import android.os.Bundle;
import android.support.annotation.NonNull;
import android.support.design.widget.NavigationView;
import android.support.v4.view.GravityCompat;
import android.support.v4.widget.DrawerLayout;
import android.support.v7.app.ActionBar;
import android.support.v7.app.ActionBarDrawerToggle;
import android.support.v7.app.AppCompatActivity;
import android.util.Log;
import android.view.MenuItem;
import android.widget.TextView;
import android.widget.Toast;

public class optimusDashboard extends AppCompatActivity implements NavigationView.OnNavigationItemSelectedListener{

    private DrawerLayout mdrawerlayout;
    private ActionBarDrawerToggle mToggle;

    private TextView mDebugText;
    private int mnCount = 0;
    final String TAG = "Optimus Dashboard";
    public static String msDebugInfo = "Awaiting Debug Info";
    //ImageDrawer imageDrawer = new ImageDrawer();//TODO: Check which class object usage works!!!
    //optManControl optManControl = new optManControl();

    static Activity myInstance = null;

    static Activity getDashBoardActivity()
    {
        return myInstance;
    }

    private void setNavigationViewListner() {
        NavigationView navigationView = (NavigationView) findViewById(R.id.nav_view);
        navigationView.setNavigationItemSelectedListener(this);
    }

    public static void setMsDebugInfo(String aNewDebugInfoString)
    {
        msDebugInfo = aNewDebugInfoString;
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_optimus_dashboard);
        mdrawerlayout = (DrawerLayout) findViewById(R.id.dashboard);
        mToggle = new ActionBarDrawerToggle(this, mdrawerlayout, R.string.navigation_drawer_open, R.string.navigation_drawer_close);
        mdrawerlayout.addDrawerListener(mToggle);
        mToggle.syncState();
        getSupportActionBar().setDisplayHomeAsUpEnabled(true);
        setNavigationViewListner();

        myInstance = this;

        mDebugText = findViewById(R.id.debug_text);
        new Thread(new Runnable() {
            public void run() {
                while(true) {
                    try {
                        Thread.sleep(1000);
                        mnCount++;
                        runOnUiThread(new Runnable() {
                           @Override
                            public void run() {
                               mDebugText.setText(msDebugInfo);//"hello" + mnCount
                            }
                        });
                        Log.v(TAG, "updated Text " + mnCount);
                    } catch (InterruptedException e) {
                        Log.e(TAG, "exception " + e);
                    }
                }
            }
        }).start();
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem menuItem){

        if(mToggle.onOptionsItemSelected(menuItem)){
            return true;
        }
        return super.onOptionsItemSelected(menuItem);
    }

    @Override
    public boolean onNavigationItemSelected(@NonNull MenuItem item) {
        // Handle navigation view item clicks here.
        switch (item.getItemId()) {

            case R.id.sensor: {
                //imageDrawer.sampleData();//TODO: Check which class object usage works!!!
                ImageDrawer imageDrawer = new ImageDrawer();
                imageDrawer.sampleData();
                Toast.makeText(optimusDashboard.this, "Switching to Lidar-Sensor Display!", Toast.LENGTH_LONG).show();
                //TODO: Do something
                break;
            }
            case R.id.motor: {
                Toast.makeText(optimusDashboard.this, "Switching to motor Activity: Currently Not in Service!", Toast.LENGTH_LONG).show();
                //TODO: Do something
                break;
            }
            case R.id.compass: {
                Toast.makeText(optimusDashboard.this, "Switching to compass Activity: Currently Not in Service!", Toast.LENGTH_LONG).show();
                //TODO: Do something
                break;
            }
            case R.id.gps: {
                Toast.makeText(optimusDashboard.this, "Switching to gps Activity: Currently Not in Service!", Toast.LENGTH_LONG).show();
                //TODO: Do something
                break;
            }
            case R.id.speedsettings: {
                Toast.makeText(optimusDashboard.this, "Switching to speedsettings Activity: Currently Not in Service!", Toast.LENGTH_LONG).show();
                //TODO: Do something
                break;
            }
        }
        //close navigation drawer
        mdrawerlayout.closeDrawer(GravityCompat.START);
        return true;
    }

    public void dashboard_btdisconnect(){
        finish();
    }
}
