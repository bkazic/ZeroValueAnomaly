#include "base.h"
#include "misc.cpp"

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
    TFltVVV Model = Misc::Model(DataMat);
    Misc::Print3DMat(Model);

    // Test reading input data
    //DataJson = Misc::JsonFileReader("json2.txt");
    //DataMat = Misc::JsonArr2TFltVV(DataJson);
    //Misc::PrintMat(DataMat);
    //Model = Misc::Model(DataMat);
    //Misc::Print3DMat(Model);

    //// Test reading input data
    DataJson = Misc::JsonFileReader("dummy.csv");
    DataMat = Misc::JsonArr2TFltVV(DataJson);
    //Misc::PrintMat(DataMat);
    Model = Misc::Model(DataMat, false);
    Misc::Print3DMat(Model);

	return 0;
}