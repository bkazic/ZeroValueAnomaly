#ifndef ANOMALY_MODEL_H
#define ANOMALY_MODEL_H

#include "base.h"

namespace AnomalyDetection {

///////////////////////////////
// Record object
// (used for wrapping timestamp and value into a record)
class TRecord {
private:
    TUInt64 Timestamp;
    TFlt Value;

public:
    TRecord();
    TRecord(const uint64& Timestamp, const double& Value);

    uint64 GetTimestamp() const { return Timestamp; }
    double GetValue() const { return Value; }
};

///////////////////////////////
// Thresholds object
// (used by Model.Detect())
class TThreshold {
private:
    TFlt Value;
    TInt Severity;
    TStr Label;

public:
    TThreshold();
    TThreshold(const double& Value, const int& SeverityLevel,
               const TStr& AlertLabel = TStr());

    /// Override comparator for sorting
    bool operator<(const TThreshold& Threshold) const {
        return Value < Threshold.Value;
    }

    double GetValue() const { return Value; }
    int GetSeverity() const { return Severity; }
    TStr GetLabel() const { return Label; }
};

///////////////////////////////
// Alert object
// (returned by Model.Detect)
class TAlert {
private:
    TUInt64 Timestamp;
    TInt AlertSeverity;
    TThreshold Info;

public:
    TAlert();
    TAlert(const uint64& Ts, const int& Severity);
    TAlert(const uint64& Ts, const int& Severity, const TThreshold& AlertInfo);

    uint64 GetTimestamp() const { return Timestamp; }
    int GetAlertSeverity() const { return AlertSeverity; }
    TThreshold GetInfo() const { return Info; }
};

typedef TVec<TAlert> TAlertV;
typedef TVec<TThreshold> TThresholdV;
typedef TVec<TRecord> TRecordV;

///////////////////////////////
// Model
class TModel {
private:
    bool Verbose; //< Verbose or not
    PNotify Notify = TStdNotify::New(); //< Info logger

    TFlt ObservedValue; //< Observed value which we are modeling
    TInt Lags; //< Number of sequential values to observe (model)

    const TInt Days = 7; //< Defines first dimension
    const TInt Hours = 24; //< Defines second dimension

    TFltVV CountsAll; //< All seen instances (used for normalization)
    TFltVVV Counts; //< Instances that meets the condition (Val == ObservedVal)
    TFltVVV Probs; //< Probabilities (normalized Counts matrix)

    TUInt64 LastTimestamp; //< Last seen timestamp when fitting
    TFlt LastValue; //< Last seen value when predicting
    TInt SeqValCount; //< Number of ObservedVales in a row

    /// Initialize matrices
    void Init();
    /// Normalize input 3D matrix (Mat), layer by layer (over ZDim)
    /// using normalization matrix (Norm)
    void Normalize(const TFltVVV& Mat, const TFltVV& Norm, TFltVVV& Res);
    /// [DEPRECATED] Get number of ObservedValues in a row
    int NumOfSeqValues(const TRecordV& PRecordV, const int& CurrIdx) const;
    /// Update sequenced values count
    void UpdateSeqValCount(const double& Value);

public:
    TModel(const int& _Lags = 5, const bool& _Verbose = false);

    /// Construct the probability matrix from the provided data (Data)
    void Fit(const TRecordV& RecordV);
    void Fit(const TRecord& Record);
    /// Detect alerts from the dataset (Data), using provided thresholds
    void Predict(const TRecordV& RecordV, TThresholdV ThresholdV,
                 TAlertV& AlertV);
    void Predict(const TRecord& Record, TThresholdV ThresholdV,
                 TAlertV& AlertV);
    /// First predict and then update the model
    void FitPredict(const TRecordV& RecordV, TThresholdV ThresholdV,
                    TAlertV& AlertV);
    void FitPredict(const TRecord& Record, TThresholdV ThresholdV,
                    TAlertV& AlertV);
    /// Clears (reinitializes) models
    void Clear();
    /// Save model
    void Save();
    /// Load model
    void Load();

    /// Set verbose
    void SetVerbose(const bool& _Verbose) { Verbose = _Verbose; }
    /// Get verbose
    bool const GetVerbose() { return Verbose; }
    /// Get number of Lags of the model
    int const GetLags() { return Lags; }
    /// Get which value is the model focusing on (calculating prob matrix)
    double const GetObservedValue() { return ObservedValue; }
    /// Get all seen instances (counts), this is used for normalization
    TFltVV const GetCountsAll() { return CountsAll; }
    /// Get instances (counts) that meets the condition (Value == ObservedVal)
    TFltVVV const GetCounts() { return Counts; }
    /// Get probabilities (normalized Counts matrix, using CountsALl)
    TFltVVV const GetProbabilities() { return Probs; }
};

} // namespace AnomalyDetection

#endif