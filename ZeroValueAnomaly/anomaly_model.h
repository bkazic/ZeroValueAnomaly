#ifndef ANOMALY_MODEL_H
#define ANOMALY_MODEL_H

#include "base.h"

namespace AnomalyDetection {

// TODO: Think about changing this co class and having Getters

///////////////////////////////
// Thresholds object
// (used by Model.Detect())
struct TThreshold {
    TFlt Value;
    TInt Severity;
    TStr Label;

    TThreshold();
    TThreshold(const double& Value, const int& SeverityLevel);
    TThreshold(const double& Value, const int& SeverityLevel,
        const TStr& AlertLabel);

    /// Override comparator for sorting
    bool operator<(const TThreshold& Threshold) const {
        return Value < Threshold.Value;
    }
};

///////////////////////////////
// Alert object
// (returned by Model.Detect)
struct TAlert {
    TUInt64 Timestamp;
    TInt AlertSeverity;
    TThreshold Info;

    TAlert();
    TAlert(const uint64& Ts, const int& Severity);
    TAlert(const uint64& Ts, const int& Severity,
        const TThreshold& AlertInfo);
};

typedef TVec<TAlert> TAlertV;
typedef TVec<TThreshold> TThresholdV;

///////////////////////////////
// Model
class TModel {
private:
    bool Verbose = false; //< Verbose or not
    PNotify Notify = TStdNotify::New(); //< Info logger

    TFlt ObservedValue = 0.; //< Observed value which we are modeling
    TInt Lags = 5; //< Number of sequential values to observe (model)

    const TInt TimestampIdx = 0; //< First should be timestamp
    const TInt ValueIdx = 1; //< Second should be value

    const TInt Days = 7; //< Defines first dimension
    const TInt Hours = 24; //< Defines second dimension

    TFltVV CountsAll; //< All seen instances (used for normalization)
    TFltVVV Counts; //< Instances that meets the condition (Val == ObservedVal)
    TFltVVV Probs; //< Probabilities (normalized Counts matrix)

    /// Initialize matrices
    void Init();
    /// Normalize input 3D matrix (Mat), layer by layer (over ZDim)
    /// using normalization matrix (Norm)
    void Normalize(const TFltVVV& Mat, const TFltVV& Norm, TFltVVV& Res);
    /// Get number of ObservedValues (usually zero) in a row
    int NumOfSeqValues(const TFltVV& Data, const int& CurrIdx) const;

public:
    TModel();
    TModel(const int& _Lags);
    TModel(const bool& _Verbose);
    TModel(const int& _Lags, const bool& _Verbose);

    /// Construct the probability matrix from the provided data (Data)
    void Fit(const TFltVV& Data);
    void Fit(const TFltV& Record);
    /// Detect alerts from the dataset (Data), using provided thresholds
    void Predict(const TFltVV& Data, TThresholdV PThresholds,
        TAlertV& PAlertV) const;
    void Predict(const TFltV& Record, TThresholdV PThresholds,
        TAlertV& PAlertV) const;
    /// First predict and then update the model
    void FitPredict(const TFltVV& Data, TThresholdV PThresholds,
        TAlertV& PAlertV);
    void FitPredict(const TFltV& Record, TThresholdV PThresholds,
        TAlertV& PAlertV);
    /// Clears (reinitializes) models
    void Clear();
    /// Set verbose
    void SetVerbose(const bool& _Verbose);
    /// Get verbose
    bool GetVerbose();
    /// Get number of Lags of the model
    int GetLags();
    /// Get which value is the model focusing on (calculating prob matrix)
    double GetObservedValue();
    /// Get all seen instances (counts), this is used for normalization
    TFltVV GetCountsAll();
    /// Get instances (counts) that meets the condition (Value == ObservedVal)
    TFltVVV GetCounts();
    /// Get probabilities (normalized Counts matrix, using CountsALl)
    TFltVVV GetProbabilities();
    /// Save model
    void Save();
    /// Load model
    void Load();
};

} // namespace AnomalyDetection

#endif