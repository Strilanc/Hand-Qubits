using System;
using System.IO;
using System.Linq;
using System.Windows.Media.Media3D;
using InTheHand.Net.Bluetooth;
using InTheHand.Net.Sockets;
using InTheHand.Net;

class MotionSourceBluetooth : MotionSource {
    private BluetoothClient localClient;
    private BinaryReader reader;
    private readonly BluetoothAddress address;
    private readonly string pin;

    public MotionSourceBluetooth(BluetoothAddress address, string pin = "1234") {
        this.address = address;
        this.pin = pin;
    }

    public static BluetoothDeviceInfo[] discoverDevices() {
        var c = new BluetoothClient();
        var devices = c.DiscoverDevices();
        return devices.Where(e => e.DeviceName.StartsWith("HC-0")).ToArray();
    }

    public void init() {
        if (localClient != null) {
            localClient.Dispose();
        }
        localClient = new BluetoothClient();
        //if (!BluetoothSecurity.PairRequest(address, pin)) {
        //    throw new IOException("pairing refused");
        //}
        var endpoint = new BluetoothEndPoint(address, BluetoothService.SerialPort);
        localClient.Client.ReceiveTimeout = (int) TimeSpan.FromSeconds(5).TotalMilliseconds;
        localClient.Connect(endpoint);
        reader = new BinaryReader(new BufferedStream(localClient.GetStream()));
    }

    public MotionSourceReading nextReading() {
        // Wait for header.
        while (reader.ReadByte() != 0xA9) {
        }

        var w = reader.ReadSingle();
        var x = reader.ReadSingle();
        var y = reader.ReadSingle();
        var z = reader.ReadSingle();
        var q = new Quaternion(x, y, z, w);

        var gx = reader.ReadSingle();
        var gy = reader.ReadSingle();
        var gz = reader.ReadSingle();
        var up = new Vector3D(gx, gy, gz);

        return new MotionSourceReading { deltaRotation = q, upward = up }; ;
    }
}
