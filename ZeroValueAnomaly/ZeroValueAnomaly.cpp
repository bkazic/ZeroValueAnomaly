#include "base.h"
#include "misc.cpp"
using namespace AnomalyDetection;

int main()
{
	// Init zero values matrix
	TFltVV TestMat = TFltVV(3, 2);
    //TFltVVV TestMat3D = TFltVVV(2, 2, 2);

	// Tests
    Misc::MatInfo(TestMat);
    Misc::PrintMat(TestMat);
    Misc::RndMat(TestMat);
    Misc::PrintMat(TestMat);
    Misc::LineReader("test.txt");
    Misc::Timestamps();

    // Test reading input data
    PJsonVal DataJson = Misc::JsonFileReader("json.txt");
    TFltVV DataMat = Misc::JsonArr2TFltVV(DataJson);
    Misc::PrintMat(DataMat);
    TFltVVV ModelMat = Misc::Model(DataMat);
    Misc::Print3DMat(ModelMat);

    // Test reading input data
    //DataJson = Misc::JsonFileReader("json2.txt");
    //DataMat = Misc::JsonArr2TFltVV(DataJson);
    //Misc::PrintMat(DataMat);
    //ModelMat = Misc::Model(DataMat);
    //Misc::Print3DMat(ModelMat);

    //// Test reading input data
    DataJson = Misc::JsonFileReader("dummy.csv");
    DataMat = Misc::JsonArr2TFltVV(DataJson);
    //Misc::PrintMat(DataMat);
    ModelMat = Misc::Model(DataMat, false);
    Misc::Print3DMat(ModelMat);

    // Testing model clas
    Model AnomalyModel(5.); //TODO: why doesent it work without input parameter
    ModelMat = AnomalyModel.Fit(DataMat, false);

    Misc::Print3DMat(ModelMat);
    Misc::PrintMat(AnomalyModel.CountsAll);

    // Alert thresholds
    TThresholdV Thresholds;
    Thresholds.Add(TThreshold(0.1, 1, "High"));
    Thresholds.Add(TThreshold(0.2, 2, "Medium"));
    Thresholds.Add(TThreshold(0.3, 3, "Low"));

    TAlertV Alerts = AnomalyModel.Detect(DataMat, Thresholds);

    // TODO check this alerts vector if it has meaningfull alerts
    printf("\nNumber of alerts: %i", Alerts.Len());
    for (int i = 0; i < Alerts.Len(); i++) {
        printf("\nTs: %.0f, Severity: %i", (double)Alerts[i].Timestamp, Alerts[i].AlertSeverity);
    }

	return 0;
}