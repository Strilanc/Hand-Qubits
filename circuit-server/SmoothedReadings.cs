public class SmoothedReadings {
    public double gyratingness = 0;
    public RawReadings calibration = new RawReadings();
    private int stableReadings = 0;

    public bool isStable() {
        return gyratingness < 40;
    }

    public bool isResting() {
        if (stableReadings < 100) {
            return gyratingness < 1;
        }
        return gyratingness < 0.5;
    }

    public RawReadings update(RawReadings readings) {
        gyratingness *= 0.7;
        gyratingness += (readings.gyro - calibration.gyro).Length;

        double a = 0;
        double b = 1;
        if (isResting()) {
            stableReadings += 1;
            a = 0.1;
            b = 0.9;
        } else if (gyratingness < 10) {
            a = 0.0001;
            b = 0.9999;
        }
        calibration *= b;
        calibration += readings * a;

        if (isResting()) {
            return readings.justAccel();
        }
        return readings - calibration.justGyro();
    }
}
