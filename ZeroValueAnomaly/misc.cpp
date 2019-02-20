#include "misc.h"

namespace AnomalyDetection {

void TMisc::MatInfo(TFltVV& Mat) {
    printf("Rows: %i\n", Mat.GetRows());
    printf("Cols: %i\n", Mat.GetCols());
}

void TMisc::RndMat(TFltVV& Mat) {
    TRnd Rnd;
    const int Rows = Mat.GetRows();
    const int Cols = Mat.GetCols();

    for (int RowN = 0; RowN < Rows; RowN++) {
        for (int ColN = 0; ColN < Cols; ColN++) {
            Mat(RowN, ColN) = Rnd.GetUniDev();
        }
    }
}

void TMisc::PrintMat(const TFltVV& Mat) {
    const int XDim = Mat.GetRows();
    const int YDim = Mat.GetCols();

    printf("\n");
    for (int X = 0; X < XDim; X++) {
        for (int Y = 0; Y < YDim; Y++) {
            printf("%.2f ", Mat(X, Y).Val);
        }
        printf("\n");
    }
}

void TMisc::Print3DMat(const TFltVVV& Mat) {
    const int XDim = Mat.GetXDim();
    const int YDim = Mat.GetYDim();
    const int ZDim = Mat.GetZDim();

    printf("\n");
    for (int X = 0; X < XDim; X++) {
        for (int Y = 0; Y < YDim; Y++) {
            printf("[");
            for (int Z = 0; Z < ZDim; Z++) {
                printf("%.2f ", Mat(X, Y, Z).Val);
            }
            printf("] ");
        }
        printf("\n");
    }
}

void TMisc::LineReader(const TStr& FName) {
    PSIn SIn = TFIn::New(FName);
    TStr CurrentLine;
    while (SIn->GetNextLn(CurrentLine)) {
        printf("\nLine: %s", CurrentLine.CStr());
    }
}

void TMisc::CsvReader(const TStr& FName) {
    PSIn SIn = TFIn::New(FName);
    TStr CurrentLine;
    while (SIn->GetNextLn(CurrentLine)) {
        printf("\nLine: %s", CurrentLine.CStr());
    }
}

void TMisc::JsonFileReaderTest(const TStr& FName) {
    PSIn SIn = TFIn::New(FName);
    PJsonVal DataJson = TJsonVal::GetValFromSIn(SIn);

    // Print entire JSON
    printf("\nData: %s", TJsonVal::GetStrFromVal(DataJson).CStr());

    // Print Row by Row
    printf("\nArrVals: %i", DataJson->GetArrVals());
    const int Rows = DataJson->GetArrVals();
    for (int RowN = 0; RowN < Rows; RowN++) {
        printf("\nRow: %s",
            TJsonVal::GetStrFromVal(DataJson->GetArrVal(RowN)).CStr());
    }
}

PJsonVal TMisc::JsonFileReader(const TStr& FName) {
    PSIn SIn = TFIn::New(FName);
    PJsonVal DataJson = TJsonVal::GetValFromSIn(SIn);

    return DataJson;
}

TFltVV TMisc::JsonArr2TFltVV(const PJsonVal& DataJson) {
    const int XDim = DataJson->GetArrVals();
    const int YDim = DataJson->GetArrVal(0)->GetArrVals(); // Should allways be 2 (ts, val)
    TFltVV Mat = TFltVV(XDim, YDim);

    // Populate Matrix
    for (int X = 0; X < XDim; X++) {
        for (int Y = 0; Y < YDim; Y++) {
            Mat(X, Y) = DataJson->GetArrVal(X)->GetArrVal(Y)->GetNum();
        }
    }

    return Mat;
}

TRecordV TMisc::JsonArr2TRecordV(const PJsonVal& DataJson) {
    const int XDim = DataJson->GetArrVals();
    TRecordV RecordV(XDim);

    // Populate vector
    for (int X = 0; X < XDim; X++) {
        const double Ts = DataJson->GetArrVal(X)->GetArrVal(0)->GetNum();
        const double Val = DataJson->GetArrVal(X)->GetArrVal(1)->GetNum();
        RecordV[X] = TRecord(uint64((int64)Ts), Val);
    }

    return RecordV;
}

// Test reading timestamps
void TMisc::Timestamps() {
    const uint64 epoch = 1536552000000;
    const TTm Tm = TTm::GetTmFromMSecs(
        TTm::GetWinMSecsFromUnixMSecs(epoch));
    printf("\nUnix: %.0f, DT: %s", (double)epoch, Tm.GetStr().CStr());
    printf("\nDay of week: %i", Tm.GetDaysSinceMonday());
    printf("\nHour of day: %i", Tm.GetHour());
}

}