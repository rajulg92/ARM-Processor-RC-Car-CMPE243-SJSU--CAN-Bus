package com.optimus.rccar;

/**
 * Created by gotham on 03/12/17.
 */

import android.content.Context;
import android.content.res.Configuration;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Paint;
import android.graphics.Paint.Style;
import android.graphics.Point;
import android.graphics.RectF;
import android.util.AttributeSet;
import android.util.Log;
import android.view.View;

class SectorInfo
{
    Point start;
    Point end;

    SectorInfo(){
        start = new Point();
        end = new Point();
        Log.v("SectorInfo", "new sec");
    }
//    SectorInfo(int axStart,
//            int ayStart,
//            int axEnd,
//            int ayEnd)
//    {
//        xStart = axStart;
//        yStart = ayStart;
//        xEnd = axEnd;
//        yEnd = ayEnd;
//    }
}

public class LidarMap extends View {
    private int mWidth;
    private int mHeight;

    Paint paint = null;
    Canvas canvas = null;
    int nWidth = 0;
    int nHeight = 0;

    int nCanvasStartX;
    int nCanvasStartY;

    boolean bLandscape = false;
    SectorInfo sectors[];

    final double SECTOR0_BOUNDARY = 20.0;
    final double SECTOR1_BOUNDARY = (SECTOR0_BOUNDARY + 25.0);
    final double SECTOR2_BOUNDARY = (SECTOR1_BOUNDARY + 15.0);
    final double SECTOR3_START    = (90);
    final double SECTOR3_BOUNDARY = (SECTOR3_START + 30);
    final double SECTOR4_BOUNDARY = (SECTOR3_BOUNDARY + 30);
    final double SECTOR5_BOUNDARY = (SECTOR4_BOUNDARY + 30);
    final double SECTOR6_BOUNDARY = (SECTOR5_BOUNDARY + 30);
    final double SECTOR7_BOUNDARY = (SECTOR6_BOUNDARY + 30);
    final double SECTOR8_BOUNDARY = (SECTOR7_BOUNDARY + 30);
    final double SECTOR9_START    = (300);
    final double SECTOR9_BOUNDARY = (SECTOR9_START + 15);
    final double SECTOR10_BOUNDARY= (SECTOR9_BOUNDARY + 25);
    final double SECTOR11_BOUNDARY= (SECTOR10_BOUNDARY + 25);

    double[] sectorBoundaryAngles;
    int nLSeparation = 0;

    String TAG = "LidarMap";

    int nCX = 0, nCY = 0, nRadius = 0;

    int[] mobstacle_info_track_vals;

    public LidarMap(Context context) {
        super(context);

    }

    public LidarMap(Context context, AttributeSet attribs) {
        super(context, attribs);

        mobstacle_info_track_vals = new int[12];
    }

    public void drawArcAlongCircumference(int anArcRectStartingX, int anArcRectStartingY,
                    int anArcRectEndingX, int anArcRectEndingY,
            float afArcStartingAngle, float afArcEndingAngle)
    {
        getOrientationScreen();
        //paint.setColor(Color.rgb(250, 64, 129));
        paint.setColor(Color.GREEN);
        paint.setStrokeWidth(2);
        paint.setStyle(Paint.Style.FILL);
        final RectF oval = new RectF();
        paint.setStyle(Paint.Style.STROKE);
        oval.set(anArcRectStartingX, anArcRectStartingY, anArcRectEndingX, anArcRectEndingY);
        canvas.drawArc(oval, afArcStartingAngle, afArcEndingAngle, true, paint);
    }

    public void drawCirclyByArcsAroundCircumference(int afCX, int afCY, int anRadius)
    {
        /*
        drawArcAlongCircumference((int)(afCX - anRadius), (int)(afCY),
                (int)(afCX - anRadius), (int)(afCY),
                0, 360);
                */
//        Paint paint2 = new Paint(Color.GREEN);
//        paint2.setStrokeWidth(10);
//        canvas.drawPoint(afCX, afCY, paint2);
//        canvas.drawLine(afCX, afCX, mWidth, mHeight, paint);
        drawArcAlongCircumference((int)(afCX - anRadius), (int)(afCY - anRadius),
                (int)(afCX + anRadius), (int)(afCY + anRadius),
                0, 360);

//        drawArcAlongCircumference((int)(afCX - anRadius), (int)(20),
//                (int)(afCX + anRadius), (int)(600),
//                270, 360);
    }

