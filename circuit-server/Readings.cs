using System;
using System.Windows.Media.Media3D;

public struct RawReadings {
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
            "acc {0} gyro {1}",
            acc.ToShortString(),
            gyro.ToShortString());
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
