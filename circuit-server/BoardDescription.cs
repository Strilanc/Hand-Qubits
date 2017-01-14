using System.Windows.Media.Media3D;
using InTheHand.Net;

struct BoardDescription {
    public readonly string name;
    public readonly BluetoothAddress address;
    public readonly Quaternion motionOrientation;

    public BoardDescription(string name, BluetoothAddress address, Quaternion motionOrientation) {
        this.name = name;
        this.address = address;
        this.motionOrientation = motionOrientation;
    }
}
