#include "base.h"
#include "misc.h"
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
    TMisc::LineReader("test.txt");
    TMisc::Timestamps();

    //// Test reading input data
    PJsonVal DataJson = TMisc::JsonFileReader("dummy.csv");
    TFltVV DataMat = TMisc::JsonArr2TFltVV(DataJson);

    // Testing model clas
    TModel AnomalyModel(5, true);
    //AnomalyModel.SetVerbose(false);
    //TFltVVV ModelMat = AnomalyModel.Fit(DataMat);
    AnomalyModel.Fit(DataMat);

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
    AnomalyModel.Predict(DataMat, ThresholdV, Alerts);
    printf("\nNumber of alerts: %i", Alerts.Len());

    // Test detecting alert based on one record
    printf("\nTesting only one record:\n");
    TFltV Record(2);
    Record[0] = 0; // Timestamp
    Record[1] = 5; // Value

    AnomalyModel.Predict(Record, ThresholdV, Alerts);

    //Env = TEnv(0, NULL, TNotify::StdNotify);

    PNotify Notify = TStdNotify::New();

    Notify->OnNotifyFmt(TNotifyType::ntInfo, "\nOk this is it %i %i", 2, 3);

    Notify->OnStatusFmt("\nTest 2  %i %i", 2, 3);

    return 0;
}