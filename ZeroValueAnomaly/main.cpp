#include <base.h>
#include <mine.h>
#include "misc.h"
#include "anomaly_model.h"
#include "examples.h"

using namespace TAnomalyDetection;

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

    // Example of anomaly detection in batch
    TExamples::batchLearningExample();

    // Example of using anomaly detection record by record (stream)
    TExamples::oneByOneLearningExample();

    // Example of using resampler
    TExamples::resamplerSyntheticData();

    // Streaming example with resampler
    TExamples::streamingExample();

    return 0;
}