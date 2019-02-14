#include "base.h"
#include "misc.cpp"
#include "anomaly_model.h"
using namespace AnomalyDetection;

int main()
{
    // Init zero values matrix
    TFltVV TestMat = TFltVV(3, 2);

    // Tests
    Misc::MatInfo(TestMat);
    Misc::PrintMat(TestMat);
    Misc::RndMat(TestMat);
    Misc::PrintMat(TestMat);
    Misc::LineReader("test.txt");
    Misc::Timestamps();

    //// Test reading input data
    PJsonVal DataJson = Misc::JsonFileReader("dummy.csv");
    TFltVV DataMat = Misc::JsonArr2TFltVV(DataJson);

    // Testing model clas
    TModel AnomalyModel(5); //TODO: why doesent it work without input parameter
    TFltVVV ModelMat = AnomalyModel.Fit(DataMat, false);

    // Debugging
    Misc::Print3DMat(AnomalyModel.GetCounts());
    Misc::PrintMat(AnomalyModel.GetCountsAll());
    Misc::Print3DMat(ModelMat);

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