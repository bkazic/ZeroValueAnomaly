#ifndef MISC_H
#define MISC_H

#include <base.h>
#include "anomaly_model.h"

namespace TAnomalyDetection {

///////////////////////////////
// Misc object
// (static functions for demo and debugging purposes only)
class TMisc {
private:
    // Disallow creating an instance of this object
    TMisc() {}

public:
    // Get matrix info
    static void MatInfo(TFltVV& Mat);
    // Populate matrix with random values
    static void RndMat(TFltVV& Mat);
    // Printing 2d matrix
    static void PrintMat(const TFltVV& Mat);
    // Printing 3d matrix
    static void Print3DMat(const TFltVVV& Mat);
    // Reading line by line
    static void LineReader(const TStr& FName);
    // Read CSV (line by line)
    static void CsvReader(const TStr& FName);
    // Reading JSON file
    static void JsonFileReaderTest(const TStr& FName);
    // Reading JSON file
    static PJsonVal JsonFileReader(const TStr& FName);
    // Convert from PJson Array to Matrix (TFltVV)
    static TFltVV JsonArr2TFltVV(const PJsonVal& DataJson);
    // Convert from PJson Array to Vector of Records
    static TRecordV JsonArr2TRecordV(const PJsonVal& DataJson);
    // Test reading timestamps
    static void Timestamps();
};

} // namespace TAnomalyDetection

#endif