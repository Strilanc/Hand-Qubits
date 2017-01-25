using System.IO.Ports;
using System.Threading.Tasks;
using System.Windows.Media.Media3D;

class MotionSourceSerial : MotionSource {
    private SerialPort r;
    private readonly string port;
    private readonly int rate;

    public MotionSourceSerial(string port, int rate = 9600) {
        this.port = port;
        this.rate = rate;
    }

    public Task init() {
        r = new SerialPort(port);
        r.BaudRate = rate;
        r.DataBits = 8;
        r.StopBits = StopBits.Two;
        r.Handshake = Handshake.None;
        r.Parity = Parity.None;
        r.Open();
        return Task.FromResult(0);
    }

    public MotionSourceReading nextReading() {
        // Wait for header.
        while (r.ReadByte() != 0xA9) {
        }

        var w = r.readFloat();
        var x = r.readFloat();
        var y = r.readFloat();
        var z = r.readFloat();
        var gx = r.readFloat();
        var gy = r.readFloat();
        var gz = r.readFloat();
        var q = new Quaternion(x, y, z, w);
        var up = new Vector3D(gx, gy, gz);
        return new MotionSourceReading { deltaRotation = q, upward = up };
    }

    public void setContactId(byte id) {
        r.Write(new[] { (byte) 'm', id }, 0, 2);
    }

    public void reportMeasurement(bool result) {
        r.Write(new[] { result ? 'r' : 'b'}, 0, 1);
    }
}
