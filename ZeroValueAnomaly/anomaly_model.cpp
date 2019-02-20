#include "anomaly_model.h"

namespace AnomalyDetection {

///////////////////////////////
// Record object
TRecord::TRecord() {}

TRecord::TRecord(const uint64& _Timestamp, const double& _Value)
        : Timestamp(_Timestamp), Value(_Value) {}

///////////////////////////////
// Thresholds object
TThreshold::TThreshold() {}

TThreshold::TThreshold(const double& _Value, const int& _SeverityLevel,
                       const TStr& _AlertLabel)
        : Value(_Value), Severity(_SeverityLevel), Label(_AlertLabel) { }

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

int TModel::NumOfSeqValues(const TRecordV& RecordV, const int& CurrIdx) const {
    // TODO: Deprecated (not in use)
    // In case CurrIdx < Lags
    const int Hist = (CurrIdx < Lags) ? (CurrIdx + 1) : (Lags.Val);

    int SeqCnt = -1;
    for (int LagN = 0; LagN < Hist; LagN++) {
        if (RecordV[CurrIdx - LagN].GetValue() == ObservedValue) {
            SeqCnt = LagN;
        } else {
            break;
        }
    }
    return SeqCnt;
}

void TModel::UpdateSeqValCount(const double& Value) {
    // Update number of sequenced values or reset count
    if (LastValue == Value) {
        if (SeqValCount < (Lags - 1)) { // Max value is Lag
            SeqValCount++;
        }
    } else {
        SeqValCount = 0;
    }
    LastValue = Value;
}

void TModel::Fit(const TRecordV& RecordV) {
    PNotify LogNotify = Verbose ? Notify : TNotify::NullNotify;

    const int Rows = RecordV.Len();

    for (int RowN = 0; RowN < Rows; RowN++) {
        const TFlt CurrValue = RecordV[RowN].GetValue();
        const TUInt64 CurrTimestamp = RecordV[RowN].GetTimestamp();

        // Check and update last timestamp
        if (CurrTimestamp <= LastTimestamp) {
            // TODO: Just throw some notification
            // TODO: Delete this WarnNotify later (just for debugging)
            WarnNotify("Warning: Last timestamp is larger is same as current!");
            break; //TODO: Test if this work as you whish
        }
        LastTimestamp = CurrTimestamp;

        // Extract timestamp features
        const TTm Tm = TTm::GetTmFromMSecs(TTm::GetWinMSecsFromUnixMSecs(CurrTimestamp));
        const int Hour = Tm.GetHour();
        const int Day = Tm.GetDaysSinceMonday();

        // Update normalization matrix
        CountsAll(Hour, Day)++;

        for (int LagN = 0; LagN < Lags; LagN++) {

            // check if data index (positive) is valid
            if ((RowN - LagN) < 0) { break; }

            // If observed value (usually zero)
            if (RecordV[RowN - LagN].GetValue() == ObservedValue) {

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
}

void TModel::Fit(const TRecord& Record) {
    Fit(TRecordV::GetV(Record));
}

void TModel::Predict(const TRecordV& RecordV, TThresholdV ThresholdV,
                     TAlertV& AlertV) {
    PNotify LogNotify = Verbose ? Notify : TNotify::NullNotify;

    const int ThrLen = ThresholdV.Len();
    const int Rows = RecordV.Len();

    // Thresholds should be sorted in increasing order
    ThresholdV.Sort();

    // Iterate over dataset
    for (int RowN = 0; RowN < Rows; RowN++) {
        const TFlt CurrValue = RecordV[RowN].GetValue();
        const TUInt64 CurrTimestamp = RecordV[RowN].GetTimestamp();

        // Update number of sequenced values or reset count
        UpdateSeqValCount(CurrValue);

        // If observed value
        if (CurrValue == ObservedValue) {

            // Extract timestamp features
            const TTm Tm = TTm::GetTmFromMSecs(TTm::GetWinMSecsFromUnixMSecs(CurrTimestamp));
            const int Hour = Tm.GetHour();
            const int Day = Tm.GetDaysSinceMonday();

            // Get number of ObservedValues in a row
            //const int Lag = NumOfSeqValues(Data, RowN);
            const int Lag = SeqValCount;

            // Get probability for a specific bucket
            const double P = Probs(Hour, Day, Lag);

            // Check all the thresholds for alert
            for (int ThrN = 0; ThrN < ThrLen; ThrN++) {
                const TThreshold& Threshold = ThresholdV[ThrN];

                if (P < Threshold.GetValue()) {

                    // Push alert object to some vector
                    AlertV.Add(TAlert(CurrTimestamp, Threshold.GetSeverity(), Threshold));

                    // Debug logger
                    LogNotify->OnStatusFmt("[Ts: %.0f, Severity: %i] "
                        "Detected %s severity alert! Lag: %i (%.2f < %.2f)",
                        (double)CurrTimestamp, Threshold.GetSeverity(),
                        Threshold.GetLabel().CStr(), Lag, P, Threshold.GetValue());

                    break;
                }
            }

        }

    }
}

void TModel::Predict(const TRecord& Record, TThresholdV ThresholdV,
                     TAlertV& AlertV) {
    Predict(TRecordV::GetV(Record), ThresholdV, AlertV);
}

void TModel::FitPredict(const TRecordV& RecordV, TThresholdV ThresholdV,
                        TAlertV& AlertV) {
    Predict(RecordV, ThresholdV, AlertV);
    Fit(RecordV);
}

void TModel::FitPredict(const TRecord& Record, TThresholdV ThresholdV,
                        TAlertV& AlertV) {
    Predict(Record, ThresholdV, AlertV);
    Fit(Record);
}

void TModel::Clear() {
    Init();
}

void TModel::Save() {
    // TODO
    // Everything from Init()
}

void TModel::Load() {
    // TODO
    // Maybe create new Init method with all the instances
}

} // namespace AnomalyDetection