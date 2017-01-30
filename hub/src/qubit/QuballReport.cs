using System.Windows.Media.Media3D;

public struct QuballReport {
    public byte id;
    public byte? peerContactId;
    public Quaternion deltaRotation;
    public Vector3D upward;
    public bool doMeasurement;
}
