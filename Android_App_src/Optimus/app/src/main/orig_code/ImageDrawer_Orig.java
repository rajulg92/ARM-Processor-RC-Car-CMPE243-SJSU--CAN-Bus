package com.optimus.rccar;

/*import android.content.Context;
import android.graphics.Canvas;
import android.graphics.Color;
import android.support.annotation.Nullable;
import android.util.AttributeSet;
import android.view.View;*/

import processing.core.*;
/*import processing.data.*;
import processing.event.*;
import processing.opengl.*;
import java.lang.reflect.Method;*/

/*import static java.lang.Math.cos;
import static java.lang.Math.sin;
import static java.lang.Math.toRadians;*/

public class ImageDrawer extends PApplet {

    /*public ImageDrawer(Context context) {
        super(context);

        init(null);
    }

    public ImageDrawer(Context context, @Nullable AttributeSet attrs) {
        super(context, attrs);

        init(attrs);
    }

    public ImageDrawer(Context context, @Nullable AttributeSet attrs, int defStyleAttr) {
        super(context, attrs, defStyleAttr);
        init(attrs);
    }

    public ImageDrawer(Context context, @Nullable AttributeSet attrs, int defStyleAttr, int defStyleRes) {
        super(context, attrs, defStyleAttr, defStyleRes);
        init(attrs);
    }

    private void init(@Nullable AttributeSet set){

    }

    @Override
    protected void onDraw(Canvas canvas) {
        canvas.drawColor(Color.BLACK);
    }*/

    int angle =  - 80 ;
    int dir =  0 ;
    int distance;
    Radar miRadar;
    int MAX_DISTANCE = 200;
    PGraphics pg;

    public void setup(){
        orientation(LANDSCAPE);
        miRadar = new Radar(displayHeight, displayWidth);
        stroke(255);
    }

    public void sampleData(){
        pg = createGraphics(800, 600, JAVA2D);
        pg.background(0, 0, 0);

    }

    public void draw(){
        //sampleData();
        miRadar.dibRadar();
        miRadar.dibResource(angle);
        miRadar.dibFind(angle, distance);

    }

    public void setAngle_Distance(String[] values){
        angle = (int)map(Integer.parseInt(values[0]),0,160,-80,80);
        distance = (int)map(Integer.parseInt(values[1]), 1, MAX_DISTANCE, 1, miRadar.radio);
        println(values);
        println(angle + "," + distance);
    }

    // ------------------------------------------------ ----------------------
    //               Class Point
    // ------------------------------------------------ ----------------------
    class  Point{
        int x, y;
        Point(int xPos, int yPos){
            x = xPos;
            y = yPos;
        }

        public int getX(){
            return x;
        }
        public int getY(){
            return y;
        }
    }
    // ------------------------------------------------ ----------------------
    //               Class Radar
    // ------------------------------------------------ ----------------------

    class  Radar
    {
        int  SIDE_LENGTH ;
        int  ANGLE_BOUNDS  =  80 ;
        int  ANGLE_STEP  =  2 ;
        int  HISTORY_SIZE  =  12 ;
        int  POINTS_HISTORY_SIZE  =  100 ;

        int radio;
        float x;
        float y;
        float leftAnguloRad;
        float rightAnguloRad;
        float[] historyX, historyY;
        Point[] points;
        float centroX;
        float centroY;

        // *************************************************** *
        // Constructor
        // *************************************************** *
        Radar(int high , int wide)
        {
            SIDE_LENGTH = (high - 100) * 2;
            radio = SIDE_LENGTH  /  2;
            centroX = wide /  2;
            centroY = high;
            leftAnguloRad = radians(- ANGLE_BOUNDS); // -  HALF_PI ;
            rightAnguloRad = radians(ANGLE_BOUNDS);// -  HALF_PI ;
            historyX =  new float[HISTORY_SIZE];
            historyY =  new float[HISTORY_SIZE];
            points = new Point[POINTS_HISTORY_SIZE];
        }

        // *************************************************** *
        // All to draw radar background
        // *************************************************** *
        public void dibRadar()
        {
            stroke(100 );  // color of the lines
            noFill();
            strokeWeight(2 ); // width of the lines

            // draw the circles as distances references
            for(int i =  0 ; i <= (SIDE_LENGTH  /  100); i++) {
                arc(centroX, centroY,100  * i,100  * i, leftAnguloRad, rightAnguloRad);
            }

            // draw lines as angle reference
            for( int i =  0 ; i <= (ANGLE_BOUNDS * 2 / 20); i++ ) {
                float angle =  - ANGLE_BOUNDS + i *  20 ;
                float radAngulo = radians(angle);
                line(centroX, centroY,centroX + radio * sin(radAngulo),centroY - radio * cos(radAngulo));
            }

        }

        // *************************************************** (I.e.
        // Method to draw the lines that simulate the trace
        // *************************************************** (I.e.
        public void dibResource(float angle) {
            float radian = radians(angle);
            x = radio * sin(radian);
            y = radio * cos(radian);
            float px = centroX + x;
            float py = centroY - y;
            historyX[0] = px;
            historyY[0] = py;
            strokeWeight (2);
            for(int i = 0 ; i < HISTORY_SIZE ; i++ ) {
                stroke(50 ,190 ,50 ,255 - ( 25 * i));
                line(centroX, centroY, historyX[i], historyY[i]);
            }
            shiftHistoryArray ();
        }

        // *************************************************** (I.e.
        // Method to draw objects that are detected
        // *************************************************** (I.e.
        public void dibFind(float angle ,float distance) {

            if (distance > 0 ) {
                float radian = radians(angle);
                x = distance * sin(radian);
                y = distance * cos(radian);
                int px = ( int )(centroX + x);
                int py = ( int )(centroY - y);
                points[0] =  new Point(px, py);
            }
            else {
                points[0] =  new Point( 0 , 0 );
            }
            for(int i = 0 ; i < POINTS_HISTORY_SIZE ; i++ ) {
                Point point = points[i];
                if (point !=  null ) {
                    int x = point.x;
                    int y = point.y;
                    if (x == 0  && y == 0 )continue ;
                    int colorAlfa = (int)map(i,0 , POINTS_HISTORY_SIZE ,50 ,0 );
                    int size = (int)map(i,0 , POINTS_HISTORY_SIZE ,30 ,5 );
                    fill(190 ,40 ,40 , colorAlfa);
                    noStroke();
                    ellipse(x, y, size, size);
                }
            }
            shiftPointsArray();
        }

        public void shiftHistoryArray() {
            for(int i = HISTORY_SIZE ; i > 1 ; i--) {
                historyX[i - 1 ] = historyX[i - 2 ];
                historyY[i - 1 ] = historyY[i - 2 ];
            }
        }

        public void shiftPointsArray() {
            for(int i = POINTS_HISTORY_SIZE ; i > 1 ; i--) {
                Point oldPoint = points[i - 2];
                if(oldPoint != null){
                    Point point =  new Point(oldPoint.x, oldPoint.y);
                    points[i-1] = point;
                }
            }
        }

    }
}