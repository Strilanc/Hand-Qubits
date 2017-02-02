using System;
using System.IO;
using System.Linq;
using System.Windows.Media.Media3D;
using InTheHand.Net.Bluetooth;
using InTheHand.Net.Sockets;
using System.Threading.Tasks;

class QuballClient : IQuballClient {
    private BluetoothClient localClient;
    private BinaryReader reader;
    private BinaryWriter writer;
    private readonly BoardDescription board;

    public QuballClient(BoardDescription board) {
        this.board = board;
    }

    public static BluetoothDeviceInfo[] DiscoverDevices() {
        using (var client = new BluetoothClient()) {
            var devices = new BluetoothClient().DiscoverDevices();
            var hcs = devices.Where(e => e.DeviceName.StartsWith("Strilanc Qubits ") || e.DeviceName.StartsWith("HC")).ToArray();
            var recents = hcs.Where(e => DateTime.Now - e.LastSeen < TimeSpan.FromMinutes(5)).ToArray();
            return recents;
        }
    }

    public void Pair() {
        if (!BluetoothSecurity.PairRequest(board.address, board.pin)) {
            throw new IOException("Pairing failed.");
        }
    }

    public async Task Init() {
        if (localClient != null) {
            localClient.Dispose();
        }

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

    public QuballReport NextReading() {
        // Wait for header magic bytes.
        while (true) {
            if (reader.ReadByte() != 0xA9) continue;
            if (reader.ReadByte() != 0x42) continue;
            break;
        }

        // Who is this?
        var id = reader.ReadByte();

        // Who are you touching?
        var peerContactId = reader.ReadByte();

        // Incremental Rotation.
        var dRot = new Quaternion(
            w: reader.ReadSingle(),
            x: reader.ReadSingle(),
            y: reader.ReadSingle(),
            z: reader.ReadSingle());

        // Upward vector.
        var up = new Vector3D(
            x: reader.ReadSingle(),
            y: reader.ReadSingle(),
            z: reader.ReadSingle());

        // Bumpiness.
        var shake = reader.ReadSingle();

        return new QuballReport {
            id = id,
            peerContactId = peerContactId < 0xFE ? (byte?)peerContactId : null,
            deltaRotation = dRot,
            upward = up,
            doMeasurement = Math.Abs(shake)/300 > 2,
        };
    }

    public void TellOwnId(byte id) {
        writer.Write((byte) 'm');
        writer.Write(id);
    }

    public void TellMeasurementResult(bool result) {
        writer.Write((byte) (result ? 'r' : 'b'));
    }
}
