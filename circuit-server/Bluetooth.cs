using System;
using System.Linq;
using System.Diagnostics;
using System.Windows.Media.Media3D;
using InTheHand.Net.Bluetooth;
using InTheHand.Net.Ports;
using InTheHand.Net.Sockets;
using InTheHand.Net.Mime;
using InTheHand.Net.IrDA;
using InTheHand.Net.Bluetooth.AttributeIds;
using InTheHand.Net.Bluetooth.Factory;
using InTheHand.Net.Bluetooth.Msft;
using InTheHand.Net.Bluetooth.Widcomm;
using InTheHand.Net;

class Bluetooth {
    private BluetoothClient localClient;
    private System.IO.BinaryReader reader;

    public static BluetoothDeviceInfo[] discoverDevices() {
        var c = new BluetoothClient();
        var devices = c.DiscoverDevices();
        return devices.Where(e => e.DeviceName == "HC-06").ToArray();
    }

    public void init(BluetoothAddress address) {
        if (localClient != null) {
            localClient.Dispose();
        }
        localClient = new BluetoothClient();
        if (!BluetoothSecurity.PairRequest(address, "1234")) {
            throw new System.IO.IOException("pairing refused");
        }
        var endpoint = new BluetoothEndPoint(address, BluetoothService.SerialPort);
        localClient.Client.ReceiveTimeout = (int) TimeSpan.FromSeconds(5).TotalMilliseconds;
        localClient.Connect(endpoint);
        reader = new System.IO.BinaryReader(localClient.GetStream());
    }

    public Quaternion readReading() {
        // Wait for header.
        while (reader.ReadByte() != 0xA9) {
        }

        var w = reader.ReadSingle();
        var x = reader.ReadSingle();
        var y = reader.ReadSingle();
        var z = reader.ReadSingle();
        var q = new Quaternion(x, y, z, w);
        Debug.WriteLine(q.Axis.ToShortString() + " around " + q.Angle);
        return q;
    }
}

