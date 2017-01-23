using System;
using System.Windows.Media.Media3D;
using InTheHand.Net;

class QubitMotionTracker {
    public readonly BoardDescription board;
    private readonly Action<Quaternion> output;
    private readonly MotionDestGraph graph;
    private Quaternion pose = Quaternion.Identity;
    private byte lastPeer = 0xFF;
    private byte nextPeer;
    private byte nextPeerStability;

    public QubitMotionTracker(BoardDescription board, MotionDestGraph graph, Action<Quaternion> output) {
        this.board = board;
        this.graph = graph;
        this.output = output;

        output(Quaternion.Identity);
    }

    public bool? advanceSimulation(MotionSourceReading reading, StateVector state) {
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
        dPose = board.motionOrientation * dPose * board.motionOrientation.Conjugated();

        pose *= dPose;
        pose.Normalize();

        var control = reading.peerContactId < state.qubitCount ? (int?)reading.peerContactId : null;
        state.rotateQubit(dPose, reading.contactId, control);

        bool? r = null;
        if (reading.doMeasurement) {
            r = state.measureQubit(reading.contactId);
            this.pose = r.Value ? new Quaternion(1, 0, 0, 0) : Quaternion.Identity;
        }

        output(pose);

        graph.showReading(new MotionSourceReading { deltaRotation = dPose, upward = reading.upward });

        return r;
    }

    public void reset() {
        this.pose = Quaternion.Identity;
    }
}
