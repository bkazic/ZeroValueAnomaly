#ifndef EXAMPLES_H
#define EXAMPLES_H

#include <base.h>
#include <mine.h>
#include "misc.h"
#include "anomaly_model.h"

///////////////////////////////
// Examples object
// (static functions for demo and debugging purposes only)
class TExamples {
private:
    // Disallow creating an instance of this object
    TExamples() {}

public:
    // Example of anomaly detection in batch
    static void batchLearningExample();
    // Example of using anomaly detection record by record (stream)
    static void oneByOneLearningExample();
    // Example of using resampler
    static void resamplerSyntheticData();
    // Streaming example with resampler
    static void streamingExample();
};

#endif