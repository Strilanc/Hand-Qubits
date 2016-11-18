using System;
using System.Collections.Generic;
using System.Linq;
using System.Windows.Media.Media3D;

struct Operation {
    public readonly double power;
    public readonly Vector3D axis;
    public Operation(Vector3D axis, double power) {
        this.power = power;
        this.axis = axis;
    }

    private String _axisName() {
        if (axis == new Vector3D(1, 0, 0)) {
            return "Y";
        }
        if (axis == new Vector3D(0, 1, 0)) {
            return "X";
        }
        if (axis == new Vector3D(0, 0, 1)) {
            return "Z";
        }
        if (axis == new Vector3D(1, 1, 0)) {
            return "H";
        }
        return "<" + axis.ToString() + ">";
    }

    public Nullable<Operation> TryMerge(Operation other) {
        if (this.axis == other.axis) {
            var p = this.power + other.power;
            if (p > 1) p -= 2;
            if (p <= -1) p += 2;
            return new Operation(this.axis, p);
        }
        return null;
    }

    private String _powerName() {
        if (power == 1) {
            return "";
        }
        if (power == 0.25) {
            return "^(1/4)";
        }
        if (power == 0.5) {
            return "^(1/2)";
        }
        if (power == -0.25) {
            return "^(-1/4)";
        }
        if (power == -0.5) {
            return "^(-1/2)";
        }
        return "^(" + power + ")";
    }

    public override string ToString() {
        return _axisName() + _powerName();
    }
}

class OperationHistory {
    private static readonly Operation[] KnownOperations = new[] {
        new Operation(new Vector3D(1, 0, 0), 0.25),
        new Operation(new Vector3D(1, 0, 0), 0.5),
        new Operation(new Vector3D(1, 0, 0), 0.75),
        new Operation(new Vector3D(1, 0, 0), 1),
        new Operation(new Vector3D(1, 0, 0), -0.75),
        new Operation(new Vector3D(1, 0, 0), -0.5),
        new Operation(new Vector3D(1, 0, 0), -0.25),

        new Operation(new Vector3D(0, 1, 0), 0.25),
        new Operation(new Vector3D(0, 1, 0), 0.5),
        new Operation(new Vector3D(0, 1, 0), 0.75),
        new Operation(new Vector3D(0, 1, 0), 1),
        new Operation(new Vector3D(0, 1, 0), -0.75),
        new Operation(new Vector3D(0, 1, 0), -0.5),
        new Operation(new Vector3D(0, 1, 0), -0.25),

        new Operation(new Vector3D(0, 0, 1), 0.25),
        new Operation(new Vector3D(0, 0, 1), 0.5),
        new Operation(new Vector3D(0, 0, 1), 0.75),
        new Operation(new Vector3D(0, 0, 1), 1),
        new Operation(new Vector3D(0, 0, 1), -0.75),
        new Operation(new Vector3D(0, 0, 1), -0.5),
        new Operation(new Vector3D(0, 0, 1), -0.25),

        new Operation(new Vector3D(1, 1, 0), 1.0),
    };
    public List<Operation> ops = new List<Operation>();
    private Quaternion accountedRotation = Quaternion.Identity;
    private Quaternion unaccountedRotation = Quaternion.Identity;

    private static bool isNear(Quaternion rotation, Operation op) {
        var axis = rotation.Axis;
        var power = rotation.Angle / 180;
        if (axis.X + axis.Y + axis.Z < 0) {
            axis *= -1;
            power *= -1;
        }
        axis.Normalize();
        axis *= op.axis.Length;
        var axis_err = (axis - op.axis).LengthSquared / op.axis.LengthSquared;
        var angle_err = Math.Abs(power - op.power) * 180;
        if (angle_err > 180) angle_err = Math.Abs(angle_err - 360);
        System.Diagnostics.Debug.WriteLine(axis_err + ", " + angle_err + ", " + op);
        return axis_err < 0.1 && angle_err < 10;
    }
    public void advance(Quaternion rotation) {
        unaccountedRotation = rotation;
        var r = accountedRotation;
        r.Conjugate();
        rotation *= r;
        unaccountedRotation = rotation;

        foreach (var op in KnownOperations) {
            if (isNear(rotation, op)) {
                var merged = ops.Count > 0 ? op.TryMerge(ops.Last()) : null;
                if (merged != null) {
                    ops[ops.Count - 1] = merged.Value;
                    if (ops.Last().power == 0) {
                        ops.RemoveAt(ops.Count - 1);
                    }
                } else {
                    ops.Add(op);
                }
                this.accountedRotation = new Quaternion(op.axis, op.power * 180) * this.accountedRotation;
                break;
            }
        }
    }

    public override string ToString() {
        return String.Join(", ", ops) + String.Format("; <{0:0.00}, {1:0.00}, {2:0.00}> {3:0.00}",
            this.unaccountedRotation.Axis.X,
            this.unaccountedRotation.Axis.Y,
            this.unaccountedRotation.Axis.Z,
            this.unaccountedRotation.Angle / 180);
    }
}

