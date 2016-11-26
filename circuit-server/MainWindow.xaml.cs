using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO.Ports;
using System.Threading;
using System.Windows;
using System.Windows.Media.Media3D;

namespace QubitServer {
    public partial class MainWindow : Window {
        private const double READ_TO_ACC = 0.0001;
        private const double READ_TO_RADS = 0.000034*4;

        private Dictionary<String, int> keys = new Dictionary<string, int>();
        private SmoothedReadings smoothedReadings = new SmoothedReadings();
        private OperationHistory history = new OperationHistory();
        private Stopwatch stopwatch = new Stopwatch();
        private TimeSpan lastElapsedTime;
        Pose pose = new Pose();
        private int counter = 0;

        public MainWindow() {
            InitializeComponent();

            gyroChart.ChartAreas[0].AxisX.MajorGrid.LineWidth = 0;
            gyroChart.ChartAreas[0].AxisY.MajorGrid.LineWidth = 0;
            gyroChart.ChartAreas[0].AxisY.Maximum = +5;
            gyroChart.ChartAreas[0].AxisY.Minimum = -5;

            stopwatch.Start();
            lastElapsedTime = stopwatch.Elapsed;
            StartReadingMotionDataBluetooth();
            //StartReadingMotionDataSerial();
        }

        private void StartReadingMotionDataBluetooth() {
            ThreadPool.QueueUserWorkItem(_ => Util.RetryForever(() => {
                var b = new Bluetooth();
                b.init();

                while (true) {
                    var reading = b.readReading();
                    Application.Current.Dispatcher.Invoke(() => advanceSimulation(reading));
                }
            }));
        }

        private void StartReadingMotionDataSerial() {
            var b = new Bluetooth();
            b.init();

            ThreadPool.QueueUserWorkItem(_ => Util.RetryForever(() => {
                var r = new SerialPort("COM4");
                r.BaudRate = 9600;
                r.DataBits = 8;
                r.StopBits = StopBits.Two;
                r.Handshake = Handshake.None;
                r.Parity = Parity.None;
                r.Open();

                while (true) {
                    var reading = readReading(r);
                    Application.Current.Dispatcher.Invoke(() => advanceSimulation(reading));
                }
            }));
        }

        private static Quaternion readReading(SerialPort r) {
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

            //var ax = r.ReadInt16() * READ_TO_ACC;
            //var ay = r.ReadInt16() * READ_TO_ACC;
            //var az = r.ReadInt16() * READ_TO_ACC;
            //var gx = r.ReadInt16() * READ_TO_RADS;
            //var gy = r.ReadInt16() * READ_TO_RADS;
            //var gz = r.ReadInt16() * READ_TO_RADS;
            //return new RawReadings {
            //    acc = new Vector3D {
            //        X = ax,
            //        Y = ay,
            //        Z = az
            //    },
            //    gyro = new Vector3D {
            //        X = gx,
            //        Y = gy,
            //        Z = gz
            //    }
            //};
        }

        Quaternion rawPose = Quaternion.Identity;
        private void advanceSimulation(Quaternion dPose) {
            if (dPose != dPose) {
                return;
            }
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
            rawPose *= dPose;

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

            var p = rawPose;
            var preRotation = new Quaternion(new Vector3D(0, 1, 0), 7) * new Quaternion(new Vector3D(1, 0, 0), 90);
            var postRotation = new Quaternion(new Vector3D(0, 0, 1), -45);
            poseTransform.Rotation = new QuaternionRotation3D(postRotation * p * preRotation);

            //var accelUpward = new Vector3D(0, 0, 1).RotationTo(smoothed.acc);
            //var recoverUpward = pose.pose * accelUpward;
            //gravityTransform.Rotation = new QuaternionRotation3D(postRotation * recoverUpward);
        }

        private void button_Click(object sender, RoutedEventArgs e) {
            rawPose = Quaternion.Identity;
            pose.pose = Quaternion.Identity;
            history = new OperationHistory();
        }
    }
}
