﻿#include "misc.h"
#include <base.h>
#include <mine.h>
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
    TMisc::LineReader("../ZeroValueAnomaly/data/test.txt");
    TMisc::Timestamps();

    //// Test reading input data
    PJsonVal DataJson = TMisc::JsonFileReader("../ZeroValueAnomaly/data/dummy.json");
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

    ///////////////////////////
    //// Testing Resampler ////
    ///////////////////////////

    //TODO: Create resampler paramVal
    PJsonVal ParamVal = TJsonVal::NewObj();
    ParamVal->AddToObj("interval", 60*60*1000);
    ParamVal->AddToObj("aggType", "sum");
    ParamVal->AddToObj("roundStart", "h");
    ParamVal->AddToObj("defaultValue", 0);

    TSignalProc::TAggrResampler Resampler(ParamVal);

    // Resampler output values
    double ResampledValue = 0;
    uint64 ResampledTm = 0;
    bool FoundEmptyP = false;
    bool SkipEmptyP = false;

    // Resampler input values
    double NewVal = 0.;
    uint64 NewTmMSecs = 1 * 60 * 60 * 1000;

    // Set current time
    Resampler.SetCurrentTm(NewTmMSecs);
    // Add record to the buffer
    Resampler.AddPoint(NewVal, NewTmMSecs);
    Resampler.PrintState();

    // Test sending new record
    NewVal = 1.;
    NewTmMSecs = 2 * 60 * 60 * 1000;
    Resampler.SetCurrentTm(NewTmMSecs);
    Resampler.AddPoint(NewVal, NewTmMSecs);

    //Resampler.TryResampleOnce(ResampledValue, ResampledTm, FoundEmptyP);
    printf("\nResampled Val: %f, ResampledTm: %.0f, EmptyBuffer: %d, Test: %d",
        ResampledValue, (double)ResampledTm, FoundEmptyP, Resampler.TryResampleOnce(ResampledValue, ResampledTm, FoundEmptyP));
    Resampler.PrintState();


    // Test sending new record
    NewVal = 2.;
    NewTmMSecs = 3 * 60 * 60 * 1000;
    Resampler.SetCurrentTm(NewTmMSecs);
    Resampler.AddPoint(NewVal, NewTmMSecs);

    //Resampler.TryResampleOnce(ResampledValue, ResampledTm, FoundEmptyP);
    printf("\nResampled Val: %f, ResampledTm: %.0f, EmptyBuffer: %d, Test: %d",
        ResampledValue, (double)ResampledTm, FoundEmptyP, Resampler.TryResampleOnce(ResampledValue, ResampledTm, FoundEmptyP));
    Resampler.PrintState();


    // Test sending new record
    NewVal = 3.;
    NewTmMSecs = 7 * 60 * 60 * 1000;
    Resampler.SetCurrentTm(NewTmMSecs);
    Resampler.AddPoint(NewVal, NewTmMSecs);

    //Resampler.TryResampleOnce(ResampledValue, ResampledTm, FoundEmptyP);
    printf("\nResampled Val: %f, ResampledTm: %.0f, EmptyBuffer: %d, Test: %d",
        ResampledValue, (double)ResampledTm, FoundEmptyP, Resampler.TryResampleOnce(ResampledValue, ResampledTm, FoundEmptyP));
    Resampler.PrintState();

    Resampler.SetCurrentTm(8 * 60 * 60 * 1000);

    // Call resampler until we get to the current timestamp
    while (Resampler.TryResampleOnce(ResampledValue, ResampledTm, FoundEmptyP)) {
        Resampler.PrintState();
        // notify out aggregate that new resampled values are available
        if (FoundEmptyP && SkipEmptyP) { continue; }
    }

    // Subsampling test
    printf("\n\nSubsampling test:\n");

    // Test sending new record
    NewVal = 2.;
    NewTmMSecs = 9 * 60 * 60 * 1000;
    Resampler.SetCurrentTm(NewTmMSecs);
    Resampler.AddPoint(NewVal, NewTmMSecs);

    // Test sending new record
    NewVal = 2.;
    NewTmMSecs = 9.1 * 60 * 60 * 1000;
    Resampler.SetCurrentTm(NewTmMSecs);
    Resampler.AddPoint(NewVal, NewTmMSecs);

    // Test sending new record
    NewVal = 2.;
    NewTmMSecs = 9.2 * 60 * 60 * 1000;
    Resampler.SetCurrentTm(NewTmMSecs);
    Resampler.AddPoint(NewVal, NewTmMSecs);

    // Test sending new record
    NewVal = 2.;
    NewTmMSecs = 9.3 * 60 * 60 * 1000;
    Resampler.SetCurrentTm(NewTmMSecs);
    Resampler.AddPoint(NewVal, NewTmMSecs);

    // Test sending new record
    NewVal = 2.;
    NewTmMSecs = 10 * 60 * 60 * 1000;
    Resampler.SetCurrentTm(NewTmMSecs);
    Resampler.AddPoint(NewVal, NewTmMSecs);


    // Call resampler until we get to the current timestamp
    while (Resampler.TryResampleOnce(ResampledValue, ResampledTm, FoundEmptyP)) {
        Resampler.PrintState();
        // notify out aggregate that new resampled values are available
        if (FoundEmptyP && SkipEmptyP) { continue; }
    }


    // Testing in for loop
    printf("\n\nTesting in loop:\n");

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


    // Testing entire dataset
    printf("\n\nTesting dummy dataset:\n");

    DataJson = TMisc::JsonFileReader("../ZeroValueAnomaly/data/dummy_10min.json");
    DataVec = TMisc::JsonArr2TRecordV(DataJson);
    const int Rows = DataVec.Len();

    // Clear old instance of Anomaly Model
    AnomalyModel.Clear();

    // new instance of resampler
    TSignalProc::TAggrResampler ResamplerNew(ParamVal);

    // Read dataset record by record
    for (int RowN = 0; RowN < Rows; RowN++) {
        const double Val = DataVec[RowN].GetValue();
        const uint64 Ts = DataVec[RowN].GetTimestamp();

        // Push data into resampler
        ResamplerNew.SetCurrentTm(Ts);
        ResamplerNew.AddPoint(Val, Ts);

        // Call resampler until we get to the current timestamp
        while (ResamplerNew.TryResampleOnce(ResampledValue, ResampledTm, FoundEmptyP)) {

            // notify out aggregate that new resampled values are available
            if (FoundEmptyP && SkipEmptyP) { continue; }

            printf("\nResampled Val: %f, ResampledTm: %.0f",
                ResampledValue, (double)ResampledTm);
            //Resampler.PrintState();

            // Fit model
            AnomalyModel.Fit(TRecord(ResampledTm, ResampledValue));

        }
    }

    return 0;
}