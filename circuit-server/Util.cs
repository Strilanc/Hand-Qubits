using System;
using System.Windows.Media.Media3D;

public static class Util {
    public static double Dot(this Vector3D a, Vector3D b) {
        return a.X * b.X + a.Y * b.Y + a.Z * b.Z;
    }

    public static Vector3D Cross(this Vector3D a, Vector3D b) {
        return new Vector3D(
            a.Y * b.Z - a.Z * b.Y,
            a.Z * b.X - a.X * b.Z,
            a.X * b.Y - a.Y * b.X);
    }

    public static Quaternion RotationTo(this Vector3D start, Vector3D end) {
        var axis = start.Cross(end);
        var angle = Math.Acos(start.Dot(end) / start.Length / end.Length);
        axis.Normalize();
        return new Quaternion(axis, angle * 180 / Math.PI);
    }

    public static String ToShortString(this Vector3D v) {
        return String.Format("<{0:+0.00;-0.00}, {1:+0.00;-0.00}, {2:+0.00;-0.00}>", v.X, v.Y, v.Z);
    }

    public static void RetryForever(Action action) {
        while (true) {
            try {
                action();
            } catch {
            }
        }
    }
}