    void getOrientationScreen()
    {
        if(getResources().getConfiguration().orientation == Configuration.ORIENTATION_LANDSCAPE){
            bLandscape = true;
        }
        else
        {
            bLandscape = false;
        }
    }

    int getX(double theta, int anRadius)
    {
        return (int)(Math.cos(theta) * (double)anRadius);
    }

    int getY(double theta, int anRadius)
    {
        return (int)(Math.sin(theta) * (double)anRadius);
    }

    void findEndForPair(int anCX, int anCY, int anRadius, int anRightIdx, int anLeftIdx, double aTheta, boolean bAbove)
    {
        sectors[anRightIdx].end.x = anCX + getX(aTheta, anRadius);
        if(bAbove)
            sectors[anRightIdx].end.y = anCY - getY(aTheta, anRadius);
        else
            sectors[anRightIdx].end.y = anCY + getY(aTheta, anRadius);
        sectors[anLeftIdx].end.x = anCX - getX(aTheta, anRadius);
        sectors[anLeftIdx].end.y = sectors[anRightIdx].end.y;
    }

    void computeSectors(int anCX, int anCY, int anRadius)
    {

        sectorBoundaryAngles = new double[12];
        sectorBoundaryAngles[0] = SECTOR0_BOUNDARY;
        sectorBoundaryAngles[1] = SECTOR1_BOUNDARY;
        sectorBoundaryAngles[2] = SECTOR2_BOUNDARY;
        sectorBoundaryAngles[3] = SECTOR3_BOUNDARY;
        sectorBoundaryAngles[4] = SECTOR4_BOUNDARY;
        sectorBoundaryAngles[5] = SECTOR5_BOUNDARY;
        sectorBoundaryAngles[6] = SECTOR6_BOUNDARY;
        sectorBoundaryAngles[7] = SECTOR7_BOUNDARY;
        sectorBoundaryAngles[8] = SECTOR8_BOUNDARY;
        sectorBoundaryAngles[9] = SECTOR9_BOUNDARY;
        sectorBoundaryAngles[10] = SECTOR10_BOUNDARY;
        sectorBoundaryAngles[11] = SECTOR11_BOUNDARY;

        sectors = new SectorInfo[12];
        for(int i = 0; i < sectors.length; i++)
        {
            sectors[i] = new SectorInfo();
        }

        /** end calc: */
        findEndForPair(anCX, anCY, anRadius, 0, 10, Math.toRadians(90 -SECTOR0_BOUNDARY), true);
        findEndForPair(anCX, anCY, anRadius, 1, 9, Math.toRadians(90 -SECTOR1_BOUNDARY), true);
        findEndForPair(anCX, anCY, anRadius, 2, 8, Math.toRadians(90 -SECTOR2_BOUNDARY), true);
        sectors[9].start = new Point(sectors[8].end);
        sectors[8].end.x = anCX - anRadius;
        sectors[8].end.y = anCY;


        findEndForPair(anCX, anCY, anRadius, 3, 7, Math.toRadians(SECTOR3_BOUNDARY - 90), false);
        findEndForPair(anCX, anCY, anRadius, 4, 6, Math.toRadians(SECTOR4_BOUNDARY - 90), false);
        sectors[5].end.x = anCX;
        sectors[5].end.y = anCY + anRadius;

        /** start calc: */
        sectors[0].start.x = anCX;
        sectors[0].start.y = anCY - anRadius;
        sectors[11].end = sectors[0].start;

        sectors[1].start = sectors[0].end;
        sectors[2].start = sectors[1].end;
        sectors[3].start.x = anCX + anRadius;
        sectors[3].start.y = anCY;
        sectors[4].start = sectors[3].end;
        sectors[5].start = sectors[4].end;
        sectors[6].start = sectors[5].end;
        sectors[7].start = sectors[6].end;
        sectors[8].start = sectors[7].end;
        sectors[10].start = sectors[9].end;
        sectors[11].start = sectors[10].end;


    }

