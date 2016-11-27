using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO.Ports;
using System.Threading;
using System.Windows;
using System.Windows.Media.Media3D;
using InTheHand.Net;

namespace QubitServer {
    class SimulationState {
        public readonly BluetoothAddress address;
        private readonly Action<Quaternion> output;
        private readonly Quaternion groundOrientation;
        private readonly Quaternion preOrientation;
        private readonly Quaternion postOrientation;
        private Quaternion pose = Quaternion.Identity;

        public SimulationState(BluetoothAddress address, Action<Quaternion> output, Quaternion groundOrientation, Quaternion preOrientation, Quaternion postOrientation) {
            this.address = address;
            this.output = output;
            this.groundOrientation = groundOrientation;
            this.preOrientation = preOrientation;
            this.postOrientation = postOrientation;
        }

        public void advanceSimulation(Quaternion dPose) {
            if (dPose != dPose) {
                return;
            }

            var counterRotation = groundOrientation;
            counterRotation.Conjugate();
            dPose = groundOrientation * dPose * counterRotation;

            //RawReadings readings
            //var t = stopwatch.Elapsed;
            //var dt = t - lastElapsedTime;
            //lastElapsedTime = t;
            //Debug.WriteLine(dt.TotalMilliseconds);

            //var smoothed = smoothedReadings.update(readings);
            //var shown = readings - smoothedReadings.calibration;
            //counter++;

            //pose.advanceSimulation(smoothed, smoothedReadings.isStable());
            //history.advance(pose.pose);
            pose *= dPose;
            pose.Normalize();

            //if (counter % 10 == 0) {
            //    gyroChart.Series[0].Points.AddY(shown.gyro.X);
            //    gyroChart.Series[1].Points.AddY(shown.gyro.Y);
            //    gyroChart.Series[2].Points.AddY(shown.gyro.Z);
            //    if (gyroChart.Series[0].Points.Count > 100) {
            //        gyroChart.Series[0].Points.RemoveAt(0);
            //        gyroChart.Series[1].Points.RemoveAt(0);
            //        gyroChart.Series[2].Points.RemoveAt(0);
            //    }

            //    readTextBlock.Text = String.Join("\n", new String[] {
            //        "reading: " + readings.ToString(),
            //        "gyro noise: " + (smoothedReadings.isResting() ? "stable" : "unstable") + "  " + smoothedReadings.gyratingness,
            //        "gyro bias: " + smoothedReadings.calibration.gyro.ToShortString(),
            //        "gyro corr: " + (readings - smoothedReadings.calibration).gyro.ToShortString(),
            //        "ops: " + history.ToString()
            //    });
            //}

            var preRotation = new Quaternion(new Vector3D(0, 1, 0), 7) * new Quaternion(new Vector3D(1, 0, 0), 90);
            preRotation = new Quaternion(new Vector3D(0, 1, 0), -3) * new Quaternion(new Vector3D(1, 0, 0), 90);
            var postRotation = new Quaternion(new Vector3D(0, 0, 1), -45);
            output(postRotation * pose * preRotation);

            //var accelUpward = new Vector3D(0, 0, 1).RotationTo(smoothed.acc);
            //var recoverUpward = pose.pose * accelUpward;
            //gravityTransform.Rotation = new QuaternionRotation3D(postRotation * recoverUpward);
        }

        public void reset() {
            this.pose = Quaternion.Identity;
        }
    }

    public partial class MainWindow : Window {
        private static readonly BluetoothAddress address1 = new BluetoothAddress(new byte[] { 0xBC, 0x53, 0x70, 0x32, 0xD3, 0x98, 0, 0 });
        private static readonly BluetoothAddress address2 = new BluetoothAddress(new byte[] { 0x9D, 0x53, 0x90, 0x34, 0xD3, 0x98, 0, 0 });

        private SimulationState state1;
        private SimulationState state2;

        private OperationHistory history = new OperationHistory();
        private TimeSpan lastElapsedTime;

        public MainWindow() {
            InitializeComponent();

            gyroChart.ChartAreas[0].AxisX.MajorGrid.LineWidth = 0;
            gyroChart.ChartAreas[0].AxisY.MajorGrid.LineWidth = 0;
            gyroChart.ChartAreas[0].AxisY.Maximum = +5;
            gyroChart.ChartAreas[0].AxisY.Minimum = -5;

            state1 = new SimulationState(
                address1,
                q => poseTransform.Rotation = new QuaternionRotation3D(q),
                new Quaternion(new Vector3D(0, 1, 0), -90),
                new Quaternion(new Vector3D(0, 1, 0), 7) * new Quaternion(new Vector3D(1, 0, 0), 90),
                new Quaternion(new Vector3D(0, 0, 1), -45));
            state2 = new SimulationState(
                address2,
                q => poseTransform2.Rotation = new QuaternionRotation3D(q),
                Quaternion.Identity,
                new Quaternion(new Vector3D(0, 1, 0), -3) * new Quaternion(new Vector3D(1, 0, 0), 90),
                new Quaternion(new Vector3D(0, 0, 1), -45));

            new BluetoothCoupling(state1.address).runForever(state1.advanceSimulation);
            new BluetoothCoupling(state2.address).runForever(state2.advanceSimulation);
        }

        private void button_Click(object sender, RoutedEventArgs e) {
            state1.reset();
            state2.reset();
            history = new OperationHistory();
        }
    }

    abstract class Coupling {
        protected abstract void init();
        protected abstract Quaternion read();

        public void runForever(Action<Quaternion> shower) {
            ThreadPool.QueueUserWorkItem(_ => Util.RetryForever(() => {
                init();

                while (true) {
                    var reading = read();
                    Application.Current.Dispatcher.Invoke(() => shower(reading));
                }
            }));
        }
    }

    class BluetoothCoupling : Coupling {
        private Bluetooth b;
        private readonly BluetoothAddress address;

        public BluetoothCoupling(BluetoothAddress address) {
            this.address = address;
        }

        protected override void init() {
            b = new Bluetooth();
            b.init(address);
        }

        protected override Quaternion read() {
            return b.readReading();
        }
    }

    class SerialCoupling : Coupling {
        private SerialPort r;
        private readonly String port;
        private readonly int rate;

        public SerialCoupling(String port, int rate = 9600) {
            this.port = port;
            this.rate = rate;
        }

        protected override void init() {
            r = new SerialPort(port);
            r.BaudRate = rate;
            r.DataBits = 8;
            r.StopBits = StopBits.Two;
            r.Handshake = Handshake.None;
            r.Parity = Parity.None;
            r.Open();
        }

        protected override Quaternion read() {
            // Wait for header.
            while (r.ReadByte() != 0xA9) {
            }

            var w = r.readFloat();
            var x = r.readFloat();
            var y = r.readFloat();
            var z = r.readFloat();
            var q = new Quaternion(x, y, z, w);
            Debug.WriteLine(q.Axis.ToShortString() + " around " + q.Angle);
            return q;
        }
    }
}
