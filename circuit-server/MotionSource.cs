using System.Windows.Media.Media3D;

public interface MotionSource {
    void init();
    MotionSourceReading nextReading();
    void setContactId(byte id);
    void reportMeasurement(bool result);
}

public struct MotionSourceReading {
    public Quaternion deltaRotation;
    public Vector3D upward;
    public bool doMeasurement;

    public byte contactId;
    public byte peerContactId; // else 0xFF
}
