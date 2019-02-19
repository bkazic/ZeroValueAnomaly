#include "anomaly_model.h"

namespace AnomalyDetection {

///////////////////////////////
// Thresholds object
TThreshold::TThreshold() {}

TThreshold::TThreshold(const double& Value, const int& SeverityLevel)
        : Value(Value), Severity(SeverityLevel) { }

TThreshold::TThreshold(const double& Value, const int& SeverityLevel,
                       const TStr& AlertLabel)
        : Value(Value), Severity(SeverityLevel), Label(AlertLabel) { }

///////////////////////////////
// Alert object
TAlert::TAlert() {}

TAlert::TAlert(const uint64& Ts, const int& Severity)
        : Timestamp(Ts), AlertSeverity(Severity) { }

TAlert::TAlert(const uint64& Ts, const int& Severity,
               const TThreshold& AlertInfo)
        : Timestamp(Ts), AlertSeverity(Severity), Info(AlertInfo) { }

///////////////////////////////
// Model
TModel::TModel() {
    Init();
};

TModel::TModel(const int& _Lags) : Lags(_Lags) {
    Init();
}

TModel::TModel(const bool& _Verbose) : Verbose(_Verbose) {
    Init();
}

TModel::TModel(const int& _Lags, const bool& _Verbose)
        : Lags(_Lags), Verbose(_Verbose) {
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
    // TODO: This should be rewritten in a way that it has its own circular buffer (of the same size as Lags)
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


void TModel::Fit(const TFltVV& Data) {
    // TODO: Remember last timestamp and ensure that new data timestamp is larger

    PNotify LogNotify = Verbose ? Notify : TNotify::NullNotify;

    const int Rows = Data.GetXDim();

    for (int RowN = 0; RowN < Rows; RowN++) {

        const uint64 Epoch = uint64((int64)Data(RowN, TimestampIdx));
        const TTm Tm = TTm::GetTmFromMSecs(
            TTm::GetWinMSecsFromUnixMSecs(Epoch));
        const int Hour = Tm.GetHour();
        const int Day = Tm.GetDaysSinceMonday();

        // Update normalization matrix
        CountsAll(Hour, Day)++;

        for (int LagN = 0; LagN < Lags; LagN++) {

            // check if valid data index (positive)
            if (RowN - LagN < 0) { break; }

            // If observed value (usually zero)
            if (Data(RowN - LagN, ValueIdx) == ObservedValue) {

                //// Debug logger
                //LogNotify->OnStatusFmt("Observed Value: %.0f ==> "
                //    "[Day: %i, Hour: %i, Lag: %i], Counts: %.0f",
                //    ObservedValue, Day, Hour, LagN, Counts(Hour, Day, LagN).Val);

                // Increase count
                Counts(Hour, Day, LagN)++;
            } else {
                break;
            }
        }
    }

    // Normalize (compute probabilities from counts)
    Normalize(Counts, CountsAll, Probs);

    Probs;
}

void TModel::Fit(const TFltV& Record) {
    Fit(TFltVV (Record));
}

void TModel::Predict(const TFltVV& Data, TThresholdV ThresholdV,
    TAlertV& PAlertV) const {
    PNotify LogNotify = Verbose ? Notify : TNotify::NullNotify;

    const int ThrLen = ThresholdV.Len();
    const int Rows = Data.GetXDim();

    // Thresholds should be sorted in increasing order
    ThresholdV.Sort();

    for (int RowN = 0; RowN < Rows; RowN++) {

        if (Data(RowN, ValueIdx) == ObservedValue) {

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

                    // Debug logger
                    LogNotify->OnStatusFmt("[Ts: %.0f, Severity: %i] "
                        "Detected %s severity alert! Lag: %i (%.2f < %.2f)",
                        (double)Epoch, Threshold.Severity.Val,
                        Threshold.Label.CStr(), Lag, P, Threshold.Value.Val);

                    break;
                }
            }

        }

    }
}

void TModel::Predict(const TFltV& Record, TThresholdV ThresholdV,
    TAlertV& PAlertV) const {
    Predict(TFltVV(Record), ThresholdV, PAlertV);
}

void TModel::FitPredict(const TFltVV& Data, TThresholdV ThresholdV,
    TAlertV& PAlertV) {
    Predict(Data, ThresholdV, PAlertV);
    Fit(Data);
}

void TModel::FitPredict(const TFltV& Record, TThresholdV ThresholdV,
    TAlertV& PAlertV) {
    Predict(Record, ThresholdV, PAlertV);
    Fit(Record);
}

void TModel::Clear() {
    Init();
}

void TModel::SetVerbose(const bool& _Verbose) {
    Verbose = _Verbose;
}

bool TModel::GetVerbose() {
    return Verbose;
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