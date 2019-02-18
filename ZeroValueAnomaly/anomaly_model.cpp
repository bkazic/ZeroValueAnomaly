#include "anomaly_model.h"

namespace AnomalyDetection {

///////////////////////////////
// Thresholds object
TThreshold::TThreshold() {}

TThreshold::TThreshold(const double& Value, const int& SeverityLevel) :
    Value(Value), Severity(SeverityLevel) { }

TThreshold::TThreshold(const double& Value, const int& SeverityLevel,
    const TStr& AlertLabel): Value(Value), Severity(SeverityLevel),
    Label(AlertLabel) { }

///////////////////////////////
// Alert object
TAlert::TAlert() {}

TAlert::TAlert(const uint64& Ts, const int& Severity) : Timestamp(Ts),
    AlertSeverity(Severity) { }

TAlert::TAlert(const uint64& Ts, const int& Severity,
    const TThreshold& AlertInfo) : Timestamp(Ts), AlertSeverity(Severity),
    Info(AlertInfo) { }

///////////////////////////////
// Model
TModel::TModel() {
    Init();
};

TModel::TModel(const int& LagsNum) {
    Lags = LagsNum;
    Init();
}

void TModel::Init() {
    Counts = TFltVVV(Hours, Days, Lags);
    CountsAll = TFltVV(Hours, Days);
    Probs = TFltVVV(Hours, Days, Lags);
}

void TModel::Normalize(const TFltVVV& Mat, const TFltVV& Norm, TFltVVV& Res) {
    const TInt XDim = Mat.GetXDim();
    const TInt YDim = Mat.GetYDim();
    const TInt ZDim = Mat.GetZDim();

    for (int X = 0; X < XDim; X++) {
        for (int Y = 0; Y < YDim; Y++) {
            for (int Z = 0; Z < ZDim; Z++) {
                Res(X, Y, Z) = Mat(X, Y, Z) / Norm(X, Y);
            }
        }
    }
}

int TModel::NumOfSeqValues(const TFltVV& Data, const int& CurrIdx) const {
    // In case CurrIdx < Lags
    const int Hist = (CurrIdx < Lags) ? (CurrIdx + 1) : (Lags.Val);

    int SeqCnt = -1;
    for (int LagN = 0; LagN < Hist; LagN++) {
        if (Data(CurrIdx - LagN, ValueIdx) == ObservedValue) {
            SeqCnt = LagN;
        } else {
            break;
        }
    }
    return SeqCnt;
}


TFltVVV TModel::Fit(const TFltVV& Data, const bool& Verbose) {
    // TODO: Remember last timestamp and ensure that new data timestamp is larger
    // TODO: Enable accepting TFltV Data, also if only one record is sent in

    const int XDim = Data.GetXDim();
    for (int RowN = 0; RowN < XDim; RowN++) {

        const uint64 Epoch = uint64((int64)Data(RowN, TimestampIdx));
        const TTm Tm = TTm::GetTmFromMSecs(
            TTm::GetWinMSecsFromUnixMSecs(Epoch));
        const int Hour = Tm.GetHour();
        const int Day = Tm.GetDaysSinceMonday();

        // TODO: Debugging, delete this later
        if (Verbose) {
            printf("\n%i [%s]: (%.0f, %.2f)", RowN, Tm.GetStr().CStr(),
                Data(RowN, TimestampIdx).Val, Data(RowN, ValueIdx).Val);
        }

        // Update normalization matrix
        CountsAll(Hour, Day)++;

        for (int LagN = 0; LagN < Lags; LagN++) {

            // check if valid data index (positive)
            if (RowN - LagN < 0) { break; }

            // If observed value (usually zero)
            if (Data(RowN - LagN, ValueIdx) == ObservedValue) {

                // TODO: Debugging, delete this later
                if (Verbose) {
                    printf("ZERO VAL ==> [Day: %i, Hour: %i, Lag: %i] = %.0f",
                        Day, Hour, LagN, Counts(Day, Hour, LagN).Val);
                }

                // Increase count
                Counts(Hour, Day, LagN)++;
            } else {
                break;
            }
        }
    }

    // Normalize (compute probabilities from counts)
    Normalize(Counts, CountsAll, Probs);

    return Probs;
}

void TModel::Detect(const TFltVV& Data, TThresholdV ThresholdV,
    TAlertV& PAlertV) const {
    // TODO: Check if model was fitted yet. If not, you can use Fit here.

    // Thresholds should be sorted in increasing order
    ThresholdV.Sort();

    const int ThrLen = ThresholdV.Len();

    for (int RowN = 0; RowN < Data.GetXDim(); RowN++) {

        if (Data(RowN, ValueIdx) == ObservedValue) {

            // TODO: maybe this could be wraped in a function (extract hour and days)
            const uint64 Epoch = uint64((int64)Data(RowN, TimestampIdx));
            const TTm Tm = TTm::GetTmFromMSecs(
                TTm::GetWinMSecsFromUnixMSecs(Epoch));
            const int Hour = Tm.GetHour();
            const int Day = Tm.GetDaysSinceMonday();

            // Get number of ObservedValues in a row
            const int Lag = NumOfSeqValues(Data, RowN);

            // Get probability for a specific bucket
            const double P = Probs(Hour, Day, Lag);

            // Check all the thresholds for alert
            for (int ThrN = 0; ThrN < ThrLen; ThrN++) {
                const TThreshold& Threshold = ThresholdV[ThrN];

                if (P < Threshold.Value) {

                    // Push alert object to some vector
                    PAlertV.Add(TAlert(Epoch, Threshold.Severity, Threshold));

                    // TODO: Debug print out. Delte this later.
                    printf("\n[Ts: %.0f, Severity: %i] Detected %s "
                        "severity alert! Lag: %i (%.2f < %.2f)",
                        (double)Epoch, Threshold.Severity.Val,
                        Threshold.Label.CStr(), Lag, P, Threshold.Value.Val);

                    break;
                }
            }

        }

    }
}

int TModel::GetLags() {
    return Lags;
}

double TModel::GetObservedValue() {
    return ObservedValue;
}

TFltVV TModel::GetCountsAll() {
    return CountsAll;
}

TFltVVV TModel::GetCounts() {
    return Counts;
}

TFltVVV TModel::GetProbabilities() {
    return Probs;
}

void TModel::Save() {
    // TODO
}

void TModel::Load() {
    // TODO
}

} // namespace AnomalyDetection