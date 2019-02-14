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
    TModel AnomalyModel(5); //TODO: why doesent it work without input parameter
    TFltVVV ModelMat = AnomalyModel.Fit(DataMat, false);

    // Debugging
    TMisc::Print3DMat(AnomalyModel.GetCounts());
    TMisc::PrintMat(AnomalyModel.GetCountsAll());
    TMisc::Print3DMat(ModelMat);

    // Alert thresholds
    TThresholdV Thresholds;
    Thresholds.Add(TThreshold(0.1, 1, "High"));
    Thresholds.Add(TThreshold(0.2, 2, "Medium"));
    Thresholds.Add(TThreshold(0.3, 3, "Low"));

    // Detect Alerts
    TAlertV Alerts;
    AnomalyModel.Detect(DataMat, Thresholds, Alerts);
    printf("\nNumber of alerts: %i", Alerts.Len());

    return 0;
}