#include "misc.h"
#include <base.h>
#include <mine.h>
//#include "qminer.h"
#include "anomaly_model.h"

using namespace AnomalyDetection;

int main()
{
    // Init zero values matrix
    TFltVV TestMat = TFltVV(3, 2);

    // Tests
    TMisc::MatInfo(TestMat);
    TMisc::PrintMat(TestMat);
    TMisc::RndMat(TestMat);
    TMisc::PrintMat(TestMat);
    TMisc::LineReader("../ZeroValueAnomaly/test.txt");
    TMisc::Timestamps();

    //// Test reading input data
    PJsonVal DataJson = TMisc::JsonFileReader("../ZeroValueAnomaly/dummy.csv");
    TFltVV DataMat = TMisc::JsonArr2TFltVV(DataJson);
    TRecordV DataVec = TMisc::JsonArr2TRecordV(DataJson);

    // Testing model clas
    TModel AnomalyModel(5, true);
    //AnomalyModel.SetVerbose(false);
    //TFltVVV ModelMat = AnomalyModel.Fit(DataMat);
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

    // Test detecting alert based on one record
    //printf("\nTesting only one record:\n");
    //AnomalyModel.Fit(TRecord(0, 0));
    //AnomalyModel.Predict(TRecord(0, 0), ThresholdV, Alerts);

    //Env = TEnv(0, NULL, TNotify::StdNotify);

    PNotify Notify = TStdNotify::New();

    Notify->OnNotifyFmt(TNotifyType::ntInfo, "\nOk this is it %i %i", 2, 3);

    Notify->OnStatusFmt("\nTest 2  %i %i", 2, 3);

    // Testing Resampler

    //TODO: Create resampler paramVal
    PJsonVal ParamVal = TJsonVal::NewObj();
    ParamVal->AddToObj("interval", 0);
    ParamVal->AddToObj("aggType", "sum");
    ParamVal->AddToObj("roundStart", "h");
    ParamVal->AddToObj("defaultValue", 0);

    TSignalProc::TAggrResampler Resampler(ParamVal);


    const uint64 NewTmMSecs = 0;
    const double NewVal = 0.;

    // Set current time
    Resampler.SetCurrentTm(NewTmMSecs);
    // Add record to the buffer
    Resampler.AddPoint(NewVal, NewTmMSecs);
    Resampler.PrintState();

    // Resample
    double ResampledValue = 0;
    uint64 ResampledTm = 0;
    bool FoundEmptyP = false;
    Resampler.TryResampleOnce(ResampledValue, ResampledTm, FoundEmptyP);

    Resampler.PrintState();

    return 0;
}