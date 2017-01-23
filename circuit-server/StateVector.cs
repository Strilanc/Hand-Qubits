using System;
using System.Linq;
using MathNet.Numerics.LinearAlgebra;
using MathNet.Numerics.LinearAlgebra.Complex;
using System.Numerics;
using System.Windows.Media.Media3D;

public class StateVector {
    private static readonly Matrix<Complex> I = Matrix.Build.DenseOfRowArrays(
        new Complex[] { 1, 0 },
        new Complex[] { 0, 1 });
    public static readonly Matrix<Complex> X = Matrix.Build.DenseOfRowArrays(
        new Complex[] { 0, 1 },
        new Complex[] { 1, 0 });
    public static readonly Matrix<Complex> Y = Matrix.Build.DenseOfRowArrays(
        new Complex[] { 0, -Complex.ImaginaryOne },
        new Complex[] { Complex.ImaginaryOne, 0 });
    public static readonly Matrix<Complex> Z = Matrix.Build.DenseOfRowArrays(
        new Complex[] { 1, 0 },
        new Complex[] { 0, -1 });
    public static readonly Matrix<Complex> H = Matrix.Build.DenseOfRowArrays(
        new Complex[] { 1, 1 },
        new Complex[] { 1, -1 }) * Math.Sqrt(0.5);
    // Mark failed control values with NaN, so they can be noticed and replaced with identity matrix entries.
    private static readonly Matrix<Complex> C = Matrix.Build.DenseOfRowArrays(
        new Complex[] { new Complex(Double.NaN, Double.NaN), 0 },
        new Complex[] { 0, 1 });

    public readonly int qubitCount;
    private Vector<Complex> vector;
    private Random random = new Random();

    public StateVector(int qubitCount) {
        this.qubitCount = qubitCount;
        this.vector = Vector.Build.Dense(1 << qubitCount);
        this.vector[0] = 1;
    }

    public void rotateQubit(Quaternion rotation, int target, int? control = null) {
        var op = I * rotation.W + (X * rotation.X + Y * rotation.Y + Z * rotation.Z) * Complex.ImaginaryOne;
        rotateQubit(op, target, control);
    }

    private static Matrix<Complex> tensor(Matrix<Complex> left, Matrix<Complex> right) {
        var result = Matrix.Build.Dense(left.RowCount * right.RowCount, left.ColumnCount * right.ColumnCount);
        var row = 0;
        for (var row1 = 0; row1 < left.RowCount; row1++) {
            for (var row2 = 0; row2 < right.RowCount; row2++) {
                var col = 0;
                for (var col1 = 0; col1 < left.ColumnCount; col1++) {
                    for (var col2 = 0; col2 < right.ColumnCount; col2++) {
                        result[row, col] = left[row1, col1] * right[row2, col2];
                        col++;
                    }
                }
                row++;
            }
        }
        return result;
    }

    public void rotateQubit(Matrix<Complex> op, int target, int? control = null) {
        var ops = Enumerable.Range(0, qubitCount).Select(e => e == target ? op
                                                            : e == control ? C
                                                            : I).ToArray();
        var operation = ops.Reverse().Aggregate(tensor);
        for (int row = 0; row < operation.RowCount; row++) {
            for (int col = 0; col < operation.RowCount; col++) {
                if (Double.IsNaN(operation[row, col].Real)) {
                    operation[row, col] = row == col ? 1 : 0;
                }
            }
        }

        vector = operation * vector;
        normalize();
    }

    private void normalize() {
        var w = vector.Select(e => e.Real * e.Real + e.Imaginary * e.Imaginary).Sum();
        vector /= w;
    }

    public bool measureQubit(int target) {
        var t = 1 << target;
        var p = vector.Select((e, i) => (t & i) != 0 ? e.Real * e.Real + e.Imaginary * e.Imaginary : 0).Sum();
        var result = random.NextDouble() < p;
        for (int i = 0; i < vector.Count; i++) {
            if (((t & i) == 0) == result) {
                vector[i] = 0;
            }
        }
        normalize();
        return result;
    }
}
