using System;
using System.IO;
using System.Linq;
using System.Windows.Media.Media3D;
using InTheHand.Net.Bluetooth;
using InTheHand.Net.Sockets;
using InTheHand.Net;
using System.Threading;
using System.Threading.Tasks;

class MotionSourceBluetooth : MotionSource {
    private BluetoothClient localClient;
    private BinaryReader reader;
    private BinaryWriter writer;
    private readonly BoardDescription board;

    public MotionSourceBluetooth(BoardDescription board) {
        this.board = board;
    }

    public static BluetoothDeviceInfo[] discoverDevices() {
        var c = new BluetoothClient();
        var devices = c.DiscoverDevices();
        var hcs = devices.Where(e => e.DeviceName.StartsWith("Strilanc Qubits ")).ToArray();
        var recents = hcs.Where(e => DateTime.Now - e.LastSeen < TimeSpan.FromMinutes(5)).ToArray();
        return recents;
    }

    public async Task init() {
        if (localClient != null) {
            localClient.Close();
            localClient.Dispose();
        }
        //if (!BluetoothSecurity.PairRequest(board.address, board.pin)) {
        //    Thread.Sleep(TimeSpan.FromSeconds(1));
        //    throw new IOException("pairing refused");
        //}

        localClient = new BluetoothClient();
        localClient.Client.ReceiveTimeout = (int) TimeSpan.FromSeconds(5).TotalMilliseconds;
        await Task.Factory.FromAsync(localClient.BeginConnect,
                                     localClient.EndConnect,
                                     board.address,
                                     BluetoothService.SerialPort,
                                     null);
        var stream = localClient.GetStream();
        reader = new BinaryReader(stream);
        writer = new BinaryWriter(stream);
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
        var up = new Vector3D(gx, 0, 0);
        var contactId = reader.ReadByte();
        var peerContactId = reader.ReadByte();

        return new MotionSourceReading {
            deltaRotation = q,
            upward = up,
            doMeasurement = up.Length/300 > 2,
            contactId = contactId,
            peerContactId = peerContactId
        };
    }

    public void setContactId(byte id) {
        writer.Write('m');
        writer.Write(id);
    }

    public void reportMeasurement(bool result) {
        writer.Write(result ? 'r' : 'b');
    }
}
