using System.Windows.Media.Media3D;
using InTheHand.Net;

static class KnownBoard {
    public static readonly BoardDescription Aldo = new BoardDescription(
        "Aldo",
        new BluetoothAddress(new byte[] { 0xD2, 0x5C, 0x80, 0x33, 0xD3, 0x98, 0, 0 }),
        new Quaternion(new Vector3D(1, 1, 1), -120));

    public static readonly BoardDescription Bip = new BoardDescription(
        "Bip",
        new BluetoothAddress(new byte[] { 0x52, 0x11, 0xFC, 0x31, 0xD3, 0x98, 0, 0 }),
        new Quaternion(new Vector3D(1, 1, 1), -120));

    public static readonly BoardDescription Colu = new BoardDescription(
        "Colu",
        new BluetoothAddress(new byte[] { 0x8A, 0x18, 0xFB, 0x31, 0xD3, 0x98, 0, 0 }),
        new Quaternion(new Vector3D(1, 1, 1), -120));

    public static readonly BoardDescription Dask = new BoardDescription(
        "Dask",
        new BluetoothAddress(new byte[] { 0x9D, 0x53, 0x90, 0x34, 0xD3, 0x98, 0, 0 }),
        new Quaternion(new Vector3D(1, 1, 1), -120));

    public static readonly BoardDescription Elto = new BoardDescription(
        "Elto",
        new BluetoothAddress(new byte[] { 0xBC, 0x53, 0x70, 0x32, 0xD3, 0x98, 0, 0 }),
        new Quaternion(new Vector3D(1, 1, 1), -120));
}
