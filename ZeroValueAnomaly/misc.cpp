#include "base.h"

class Misc {
private:
    // Disallow creating an instance of this object
    Misc() {}

public:
    // Get matrix info
    static void MatInfo(TFltVV& Mat) {
        printf("Rows: %d\n", Mat.GetRows());
        printf("Cols: %d\n", Mat.GetCols());
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
            printf("\nRow: %s", TJsonVal::GetStrFromVal(DataJson->GetArrVal(RowN)).CStr());

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
        TTm Tm = TTm::GetTmFromMSecs(TTm::GetWinMSecsFromUnixMSecs(epoch));
        printf("\nUnix: %.0f, DateTime: %s", (double)epoch, Tm.GetStr());
        printf("\nDay of week: %i", Tm.GetDaysSinceMonday());
        printf("\nHour of day: %i", Tm.GetHour());
    }

    // Protoype of the model (from PJsonVal)
    static void Model(const PJsonVal& Data) {
        const int Days = 6;
        const int Hours = 23;
        const int Lags = 5;

        // Initialize the model (3d matrix)
        TFltVVV Mat = TFltVVV(Days, Hours, Lags);

        // For: iterate over data
        const int Rows = Data->GetArrVals();
        for (int RowN = 0; RowN < Rows; RowN++) {

            // TODO: maybe cast data from PJsonVal to TFltVV (matrix)

            // For: Iterate over logs

        }
    }

    // Protoype of the model (from TFltVV)
    static TFltVVV Model(const TFltVV& Data, const bool& Verbose=true) {
        // model parameters
        int Lags = 5;
        double ObservedValue = 0.;

        const int TimestampIdx = 0; // First one is timestamp
        const int ValueIdx = 1; // Second is value

        const int Days = 7;
        const int Hours = 24;

        // Initialize the model (3d matrix)
        TFltVVV Res = TFltVVV(Hours, Days, Lags);

        // Iterate over entire dataset
        const int Rows = Data.GetXDim();
        for (int RowN = 0; RowN < Rows; RowN++) {

            const uint64 epoch = Data(RowN, TimestampIdx);
            TTm Tm = TTm::GetTmFromMSecs(TTm::GetWinMSecsFromUnixMSecs(epoch));
            const int Day = Tm.GetDaysSinceMonday();
            const int Hour = Tm.GetHour();

            // TODO: Debugging, delete this later
            if (Verbose) {
                printf("\n%i [%s]: (%.0f, %.2f)", RowN, Tm.GetStr(), 
                    (double)Data(RowN, TimestampIdx), Data(RowN, ValueIdx));
            }

            // TODO: Construct the normalization matrix

            // Iterate over lags
            for (int LagN = 0; LagN < Lags; LagN++) {

                // check if valid data index
                if (RowN - LagN < 0) break;

                // If observed value (usually zero)
                if (Data(RowN - LagN, ValueIdx) == ObservedValue) {

                    // TODO: Debugging, delete this later
                    if (Verbose) {
                        printf(" ZERO VALUE ==> [Day: %i, Hour: %i, Lag: %i] = %.0f", Day, Hour, LagN, Res(Day, Hour, LagN));
                    }

                    // Increase count
                    Res(Hour, Day, LagN)++;
                }
                else {
                    break;
                }

            }

        }

        // TODO: Normalize

        return Res;
    }

};

class Model {
public:
    // model parameters
    int Lags = 5; // TODO: should be able to change only through constructor
    double ObservedValue = 0.;

    const int TimestampIdx = 0; // First one is timestamp
    const int ValueIdx = 1; // Second is value

    const int Days = 7;
    const int Hours = 24;

    TFltVVV Counts;
    TFltVV CountsAll;
    TVec<TFltVV> CountsTest; // TODO: Delete! Not in use
    TFltVVV Probs;

    // Constructors
    Model() { 
        Init();
    };

    Model(const int& LagsNum) {
        Lags = LagsNum;
        Init();
    };

    // Methods
    void Init() {
        Counts = TFltVVV(Hours, Days, Lags);
        CountsAll = TFltVV(Hours, Days);
        Probs = TFltVVV(Hours, Days, Lags);

        // TODO: Delete this later
        CountsTest = TVec<TFltVV>(Lags);
        for (int LagN = 0; LagN < Lags; LagN++) {
            CountsTest[LagN] = TFltVV(Hours, Days);
        }
    }

    TFltVVV Fit(const TFltVV& Data, const bool& Verbose = true) {
        const int Rows = Data.GetXDim();
        for (int RowN = 0; RowN < Rows; RowN++) {

            const uint64 epoch = Data(RowN, TimestampIdx);
            TTm Tm = TTm::GetTmFromMSecs(TTm::GetWinMSecsFromUnixMSecs(epoch));
            const int Day = Tm.GetDaysSinceMonday();
            const int Hour = Tm.GetHour();

            // TODO: Debugging, delete this later
            if (Verbose) {
                printf("\n%i [%s]: (%.0f, %.2f)", RowN, Tm.GetStr(),
                    (double)Data(RowN, TimestampIdx), Data(RowN, ValueIdx));
            }

            // Update ormalization matrix
            CountsAll(Hour, Day)++;

            // Iterate over lags
            for (int LagN = 0; LagN < Lags; LagN++) {

                // check if valid data index (positive)
                if (RowN - LagN < 0) break;

                // If observed value (usually zero)
                if (Data(RowN - LagN, ValueIdx) == ObservedValue) {

                    // TODO: Debugging, delete this later
                    if (Verbose) {
                        printf(" ZERO VALUE ==> [Day: %i, Hour: %i, Lag: %i] = %.0f", Day, Hour, LagN, Counts3D(Day, Hour, LagN));
                    }

                    // Increase count
                    Counts(Hour, Day, LagN)++;
                }
                else {
                    break;
                }
            }
        }

        // Normalize
        Normalize(Counts, CountsAll, Probs);

        return Probs;
    }

    void Normalize(const TFltVVV& Mat, const TFltVV& Norm, TFltVVV& Res) {
        // You could test if matrices are of equal shape (CountsAll and COunts)
        const int XDim = Mat.GetXDim();
        const int YDim = Mat.GetYDim();
        const int ZDim = Mat.GetZDim();

        // Update normalized matrix
        for (int X = 0; X < XDim; X++) {
            for (int Y = 0; Y < YDim; Y++) {
                for (int Z = 0; Z < ZDim; Z++) {
                    Res(X, Y, Z) = Mat(X, Y, Z).Val / Norm(X, Y).Val;
                }
            }
        }
    }

    void Predict() {
        // TODO
    }

    void Save() {
        // TODO
    }

    void Load() {
        // TODO
    }

};