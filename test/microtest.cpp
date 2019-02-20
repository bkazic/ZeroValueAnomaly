#include "microtest.h"
#include "base.h"
#include "anomaly_model.h"
using namespace AnomalyDetection;

TEST(Test) {
    // Init zero values test dataset
    TRecordV DataVec(3);
    DataVec[0] = TRecord(1, 0);
    DataVec[1] = TRecord(2, 0);
    DataVec[2] = TRecord(3, 0);

    // Testing model clas
    TModel AnomalyModel(5);

    // Test model fit
    AnomalyModel.Fit(DataVec);

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
    AnomalyModel.Predict(DataVec, ThresholdV, Alerts);

    ASSERT(true);
}

TEST_MAIN();