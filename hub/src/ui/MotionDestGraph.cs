using System.Windows.Forms.DataVisualization.Charting;

class MotionDestGraph {
    private readonly Chart chart;

    public MotionDestGraph(Chart chart) {
        this.chart = chart;

        chart.ChartAreas[0].AxisX.MajorGrid.LineWidth = 0;
        chart.ChartAreas[0].AxisY.MajorGrid.LineWidth = 0;
        chart.ChartAreas[0].AxisY.Maximum = +4;
        chart.ChartAreas[0].AxisY.Minimum = -4;
    }

    public void showReading(MotionSourceReading reading) {
        var bump = reading.upward.X / 300 - 4;
        var ang = reading.deltaRotation.Axis * reading.deltaRotation.Angle / 13;

        var readings = new[] { bump, ang.X, ang.Y, ang.Z };

        for (var i = 0; i < readings.Length; i++) {
            chart.Series[i].Points.AddY(readings[i]);
            while (chart.Series[i].Points.Count > 40) {
                chart.Series[i].Points.RemoveAt(0);
            }
        }
    }
}
