using System;
using System.Threading;
using System.Windows;
using System.Windows.Forms.DataVisualization.Charting;
using System.Windows.Media.Media3D;

public partial class MainWindow : Window {
    private Action buttonHandlers = () => { };
    private byte next_id = 0;
    private StateVector state = new StateVector(4);

    public MainWindow() {
        InitializeComponent();

        ThreadPool.SetMinThreads(8, 8);
        dumpMotionDataToGraph(KnownBoard.Aldo, motionChart1, poseTransform1, blochPoint1);
        dumpMotionDataToGraph(KnownBoard.Boni, motionChart2, poseTransform2, blochPoint2);
        dumpMotionDataToGraph(KnownBoard.Colu, motionChart3, poseTransform3, blochPoint3);
        dumpMotionDataToGraph(KnownBoard.Dask, motionChart4, poseTransform4, blochPoint4);
    }

    private void dumpMotionDataToGraph(BoardDescription board, Chart chart, RotateTransform3D pose, TranslateTransform3D blochTransform) {
        var teapotRotationAdjust = new Quaternion(new Vector3D(1, 0, 0), 90) * new Quaternion(new Vector3D(0, 1, 0), 90);

        var qubitMotionTracker = new QubitMotionTracker(
            board,
            new MotionDestGraph(chart),
            q => pose.Rotation = new QuaternionRotation3D(teapotRotationAdjust * q),
            v => {
                blochTransform.OffsetX = v.X;
                blochTransform.OffsetY = v.Y;
                blochTransform.OffsetZ = v.Z;
            });

        buttonHandlers += () => qubitMotionTracker.reset();

        dumpMotionDataToGraph(
            new QuballClient(qubitMotionTracker.board),
            e => qubitMotionTracker.advanceSimulation(e, state));
    }

    private void dumpMotionDataToGraph(IQuballClient src, Func<QuballReport, bool?> dst) {
        var id = next_id++;
        ThreadPool.QueueUserWorkItem(_ => Util.RetryForeverAsync(async () => {
            await src.Init();
            src.TellOwnId(id);

            while (true) {
                var reading = src.NextReading();
                if (reading.id != id) {
                    src.TellOwnId(id);
                    reading.id = id;
                }
                Application.Current.Dispatcher.Invoke(() => {
                    var m = dst(reading);
                    if (m.HasValue) {
                        src.TellMeasurementResult(m.Value);
                    }
                });
            }
        }));
    }

    private void button_Click(object sender, RoutedEventArgs e) {
        buttonHandlers();
    }
}
