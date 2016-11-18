using System;
using System.Windows.Media.Media3D;

struct RawReadings {
    public Vector3D acc;
    public Vector3D gyro;

    public static RawReadings operator +(RawReadings a, RawReadings b) {
        return new RawReadings() {
            acc = a.acc + b.acc,
            gyro = a.gyro + b.gyro
        };
    }

    public static RawReadings operator -(RawReadings a, RawReadings b) {
        return new RawReadings() {
            acc = a.acc - b.acc,
            gyro = a.gyro - b.gyro
        };
    }

    public static RawReadings operator *(RawReadings a, double b) {
        return new RawReadings() {
            acc = a.acc * b,
            gyro = a.gyro * b
        };
    }

    public override string ToString() {
        return String.Format(
            "acc ({0:+0.00;-0.00}, {1:+0.00;-0.00}, {2:+0.00;-0.00}) gyro ({3:+0.00;-0.00}, {4:+0.00;-0.00}, {5:+0.00;-0.00})",
            acc.X,
            acc.Y,
            acc.Z,
            gyro.X,
            gyro.Y,
            gyro.Z);
    }

    public RawReadings justGyro() {
        return new RawReadings() {
            gyro = gyro
        };
    }

    public RawReadings justAccel() {
        return new RawReadings() {
            acc = acc
        };
    }
}

class SmoothedReadings {
    public double gyroNoise = 0;
    public RawReadings calibration = new RawReadings();
    private int stableReadings = 0;

    public bool isStable() {
        return gyroNoise < 40;
    }

    public bool isResting() {
        if (stableReadings < 100) {
            return gyroNoise < 1;
        }
        if (stableReadings < 1000) {
            return gyroNoise < 0.5;
        }
        return gyroNoise < 0.25;
    }

    public RawReadings update(RawReadings readings) {
        gyroNoise *= 0.7;
        gyroNoise += (readings.gyro - calibration.gyro).Length;

        if (isResting()) {
            stableReadings += 1;
            double a = 0.1;
            double b = 0.9;
            calibration *= b;
            calibration += readings * a;
            return readings.justAccel();
        }

        return readings - calibration.justGyro();
    }
}

