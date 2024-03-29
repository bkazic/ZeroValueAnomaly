#include "anomaly_model.h"

namespace TAnomalyDetection {

///////////////////////////////
// Record object
TTimeValue::TTimeValue() {}

TTimeValue::TTimeValue(const uint64& _Timestamp, const double& _Value)
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
// Number of observed vals
TZeroValModel::SeqValues::SeqValues() { }

TZeroValModel::SeqValues::SeqValues(const int& Cnt, const int& Max)
        : Count(Cnt), MaxSize(Max) { }

void TZeroValModel::SeqValues::Update(const TTimeValue& Record) {
    if (LastRecord.GetValue() == Record.GetValue()) {
        if (Count < MaxSize ) {
            Count++;
        }
    } else {
        Count = 0;
    }
    LastRecord = Record;
}

///////////////////////////////
// Model
TZeroValModel::TZeroValModel(const int& _Lags, const bool& _Verbose)
        : Lags(_Lags), Verbose(_Verbose) {
    Init();
}

void TZeroValModel::Init() {
    Counts = TFltVVV(Hours, Days, Lags);
    CountsAll = TFltVV(Hours, Days);
    Probs = TFltVVV(Hours, Days, Lags);

    SeqValsFit = TZeroValModel::SeqValues(0, Lags - 1);
    SeqValsPredict = TZeroValModel::SeqValues(0, Lags - 1);
}

void TZeroValModel::Normalize(const TFltVVV& Mat, const TFltVV& Norm, TFltVVV& Res) {
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

void TZeroValModel::Fit(const TRecordV& RecordV) {
    PNotify LogNotify = Verbose ? Notify : TNotify::NullNotify;

    const int Rows = RecordV.Len();

    for (int RowN = 0; RowN < Rows; RowN++) {
        TTimeValue Record = RecordV[RowN];
        const double CurrValue = Record.GetValue();
        const uint64 CurrTimestamp = Record.GetTimestamp();

        // Check and update last timestamp
        if (CurrTimestamp <= SeqValsFit.GetLastRecord().GetTimestamp()) {
            // TODO: Just throw some notification
            // TODO: Delete this WarnNotify later (just for debugging)
            WarnNotify("Warning: Last timestamp is larger or same as current!");
            break;
        }
        SeqValsFit.Update(Record);

        // Extract timestamp features
        const TTm Tm = TTm::GetTmFromMSecs(TTm::GetWinMSecsFromUnixMSecs(CurrTimestamp));
        const int Hour = Tm.GetHour();
        const int Day = Tm.GetDaysSinceMonday();

        // Update normalization matrix
        CountsAll(Hour, Day)++;

        // If observed value
        if (CurrValue == ObservedValue()) {

            // Iterate over number of sequenced values
            for (int LagN = 0; LagN <= SeqValsFit.GetCount(); LagN++) {

                // Increase count
                Counts(Hour, Day, LagN)++;

                // Debug logger
                //LogNotify->OnStatusFmt("Observed Value: %.0f ==> "
                //    "[Day: %i, Hour: %i, Lag: %i], Counts: %.0f",
                //    ObservedValue, Day, Hour, LagN, Counts(Hour, Day, LagN).Val);
            }
        }
    }

    // Normalize (compute probabilities from counts)
    Normalize(Counts, CountsAll, Probs);
}

void TZeroValModel::Fit(const TTimeValue& Record) {
    Fit(TRecordV::GetV(Record));
}

void TZeroValModel::Predict(const TRecordV& RecordV, TThresholdV ThresholdV,
                     TAlertV& AlertV) {
    PNotify LogNotify = Verbose ? Notify : TNotify::NullNotify;

    const int ThrLen = ThresholdV.Len();
    const int Rows = RecordV.Len();

    // Thresholds should be sorted in increasing order
    ThresholdV.Sort();

    // Iterate over dataset
    for (int RowN = 0; RowN < Rows; RowN++) {
        TTimeValue Record = RecordV[RowN];
        const double CurrValue = Record.GetValue();
        const uint64 CurrTimestamp = Record.GetTimestamp();

        // Update number of sequenced values or reset count
        SeqValsPredict.Update(Record);

        // If observed value
        if (CurrValue == ObservedValue) {

            // Extract timestamp features
            const TTm Tm = TTm::GetTmFromMSecs(TTm::GetWinMSecsFromUnixMSecs(CurrTimestamp));
            const int Hour = Tm.GetHour();
            const int Day = Tm.GetDaysSinceMonday();

            // Get number of ObservedValues in a row
            const int Lag = SeqValsPredict.GetCount();

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

void TZeroValModel::Predict(const TTimeValue& Record, TThresholdV ThresholdV,
                     TAlertV& AlertV) {
    Predict(TRecordV::GetV(Record), ThresholdV, AlertV);
}

void TZeroValModel::FitPredict(const TRecordV& RecordV, TThresholdV ThresholdV,
                        TAlertV& AlertV) {
    Predict(RecordV, ThresholdV, AlertV);
    Fit(RecordV);
}

void TZeroValModel::FitPredict(const TTimeValue& Record, TThresholdV ThresholdV,
                        TAlertV& AlertV) {
    Predict(Record, ThresholdV, AlertV);
    Fit(Record);
}

void TZeroValModel::Clear() {
    Init();
}

void TZeroValModel::Save() {
    // TODO
    // Everything from Init()
}

void TZeroValModel::Load() {
    // TODO
    // Maybe create new Init method with all the instances
}

} // namespace TAnomalyDetection