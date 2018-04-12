/**
 *
 * Interface for all Gps implementations.
 *
 * @author Dalmir da Silva <dalmirdasilva@gmail.com>
 */

class Gps {
public:

    enum FixType {
        NO_FIX = 0,
        FIX_2D = 2,
        FIX_3D = 3,
        FIX_DGPS = 4
    };

    struct GpsInfo {
        double latitude;
        double longitude;
        double altitude;
        double speed;
        FixType fixType;
        unsigned char satellites;
        double courseOverGround;
        double verticalSpeedIndicator;
        double horizontalDilutionOfPrecision;
        double verticalDilutionOfPrecision;
        unsigned char year;
        unsigned char month;
        unsigned char day;
        unsigned char hour;
        unsigned char minute;
        unsigned char second;
        unsigned char isLocked;
    };

    virtual ~Gps();

    void fillGpsInfo(GpsInfo* info);

    /**
     * Gets the latitude.
     */
    virtual double getLatitude() = 0;

    /**
     * Gets the longitude.
     */
    virtual double getLongitude() = 0;

    /**
     * Gets the altitude.
     */
    virtual double getAltitude() = 0;

    /**
     * Gets the speed.
     */
    virtual double getSpeed() = 0;

    /**
     * Gets the fix type.
     */
    virtual FixType getFixType() = 0;

    /**
     * Gets the number of satellites.
     */
    virtual unsigned char getSatellites() = 0;

    /**
     * Gets the course over ground.
     */
    virtual double getCourseOverGround() = 0;

    /**
     * Gets the vertical speed indicator.
     */
    virtual double getVerticalSpeedIndicator() = 0;

    /**
     * Gets the horizontal dilution of precision.
     */
    virtual double getHorizontalDilutionOfPrecision() = 0;

    /**
     * Gets the vertical dilution of precision.
     */
    virtual double getVerticalDilutionOfPrecision() = 0;

    /**
     * Gets the year.
     */
    virtual unsigned char getYear() = 0;

    /**
     * Gets the month.
     */
    virtual unsigned char getMonth() = 0;

    /**
     * Gets the day.
     */
    virtual unsigned char getDay() = 0;

    /**
     * Gets the hour.
     */
    virtual unsigned char getHour() = 0;

    /**
     * Gets the minute.
     */
    virtual unsigned char getMinute() = 0;

    /**
     * Gets the second.
     */
    virtual unsigned char getSecond() = 0;

    /**
     * Gets locked information.
     */
    virtual unsigned char isLocked() = 0;
};
