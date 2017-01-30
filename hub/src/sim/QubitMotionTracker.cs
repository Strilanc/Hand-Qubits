using System;
using System.Windows.Media.Media3D;

class QubitMotionTracker {
    public readonly BoardDescription board;
    private readonly Action<Quaternion> output;
    private readonly Action<Vector3D> blochOutput;
    private readonly MotionDestGraph graph;
    private Quaternion pose = Quaternion.Identity;
    private byte? lastPeer;
    private byte? nextPeer;
    private byte nextPeerStability;

    public QubitMotionTracker(BoardDescription board, MotionDestGraph graph, Action<Quaternion> output, Action<Vector3D> blochOutput) {
        this.board = board;
        this.graph = graph;
        this.output = output;
        this.blochOutput = blochOutput;

        output(Quaternion.Identity);
    }

    public bool? advanceSimulation(QuballReport reading, StateVector state) {
        var needOperation = reading.doMeasurement;

        var dPose = reading.deltaRotation;
        if (double.IsNaN(dPose.W) || double.IsNaN(dPose.X) || double.IsNaN(dPose.Y) || double.IsNaN(dPose.Z)) {
            return null;
        }

        if (reading.peerContactId == lastPeer) {
            nextPeerStability = 0;
        } else if (reading.peerContactId != nextPeer) {
            nextPeerStability = 0;
            nextPeer = reading.peerContactId;
        } else if (nextPeerStability > 10) {
            lastPeer = nextPeer;
            needOperation = true;
        }

        // Switch from accelerometer coordinates to board coordinates.
        var teapotRotationAdjust = new Quaternion(new Vector3D(1, 0, 0), 90) * new Quaternion(new Vector3D(0, 1, 0), 90);
        var dPose2 = pose.Conjugated() * dPose * pose;

        pose = dPose.Conjugated() * pose;
        pose.Normalize();

        var control = reading.peerContactId < state.qubitCount ? (int?)reading.peerContactId : null;
        state.rotateQubit(dPose2, reading.id, control);

        bool? r = null;
        if (reading.doMeasurement) {
            r = state.measureQubit(reading.id);
            //this.pose = r.Value ? new Quaternion(1, 0, 0, 0) : Quaternion.Identity;
        }

        output(board.motionOrientation * pose.Conjugated() * board.motionOrientation.Conjugated());
        blochOutput(state.qubitAsBlochVector(reading.id));

        graph.showReading(new QuballReport { deltaRotation = dPose, upward = reading.upward });

        return r;
    }

    public void reset() {
        this.pose = Quaternion.Identity;
    }
}
