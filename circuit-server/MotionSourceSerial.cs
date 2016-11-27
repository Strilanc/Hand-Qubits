using System;
using System.Diagnostics;
using System.IO.Ports;
using System.Windows.Media.Media3D;

class MotionSourceSerial : MotionSource {
    private SerialPort r;
    private readonly string port;
    private readonly int rate;

    public MotionSourceSerial(string port, int rate = 9600) {
        this.port = port;
        this.rate = rate;
    }

    public void init() {
        r = new SerialPort(port);
        r.BaudRate = rate;
        r.DataBits = 8;
        r.StopBits = StopBits.Two;
        r.Handshake = Handshake.None;
        r.Parity = Parity.None;
        r.Open();
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
}
