using System.Linq;
using System.Windows.Media.Media3D;
using InTheHand.Net;
using System;

struct BoardDescription {
    public readonly string name;
    public readonly string pin;
    public readonly BluetoothAddress address;
    public readonly Quaternion motionOrientation;

    private static BluetoothAddress spaceSeparatedHexToBluetoothAddress(string addressAsSpaceSeparatedHex) {
        return new BluetoothAddress(addressAsSpaceSeparatedHex.Split(' ').Select(e => Convert.ToByte(e, 16)).Reverse().ToArray());
    }

    public BoardDescription(string name, string addressAsSpaceSeparatedHex, string pin, Quaternion motionOrientation) {
        this.name = name;
        this.pin = pin;
        this.address = spaceSeparatedHexToBluetoothAddress(addressAsSpaceSeparatedHex);
        this.motionOrientation = motionOrientation;
    }

    public BoardDescription(string name, BluetoothAddress address, string pin, Quaternion motionOrientation) {
        this.name = name;
        this.pin = pin;
        this.address = address;
        this.motionOrientation = motionOrientation;
    }

    public override string ToString() {
        return name + " (" + address + ")";
    }
}
