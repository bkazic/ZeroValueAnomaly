#include "examples.h"

using namespace AnomalyDetection;

void TExamples::batchLearningExample() {
    // Testing in for loop
    printf("\n\nTesting model fitting in batc:\n");

    //// Test reading input data
    PJsonVal DataJson = TMisc::JsonFileReader("../ZeroValueAnomaly/data/dummy.json");
    TRecordV DataVec = TMisc::JsonArr2TRecordV(DataJson);

    // Testing model clas
    TZeroValModel AnomalyModel(5, true);

    AnomalyModel.Fit(DataVec);

    // Debugging
    //TMisc::Print3DMat(AnomalyModel.GetCounts());
    //TMisc::PrintMat(AnomalyModel.GetCountsAll());
    //TMisc::Print3DMat(ModelMat);

    // Alert thresholds
    TThresholdV ThresholdV;
    ThresholdV.Add(TThreshold(0.2, 2, "Medium"));
    ThresholdV.Add(TThreshold(0.1, 1, "High"));
    ThresholdV.Add(TThreshold(0.3, 3, "Low"));

    // Detect Alerts
    TAlertV Alerts;
    AnomalyModel.Predict(DataVec, ThresholdV, Alerts);
    printf("\nNumber of alerts: %i", Alerts.Len());
}

void TExamples::oneByOneLearningExample() {
    // Testing in for loop
    printf("\n\nTesting model fitting and learning record by record:\n");

    //// Test reading input data
    PJsonVal DataJson = TMisc::JsonFileReader("../ZeroValueAnomaly/data/dummy.json");
    TRecordV DataVec = TMisc::JsonArr2TRecordV(DataJson);

    // Testing model clas
    TZeroValModel AnomalyModel(5, true);

    // Alert thresholds
    TThresholdV ThresholdV;
    ThresholdV.Add(TThreshold(0.2, 2, "Medium"));
    ThresholdV.Add(TThreshold(0.1, 1, "High"));
    ThresholdV.Add(TThreshold(0.3, 3, "Low"));

    // Detect Alerts
    TAlertV Alerts;

    // Learning record by record
    const int Rows = DataVec.Len();
    for (int RowN = 0; RowN < Rows; RowN++) {
        // Fit model
        AnomalyModel.Fit(DataVec[RowN]);
    }

    // Predicting record by record
    for (int RowN = 0; RowN < Rows; RowN++) {
        // Fit model
        AnomalyModel.Predict(DataVec[RowN], ThresholdV, Alerts);
    }

    // Learned models
    //TMisc::Print3DMat(AnomalyModel.GetCounts());
    //TMisc::Print3DMat(AnomalyModel.GetProbabilities());
    //TMisc::PrintMat(AnomalyModel.GetCountsAll());

    // Detected alerts
    printf("\nNumber of alerts: %i", Alerts.Len());
}

void TExamples::resamplerSyntheticData() {
    // Testing in for loop
    printf("\n\nTesting resampler with synthetic data:\n");

    // Create resampler paramVal
    PJsonVal ParamVal = TJsonVal::NewObj();
    ParamVal->AddToObj("interval", 60 * 60 * 1000);
    ParamVal->AddToObj("aggType", "sum");
    ParamVal->AddToObj("roundStart", "h");
    ParamVal->AddToObj("defaultValue", 0);

    // Create resampler
    TSignalProc::TAggrResampler Resampler(ParamVal);

    // Resampler output values
    double ResampledValue = 0;
    uint64 ResampledTm = 0;
    bool FoundEmptyP = false;
    bool SkipEmptyP = false;

    double TestVal = 0.;
    uint64 TestTmMSecs = 10 * 60 * 60 * 1000;
    uint64 TestTmDiff = 10 * 60 * 1000; // 10 min

    for (int i = 0; i < 20; i++) {
        TestVal++;
        TestTmMSecs += TestTmDiff;

        printf("\nData: [%.0f, %f]", (double)TestTmMSecs, TestVal);

        // Push data into resampler
        Resampler.SetCurrentTm(TestTmMSecs);
        Resampler.AddPoint(TestVal, TestTmMSecs);

        // Call resampler until we get to the current timestamp
        while (Resampler.TryResampleOnce(ResampledValue, ResampledTm, FoundEmptyP)) {

            // notify out aggregate that new resampled values are available
            if (FoundEmptyP && SkipEmptyP) { continue; }

            printf("\nResampled Val: %f, ResampledTm: %.0f",
                ResampledValue, (double)ResampledTm);
            //Resampler.PrintState();

        }
    }
}

void TExamples::streamingExample() {
    // Testing in for loop
    printf("\n\nTesting stream learning with resampling:\n");

    //// Test reading input data
    //PJsonVal DataJson = TMisc::JsonFileReader("../ZeroValueAnomaly/data/dummy.json");
    PJsonVal DataJson = TMisc::JsonFileReader("../ZeroValueAnomaly/data/dummy_10min.json");
    TRecordV DataVec = TMisc::JsonArr2TRecordV(DataJson);

    // Create resampler paramVal
    PJsonVal ParamVal = TJsonVal::NewObj();
    ParamVal->AddToObj("interval", 60 * 60 * 1000);
    ParamVal->AddToObj("aggType", "sum");
    ParamVal->AddToObj("roundStart", "h");
    ParamVal->AddToObj("defaultValue", 0);

    // Create resampler
    TSignalProc::TAggrResampler Resampler(ParamVal);

    // Resampler output values
    double ResampledValue = 0;
    uint64 ResampledTm = 0;
    bool FoundEmptyP = false;
    bool SkipEmptyP = false;

    // Clear old instance of Anomaly Model
    TZeroValModel AnomalyModel(5, true);

    // New Alerts class
    TAlertV Alerts;

    // Alert thresholds
    TThresholdV ThresholdV;
    ThresholdV.Add(TThreshold(0.2, 2, "Medium"));
    ThresholdV.Add(TThreshold(0.1, 1, "High"));
    ThresholdV.Add(TThreshold(0.3, 3, "Low"));

    // Read dataset record by record
    const int Rows = DataVec.Len();
    for (int RowN = 0; RowN < Rows; RowN++) {
        const double Val = DataVec[RowN].GetValue();
        const uint64 Ts = DataVec[RowN].GetTimestamp();

        // Push data into resampler
        Resampler.SetCurrentTm(Ts);
        Resampler.AddPoint(Val, Ts);

        // Call resampler until we get to the current timestamp
        while (Resampler.TryResampleOnce(ResampledValue, ResampledTm, FoundEmptyP)) {

            // notify out aggregate that new resampled values are available
            if (FoundEmptyP && SkipEmptyP) { continue; }

            //printf("\nResampled Val: %f, ResampledTm: %.0f\n", ResampledValue, (double)ResampledTm);
            //Resampler.PrintState();

            //// Predict model
            AnomalyModel.Predict(TRecord(ResampledTm, ResampledValue), ThresholdV, Alerts);

            // Fit model
            AnomalyModel.Fit(TRecord(ResampledTm, ResampledValue));
        }
    }

    // Learned models
    //TMisc::Print3DMat(AnomalyModel.GetCounts());
    //TMisc::Print3DMat(AnomalyModel.GetProbabilities());
    //TMisc::PrintMat(AnomalyModel.GetCountsAll());

    // Detected alerts
    printf("\nNumber of alerts: %i", Alerts.Len());
}