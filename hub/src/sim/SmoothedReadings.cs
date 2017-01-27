public class SmoothedReadings {
    public double gyratingness = 0;
    public RawReadings calibration = new RawReadings();
    private int stableReadings = 0;

    public bool isStable() {
        return gyratingness < 4;
    }

    public bool isResting() {
        if (stableReadings < 1000) {
            return gyratingness < 0.1;
        }
        return gyratingness < 0.005;
    }

    public RawReadings update(RawReadings readings) {
        gyratingness *= 0.7;
        gyratingness += (readings.gyro - calibration.gyro).Length;

        double a = 0;
        double b = 1;
        if (isResting()) {
            stableReadings += 1;
            a = 0.004;
            b = 0.996;
        } else if (gyratingness < 1) {
            a = 0.000001;
            b = 0.999999;
        }
        calibration *= b;
        calibration += readings * a;

        if (isResting()) {
            return readings.justAccel();
        }
        return readings - calibration.justGyro();
    }
}
