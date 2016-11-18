using System;
using System.Windows.Media.Media3D;

class Pose {
    public Quaternion pose = Quaternion.Identity;

    private static Vector3D cross(Vector3D a, Vector3D b) {
        return new Vector3D(
            a.Y * b.Z - a.Z * b.Y,
            a.Z * b.X - a.X * b.Z,
            a.X * b.Y - a.Y * b.X);
    }

    public void advanceSimulation(RawReadings readings, bool isResting) {
        Matrix3D m = Matrix3D.Identity;
        m.Rotate(pose);
        var axis = readings.gyro;
        axis = m.Transform(axis);
        var angle = axis.Length;
        axis.Normalize();
        if (angle > 0 && !double.IsNaN(axis.Length)) {
            var q = new Quaternion(axis, angle);
            pose = q * pose;
        }

        m = Matrix3D.Identity;
        var p = pose;
        //p.Conjugate();
        m.Rotate(p);
        var acc = readings.acc;
        acc = m.Transform(acc);

        var torque = cross(new Vector3D(0, 0, -1), acc);
        var t = torque.Length * 10;
        torque.Normalize();
        if (isResting && t > 0) {
            pose = new Quaternion(torque, t) * pose;
        }
    }
}

