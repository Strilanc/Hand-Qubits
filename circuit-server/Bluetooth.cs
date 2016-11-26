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

    public void init() {
        localClient = new BluetoothClient();
        //var devices = localClient.DiscoverDevices();
        //var hcDevices = devices.Where(e => e.DeviceName == "HC-06").ToArray();
        //var device = devices.First(e => e.DeviceName == "HC-06");

        var address1 = new BluetoothAddress(new byte[] { 0xBC, 0x53, 0x70, 0x32, 0xD3, 0x98, 0, 0 });
        var address2 = new BluetoothAddress(new byte[] { 0x9D, 0x53, 0x90, 0x34, 0xD3, 0x98, 0, 0 });

        //BluetoothSecurity.PairRequest(address2, "1234");

        var endpoint = new BluetoothEndPoint(address1, BluetoothService.SerialPort);
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

