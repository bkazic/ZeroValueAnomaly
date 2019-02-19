#include "microtest.h"
#include "base.h"
#include "anomaly_model.h"
using namespace AnomalyDetection;

TEST(Test) {
    // Init zero values test matrix
    TFltVV TestMat = TFltVV(3, 2);

    // Testing model clas
    TModel AnomalyModel(5); //TODO: why doesent it work without input parameter

    // Test model fit
    AnomalyModel.Fit(TestMat);

    // Alert thresholds
    TThresholdV ThresholdV;
    ThresholdV.Add(TThreshold(0.2, 2, "Medium"));
    ThresholdV.Add(TThreshold(0.1, 1, "High"));
    ThresholdV.Add(TThreshold(0.3, 3, "Low"));

    // Test if thresholds can be sorted
    ThresholdV.Sort();
    for (int i = 1; i < ThresholdV.Len(); i++) {
        ASSERT_TRUE(ThresholdV[i].Value > ThresholdV[i-1].Value);
    }

    TAlertV Alerts;
    AnomalyModel.Predict(TestMat, ThresholdV, Alerts);

    ASSERT(true);
}

TEST_MAIN();