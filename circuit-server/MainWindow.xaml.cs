using System;
using System.Threading;
using System.Windows;
using System.Windows.Media.Media3D;
using InTheHand.Net;

namespace QubitServer {
    public partial class MainWindow : Window {
        private QubitMotionTracker qubit1;
        private QubitMotionTracker qubit2;

        public MainWindow() {
            InitializeComponent();

            var teapotRotationAdjust = new Quaternion(new Vector3D(0, 0, 1), -45)
                * new Quaternion(new Vector3D(1, 0, 0), 90);
            qubit1 = new QubitMotionTracker(
                KnownBoard.Bip,
                new MotionDestGraph(motionChart1),
                q => poseTransform.Rotation = new QuaternionRotation3D(teapotRotationAdjust * q));
            qubit2 = new QubitMotionTracker(
                KnownBoard.Colu,
                new MotionDestGraph(motionChart2),
                q => poseTransform2.Rotation = new QuaternionRotation3D(teapotRotationAdjust * q));

            runForever(new MotionSourceBluetooth(qubit1.board.address), qubit1.advanceSimulation);
            runForever(new MotionSourceBluetooth(qubit2.board.address), qubit2.advanceSimulation);
        }

        private void runForever(MotionSource src, Action<MotionSourceReading> dst) {
            ThreadPool.QueueUserWorkItem(_ => Util.RetryForever(() => {
                src.init();

                while (true) {
                    var reading = src.nextReading();
                    Application.Current.Dispatcher.Invoke(() => dst(reading));
                }
            }));
        }

        private void button_Click(object sender, RoutedEventArgs e) {
            qubit1.reset();
            qubit2.reset();
        }
    }
}
