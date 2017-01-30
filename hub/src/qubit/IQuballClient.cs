using System.Threading.Tasks;

/// <summary>
/// A connection to one of the qubit balls.
/// </summary>
public interface IQuballClient {
    Task Init();
    QuballReport NextReading();
    void TellOwnId(byte id);
    void TellMeasurementResult(bool result);
}
