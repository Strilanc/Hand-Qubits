using System;
using System.Collections.Generic;
using System.IO.Ports;
using System.Threading;
using System.Windows;
using System.Windows.Media.Media3D;

namespace QubitServer {
    public partial class MainWindow : Window {
        private Dictionary<String, int> keys = new Dictionary<string, int>();
        private RawReadings readings = new RawReadings();
        private SmoothedReadings smoothedReadings = new SmoothedReadings();
        private double readToRads = 0.008 / 11.0;
        private OperationHistory history = new OperationHistory();

        public MainWindow() {
            InitializeComponent();

            gyroChart.ChartAreas[0].AxisX.MajorGrid.LineWidth = 0;
            gyroChart.ChartAreas[0].AxisY.MajorGrid.LineWidth = 0;
            gyroChart.ChartAreas[0].AxisY.Maximum = +50;
            gyroChart.ChartAreas[0].AxisY.Minimum = -50;

            StartReadingMotionData();
        }

        private void StartReadingMotionData() {
            ThreadPool.QueueUserWorkItem(_ => Util.RetryForever(() => {
                var r = new SerialPort("COM3");
                r.BaudRate = 9600;
                r.DataBits = 8;
                r.StopBits = StopBits.Two;
                r.Handshake = Handshake.None;
                r.Parity = Parity.None;
                r.Open();

                r.ReadLine();
                r.ReadLine();
                r.ReadLine();
                while (true) {
                    var s = r.ReadLine();
                    Application.Current.Dispatcher.Invoke(() => onReceive(s));
                }
            }));
        }

        private void onReceive(String line) {
            var parts = line.Split(':');
            if (parts.Length != 2) {
                return;
            }
            double val = 0;
            if (!Double.TryParse(parts[1], out val)) {
                val = 0;
            }

            var key = parts[0];
            switch (key) {
                case "Gyro X":
                    val *= readToRads;
                    readings.gyro.X = val;
                    break;
                case "Gyro Y":
                    val *= readToRads;
                    readings.gyro.Y = val;
                    break;
                case "Gyro Z":
                    val *= readToRads;
                    readings.gyro.Z = val;
                    break;
                case "Accel X":
                    readings.acc.X = val;
                    break;
                case "Accel Y":
                    readings.acc.Y = val;
                    break;
                case "Accel Z":
                    readings.acc.Z = val;
                    advanceSimulation();
                    break;
            }
        }

        List<String> lines = new List<string>();
        private void showReading(String key, String valDesc) {
            if (!keys.ContainsKey(key)) {
                keys.Add(key, keys.Count);
            }
            var i = keys[key];
            while (lines.Count <= i) {
                lines.Add("");
            }
            lines[i] = valDesc;
            readTextBlock.Text = String.Join("\n", lines);
        }

        Pose pose = new Pose();

        private void advanceSimulation() {
            var smoothed = smoothedReadings.update(readings);
            var shown = readings - smoothedReadings.calibration;
            gyroChart.Series[0].Points.AddY(shown.gyro.X);
            gyroChart.Series[1].Points.AddY(shown.gyro.Y);
            gyroChart.Series[2].Points.AddY(shown.gyro.Z);
            if (gyroChart.Series[0].Points.Count > 25) {
                gyroChart.Series[0].Points.RemoveAt(0);
                gyroChart.Series[1].Points.RemoveAt(0);
                gyroChart.Series[2].Points.RemoveAt(0);
            }

            pose.advanceSimulation(smoothed, smoothedReadings.isStable());
            history.advance(pose.pose);

            showReading("readings", "reading: " + readings.ToString());
            showReading("gyro noise",
                (smoothedReadings.isResting() ? "stable" : "unstable") + "  " + smoothedReadings.gyroNoise);
            showReading("calibration",
                "bias: " + smoothedReadings.calibration.ToString());
            showReading("corrected", "corrected: " + (readings - smoothedReadings.calibration).ToString());
            showReading("pose-rotation",
                String.Format("pose-rotation: {0:0} around {1:0.00}, {2:0.00}, {3:0.00}", pose.pose.Angle, pose.pose.Axis.X, pose.pose.Axis.Y, pose.pose.Axis.Z));
            showReading("history", history.ToString());

            var preRotation = new Quaternion(new Vector3D(0, 1, 0), 7) * new Quaternion(new Vector3D(1, 0, 0), 90);
            var postRotation = new Quaternion(new Vector3D(0, 0, 1), -45);
            poseTransform.Rotation = new QuaternionRotation3D(postRotation * pose.pose * preRotation);

            var accelUpward = new Vector3D(0, 0, 1).RotationTo(smoothed.acc);
            var recoverUpward = pose.pose * accelUpward;
            gravityTransform.Rotation = new QuaternionRotation3D(postRotation * recoverUpward);
        }

        private void button_Click(object sender, RoutedEventArgs e) {
            pose.pose = Quaternion.Identity;
            history = new OperationHistory();
        }
    }
}
