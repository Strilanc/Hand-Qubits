using System;
using System.Threading;
using System.Windows;
using System.Windows.Forms.DataVisualization.Charting;
using System.Windows.Media.Media3D;

namespace QubitServer {
    public partial class MainWindow : Window {
        private Action buttonHandlers = () => { };

        public MainWindow() {
            InitializeComponent();

            ThreadPool.SetMinThreads(8, 8);
            dumpMotionDataToGraph(KnownBoard.Aldo, motionChart1, poseTransform1);
            dumpMotionDataToGraph(KnownBoard.Bip, motionChart2, poseTransform2);
            dumpMotionDataToGraph(KnownBoard.Colu, motionChart3, poseTransform3);
            dumpMotionDataToGraph(KnownBoard.Dask, motionChart4, poseTransform4);
        }

        private void dumpMotionDataToGraph(BoardDescription board, Chart chart, RotateTransform3D pose) {
            var teapotRotationAdjust = new Quaternion(new Vector3D(0, 0, 1), -45)
                * new Quaternion(new Vector3D(1, 0, 0), 90);

            var qubitMotionTracker = new QubitMotionTracker(
                KnownBoard.Colu,
                new MotionDestGraph(chart),
                q => pose.Rotation = new QuaternionRotation3D(teapotRotationAdjust * q));

            buttonHandlers += () => qubitMotionTracker.reset();

            dumpMotionDataToGraph(
                new MotionSourceBluetooth(qubitMotionTracker.board.address),
                qubitMotionTracker.advanceSimulation);
        }

        private void dumpMotionDataToGraph(MotionSource src, Action<MotionSourceReading> dst) {
            ThreadPool.QueueUserWorkItem(_ => Util.RetryForever(() => {
                src.init();

                while (true) {
                    var reading = src.nextReading();
                    Application.Current.Dispatcher.Invoke(() => dst(reading));
                }
            }));
        }

        private void button_Click(object sender, RoutedEventArgs e) {
            buttonHandlers();
        }
    }
}
