using System.Windows.Media.Media3D;

public interface MotionSource {
    void init();
    MotionSourceReading nextReading();
}

public struct MotionSourceReading {
    public Quaternion deltaRotation;
    public Vector3D upward;
}
