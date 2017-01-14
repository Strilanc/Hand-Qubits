using System;
using System.Windows.Media.Media3D;
using InTheHand.Net;

class QubitMotionTracker {
    public readonly BoardDescription board;
    private readonly Action<Quaternion> output;
    private readonly MotionDestGraph graph;
    private Quaternion pose = Quaternion.Identity;

    public QubitMotionTracker(BoardDescription board, MotionDestGraph graph, Action<Quaternion> output) {
        this.board = board;
        this.graph = graph;
        this.output = output;

        output(Quaternion.Identity);
    }

    public void advanceSimulation(MotionSourceReading reading) {
        var dPose = reading.deltaRotation;
        if (double.IsNaN(dPose.W) || double.IsNaN(dPose.X) || double.IsNaN(dPose.Y) || double.IsNaN(dPose.Z)) {
            return;
        }
        
        // Switch from accelerometer coordinates to board coordinates.
        dPose = board.motionOrientation * dPose * board.motionOrientation.Conjugated();

        pose *= dPose;
        pose.Normalize();

        output(pose);

        graph.showReading(new MotionSourceReading { deltaRotation = dPose, upward = reading.upward });
        //var accelUpward = new Vector3D(0, 0, 1).RotationTo(smoothed.acc);
        //var recoverUpward = pose.pose * accelUpward;
        //gravityTransform.Rotation = new QuaternionRotation3D(postRotation * recoverUpward);
    }

    public void reset() {
        this.pose = Quaternion.Identity;
    }
}
