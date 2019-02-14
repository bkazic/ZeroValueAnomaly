#ifndef ANOMALY_MODEL_H
#define ANOMALY_MODEL_H

#include "base.h"

namespace AnomalyDetection {

///////////////////////////////
// Thresholds object
// (used by Model.Detect())
struct TThreshold {
    TFlt Val;
    TInt Severity;
    TStr Label;

    TThreshold();
    TThreshold(const double& Value, const int& SeverityLevel);
    TThreshold(const double& Value, const int& SeverityLevel,
        const TStr& AlertLabel);
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
    TInt Lags = 5;
    TFlt ObservedValue = 0.;

    const TInt TimestampIdx = 0; //< First should be timestamp
    const TInt ValueIdx = 1; //< Second should be value

    const TInt Days = 7;
    const TInt Hours = 24;

    TFltVV CountsAll;
    TFltVVV Counts;
    TFltVVV Probs;

    /// Initialize matrices
    void Init();
    /// Normalize input 3D matrix (Mat), layer by layer (over ZDim)
    /// using normalization matrix (Norm)
    void Normalize(const TFltVVV& Mat, const TFltVV& Norm, TFltVVV& Res);
    /// Get number of ObservedValues (usually zero) in a row
    int NumOfSeqValues(const TFltVV& Data, const int& CurrIdx) const;

public:
    TModel();
    TModel(const int& LagsNum);

    /// Construct the probability matrix from the provided data (Data)
    TFltVVV Fit(const TFltVV& Data, const bool& Verbose = true);
    /// Detect alerts from the dataset (Data), using provided thresholds (Thresholds)
    void Detect(const TFltVV& Data, const TThresholdV& PThresholds,
        TAlertV& Alerts) const;
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