    void drawSectors()
    {
        int nLRadiusTmp = 0;



        if(bLandscape) {
            nCX = nWidth / 2 + nWidth / 4;
            nCY = nHeight / 2;
        }
        else {
            nCX =nWidth / 2;
            nCY = nHeight / 2 + nHeight / 4;
        }
        nRadius = (nWidth /4);
        nLSeparation = nRadius / 12;

        computeSectors(nCX, nCY, nRadius);


        for(int i = 1; i <= 12; i++) {

            nLRadiusTmp += nLSeparation;

            drawCirclyByArcsAroundCircumference(nCX, nCY, nLRadiusTmp);

        }
        for(int i = 0; i < 12; i++) {
            Log.v(TAG, "sector" + i + " start[" + sectors[i].start + "] " + "end[" + sectors[i].end + "]");
            paint.setColor(Color.GREEN);
            Paint paintLocalStart = new Paint();
            paintLocalStart.setColor(Color.GREEN);
            canvas.drawLine(sectors[i].start.x, sectors[i].start.y, nCX, nCY, paintLocalStart);
            canvas.drawLine(sectors[i].end.x, sectors[i].end.y, nCX, nCY, paint);
        }
    }

    void clearCanvasAndRedrawSectors()
    {
        //clear canvas and redraw
        canvas.drawColor(Color.BLACK);
        drawSectors();
    }

    void reportObstacle(int[] obstacle_info_track_vals)
    {
        mobstacle_info_track_vals = obstacle_info_track_vals;
        postInvalidate();
    }

    void drawObstacle(int sector, int track)
    {
        Point pointBoundary = sectors[sector].start;
        int x2x1 = pointBoundary.x - nCX;
        int y2y1 = pointBoundary.y - nCY;
        double fDivFactor = nRadius / 12;
        double xTxt = 0, yTxt = 0;

        if(track == 0 || track > 12)
            return;

        xTxt = (fDivFactor * track);
        yTxt = (fDivFactor * track);




        Log.v(TAG, "track=" + track);
        int nDistAtWhichToDraw = (track * nLSeparation) - nLSeparation/2;
        //xTxt = getX(sectorBoundaryAngles[sector], nDistAtWhichToDraw);
        //yTxt = getY(sectorBoundaryAngles[sector], nDistAtWhichToDraw);


        /*
         * point is at nDistAtWhichToDraw on the line between (nCX, nCY) and sectors[sector].start
         *
         */

        double d = Math.sqrt(x2x1 * x2x1 + y2y1 * y2y1);
        double dt = nDistAtWhichToDraw;
        double t = dt/d;

        xTxt = (int)(((1.0 - t) * nCX) + (t * pointBoundary.x));
        yTxt = (int)(((1.0 - t) * nCY) + (t * pointBoundary.y));

//        if(x2x1 >= 0)
//            xTxt += nCX;
//        else
//            xTxt = nCX - xTxt;
//
//        if(y2y1 >= 0)
//            yTxt += nCY;
//        else
//            yTxt = nCY - yTxt;
        Paint paintText = new Paint();
        paintText.setColor(Color.WHITE);
        paintText.setTextAlign(Paint.Align.CENTER);
        paintText.setTextSize(25.0f);
        canvas.drawText(""+track, (float)xTxt  , (float)yTxt, paintText);
        //canvas.drawText("" + sector, (float)pointBoundary.x  , (float)pointBoundary.y, paintText);
    }

    @Override
    protected void onDraw(Canvas aCanvas) {
        canvas = aCanvas;
        super.onDraw(canvas);
        getOrientationScreen();

        /*
        nWidth = canvas.getWidth();
        nHeight = canvas.getHeight();*/
        Log.v(TAG, "[" + mWidth + "X" + mHeight + "]  canvas:[" + canvas.getWidth() + "X" + canvas.getHeight() + "]");
        nWidth = mWidth;
        nHeight =  mHeight;

        nCanvasStartY = mHeight/2;

        paint = new Paint();
        //paint.setColor(Color.GRAY);
        //paint.setStyle(Style.FILL);
        //canvas.drawPaint(paint);

        paint.setColor(Color.WHITE);

//        canvas.drawLine(0, 0, mWidth, mHeight, paint);
//        canvas.drawLine(mWidth, 0, 0, mHeight, paint);
//
//        paint.setColor(Color.RED);
        //canvas.drawText("0 kal", 50, 85, paint);
        //canvas.save();

        //canvas.drawCircle(nWidth/2, nCanvasStartY + nHeight/2, nWidth/8, paint);

        drawSectors();

        for(int i = 0; i < 12; i++)
        {
            drawObstacle(i, mobstacle_info_track_vals[i]);
        }
    }

    @Override
    protected void onMeasure(int widthMeasureSpec, int heightMeasureSpec) {
        mWidth = View.MeasureSpec.getSize(widthMeasureSpec);
        mHeight = View.MeasureSpec.getSize(heightMeasureSpec);
        setMeasuredDimension(mWidth, mHeight);
    }
}
