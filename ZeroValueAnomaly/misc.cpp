#include "base.h"

class Misc {
private:
    // Disallow creating an instance of this object
    Misc() {}

public:
    // Get matrix info
    static void MatInfo(TFltVV& Mat) {
        printf("Rows: %i\n", Mat.GetRows());
        printf("Cols: %i\n", Mat.GetCols());
    }

    // Populate matrix with random values
    static void RndMat(TFltVV& Mat) {
        TRnd Rnd;
        const int Rows = Mat.GetRows();
        const int Cols = Mat.GetCols();

        for (int RowN = 0; RowN < Rows; RowN++) {
            for (int ColN = 0; ColN < Cols; ColN++) {
                //Mat.PutXY(RowN, ColN, Rnd.GetUniDev());
                Mat(RowN, ColN) = Rnd.GetUniDev();
            }
        }
    }

    // Printing 2d matrix
    static void PrintMat(const TFltVV& Mat) {
	    const int XDim = Mat.GetRows();
	    const int YDim = Mat.GetCols();

        printf("\n");
	    for (int X = 0; X < XDim; X++) {
		    for (int Y = 0; Y < YDim; Y++) {
			    //printf("%f ", Mat.GetXY(X, Y).Val);
                printf("%.2f ", Mat(X, Y).Val);
		    }
		    printf("\n");
	    }
    }

    // Printing 3d matrix
    static void Print3DMat(const TFltVVV& Mat) {
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

    // Reading line by line
    static void LineReader(const TStr& FName) {
        PSIn SIn = TFIn::New(FName);
        TStr CurrentLine;
        while (SIn->GetNextLn(CurrentLine)) {
            printf("\nLine: %s", CurrentLine.CStr());
        }
    }

    // Read CSV (line by line)
    static void CsvReader(const TStr& FName) {
        PSIn SIn = TFIn::New(FName);
        TStr CurrentLine;
        while (SIn->GetNextLn(CurrentLine)) {
            printf("\nLine: %s", CurrentLine.CStr());
        }
    }

    // Reading JSON file
    static PJsonVal JsonFileReaderTest(const TStr& FName) {
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

    // Reading JSON file
    static PJsonVal JsonFileReader(const TStr& FName) {
        PSIn SIn = TFIn::New(FName);
        PJsonVal DataJson = TJsonVal::GetValFromSIn(SIn);

        return DataJson;
    }

    // Convert from PJson Array to Matrix (TFltVV)
    static TFltVV JsonArr2TFltVV(const PJsonVal& DataJson) {
        // Create matrix
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

    // Test reading timestamps
    static void Timestamps() {
        const uint64 epoch = 1536552000000;
        const TTm Tm = TTm::GetTmFromMSecs(
            TTm::GetWinMSecsFromUnixMSecs(epoch));
        printf("\nUnix: %.0f, DT: %s", (double)epoch, Tm.GetStr().CStr());
        printf("\nDay of week: %i", Tm.GetDaysSinceMonday());
        printf("\nHour of day: %i", Tm.GetHour());
    }

};