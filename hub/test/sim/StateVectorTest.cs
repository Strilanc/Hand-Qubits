using Microsoft.VisualStudio.TestTools.UnitTesting;
using System;
using System.Windows.Media.Media3D;

[TestClass]
public class StateVectorTest {
    [TestMethod]
    public void TestMeasureImitialState() {
        StateVector v = new StateVector(1);
        Assert.IsFalse(v.measureQubit(0));

        StateVector v2 = new StateVector(2);
        Assert.IsFalse(v2.measureQubit(0));
        Assert.IsFalse(v2.measureQubit(1));

        StateVector v4 = new StateVector(4);
        Assert.IsFalse(v2.measureQubit(0));
        Assert.IsFalse(v2.measureQubit(1));
        Assert.IsFalse(v2.measureQubit(2));
        Assert.IsFalse(v2.measureQubit(3));
    }

    [TestMethod]
    public void TestMeasureAfterRotating() {
        StateVector v = new StateVector(4);
        Assert.IsFalse(v.measureQubit(0));
        Assert.IsFalse(v.measureQubit(0));
        v.rotateQubit(StateVector.X, 0);
        Assert.IsTrue(v.measureQubit(0));
        Assert.IsTrue(v.measureQubit(0));
        v.rotateQubit(new Quaternion(1, 0, 0, 0), 0);
        Assert.IsFalse(v.measureQubit(0));
        Assert.IsFalse(v.measureQubit(0));

        for (int i = 0; i < 4; i++) {
            v.rotateQubit(new Quaternion(new Vector3D(1, 0, 0), 90), i);
            var b = v.measureQubit(i);
            Assert.AreEqual(b, v.measureQubit(i));
        }
    }

    [TestMethod]
    public void TestBellPair() {
        for (int i = 0; i < 10; i++) {
            StateVector v = new StateVector(2);
            v.rotateQubit(StateVector.H, 0);
            v.rotateQubit(StateVector.X, 1, 0);
            var b = v.measureQubit(0);
            Assert.AreEqual(b, v.measureQubit(1));
        }
    }

    [TestMethod]
    public void TestBellInequalityViolation() {
        var r = new Random();
        var t = 0;
        for (int i = 0; i < 20000; i++) {
            StateVector v = new StateVector(2);
            v.rotateQubit(StateVector.H, 0);
            v.rotateQubit(StateVector.X, 1, 0);
            v.rotateQubit(new Quaternion(new Vector3D(1, 0, 0), -45), 0);

            var ref1 = r.NextDouble() < 0.5;
            var ref2 = r.NextDouble() < 0.5;

            if (ref1) {
                v.rotateQubit(new Quaternion(new Vector3D(1, 0, 0), 90), 0);
            }
            if (ref2) {
                v.rotateQubit(new Quaternion(new Vector3D(1, 0, 0), 90), 1);
            }

            var p1 = v.measureQubit(0);
            var p2 = v.measureQubit(1);

            if ((p1 ^ p2) == (ref1 && ref2)) {
                t++;
            }
        }
        Assert.IsTrue(t > 8000);
    }
}
