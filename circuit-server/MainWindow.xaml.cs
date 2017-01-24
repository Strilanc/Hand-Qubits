using System;
using System.Threading;
using System.Windows;
using System.Windows.Forms.DataVisualization.Charting;
using System.Windows.Media.Media3D;

namespace QubitServer {
    public partial class MainWindow : Window {
        private Action buttonHandlers = () => { };
        private byte next_id = 0;
        private StateVector state = new StateVector(4);

        public MainWindow() {
            InitializeComponent();

            ThreadPool.SetMinThreads(8, 8);
            dumpMotionDataToGraph(KnownBoard.Aldo, motionChart1, poseTransform1, blochPoint1);
            dumpMotionDataToGraph(KnownBoard.Bip, motionChart2, poseTransform2, blochPoint2);
            dumpMotionDataToGraph(KnownBoard.Colu, motionChart3, poseTransform3, blochPoint3);
            dumpMotionDataToGraph(KnownBoard.Dask, motionChart4, poseTransform4, blochPoint4);
        }

        private void dumpMotionDataToGraph(BoardDescription board, Chart chart, RotateTransform3D pose, TranslateTransform3D blochTransform) {
            var teapotRotationAdjust = new Quaternion(new Vector3D(1, 0, 0), 90) * new Quaternion(new Vector3D(0, 1, 0), 90);

            var qubitMotionTracker = new QubitMotionTracker(
                KnownBoard.Colu,
                new MotionDestGraph(chart),
                q => pose.Rotation = new QuaternionRotation3D(teapotRotationAdjust * q),
                v => {
                    blochTransform.OffsetX = v.X;
                    blochTransform.OffsetY = v.Y;
                    blochTransform.OffsetZ = v.Z;
                });

            buttonHandlers += () => qubitMotionTracker.reset();

            dumpMotionDataToGraph(
                new MotionSourceBluetooth(qubitMotionTracker.board.address),
                e => qubitMotionTracker.advanceSimulation(e, state));
        }

        private void dumpMotionDataToGraph(MotionSource src, Func<MotionSourceReading, bool?> dst) {
            var id = next_id++;
            ThreadPool.QueueUserWorkItem(_ => Util.RetryForever(() => {
                src.init();
                src.setContactId(id);

                while (true) {
                    var reading = src.nextReading();
                    if (reading.contactId != id) {
                        src.setContactId(id);
                        reading.contactId = id;
                    }
                    Application.Current.Dispatcher.Invoke(() => {
                        var m = dst(reading);
                        if (m.HasValue) {
                            src.reportMeasurement(m.Value);
                        }
                    });
                }
            }));
        }

        private void button_Click(object sender, RoutedEventArgs e) {
            buttonHandlers();
        }
    }
}
