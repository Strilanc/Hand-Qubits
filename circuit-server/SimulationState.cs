using System;
using System.Windows.Media.Media3D;
using InTheHand.Net;

class QubitMotionTracker {
    public readonly BluetoothAddress address;
    private readonly Action<Quaternion> output;
    private readonly Quaternion groundOrientation;
    private readonly Quaternion preOrientation;
    private readonly Quaternion postOrientation;
    private readonly MotionDestGraph graph;
    private Quaternion pose = Quaternion.Identity;

    public QubitMotionTracker(BluetoothAddress address, MotionDestGraph graph, Action<Quaternion> output, Quaternion groundOrientation, Quaternion preOrientation, Quaternion postOrientation) {
        this.address = address;
        this.graph = graph;
        this.output = output;
        this.groundOrientation = groundOrientation;
        this.preOrientation = preOrientation;
        this.postOrientation = postOrientation;
    }

    public void advanceSimulation(MotionSourceReading reading) {
        var dPose = reading.deltaRotation;
        if (double.IsNaN(dPose.W) || double.IsNaN(dPose.X) || double.IsNaN(dPose.Y) || double.IsNaN(dPose.Z)) {
            return;
        }

        var groundOrientation = this.groundOrientation;
        var counterRotation = groundOrientation;
        counterRotation.Conjugate();
        dPose = groundOrientation * dPose * counterRotation;

        pose *= dPose;
        pose.Normalize();

        output(this.postOrientation * pose * this.preOrientation);

        graph.showReading(new MotionSourceReading { deltaRotation = dPose, upward = reading.upward });
        //var accelUpward = new Vector3D(0, 0, 1).RotationTo(smoothed.acc);
        //var recoverUpward = pose.pose * accelUpward;
        //gravityTransform.Rotation = new QuaternionRotation3D(postRotation * recoverUpward);
    }

    public void reset() {
        this.pose = Quaternion.Identity;
    }
}
