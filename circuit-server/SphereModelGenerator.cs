using System;
using System.Windows;
using System.Windows.Media.Media3D;

namespace QubitServer {
    public class SphereModelGenerator {
        public MeshGeometry3D Geometry {
            get {
                return CalculateMesh();
            }
        }

        private MeshGeometry3D CalculateMesh() {
            const int numLats = 32;
            const int numLons = 32;
            var origin = new Point3D();
            MeshGeometry3D mesh = new MeshGeometry3D();

            // Generate lat/lon intersection points.
            for (int lon = 0; lon < numLons; lon++) {
                for (int lat = 0; lat < numLats; lat++) {
                    double pLat = (double)lat / numLats;
                    double pLon = (double)lon / (numLons - 1);

                    double phi = (pLon - 0.5) * Math.PI;
                    double theta = pLat * 2 * Math.PI;
                    double x = Math.Cos(phi) * Math.Sin(theta);
                    double y = Math.Cos(phi) * Math.Cos(theta);
                    double z = Math.Sin(phi);

                    Vector3D normal = new Vector3D(x, y, z);
                    mesh.Normals.Add(normal);
                    mesh.Positions.Add(origin + normal);
                    mesh.TextureCoordinates.Add(new Point(pLat, pLon));
                }
            }

            // Build lat/lon faces.
            for (int lon = 0; lon < numLons; lon++) {
                for (int lat = 0; lat < numLats; lat++) {
                    int cur = lon * numLats + lat;
                    int dwn = cur + numLats;

                    if (lon > 0) {
                        mesh.TriangleIndices.Add(cur);
                        mesh.TriangleIndices.Add(dwn);
                        mesh.TriangleIndices.Add(cur + 1);
                    }

                    if (lon < numLons) {
                        mesh.TriangleIndices.Add(cur + 1);
                        mesh.TriangleIndices.Add(dwn);
                        mesh.TriangleIndices.Add(dwn + 1);
                    }
                }
            }

            return mesh;
        }
    }
}
