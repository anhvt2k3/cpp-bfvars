#pragma once
#include <fstream>
#include <vector>
#include "../csbf/Buckets.h"
#include "../csbf/BaseBF.h"


struct TestCase {
    string filterID = "NaN";
    string test_case = "NaN";
    double adding_time = 0;
    string key_set = "NaN";
    string nonkey_set = "NaN";
    double f1 = 0;
    double f2 = 0;
    double f3 = 0;
    double f4 = 0;
    double f5 = 0;
    size_t test_size = 0;
    double accuracy = 0;
    double test_time = 0;
    int nof_collision = 0;
    int nof_removable = 0;
    int nof_operand = 0;

    TestCase() {}

    void reset() {
        filterID = "NaN";
        test_case = "NaN";
        adding_time = 0;
        key_set = "NaN";
        nonkey_set = "NaN";
        f1 = 0;
        f2 = 0;
        f3 = 0;
        f4 = 0;
        f5 = 0;
        test_size = 0;
        accuracy = 0;
        test_time = 0;
        nof_collision = 0;
        nof_removable = 0;
        nof_operand = 0;
    }

    // Function to return the CSV header
    std::string getHeader() const {
        return "FilterID/TestCase/AddOrRemoveTime/KeySet/NonkeySet/F1/F2/F3/F4/F5/TestSize/Accuracy/TestTime/NofCollision/NofRemovable/NofOperand\n";
    }

    // Function to format the TestCase object as a CSV string
    std::string toCSVString() const {
        std::ostringstream oss;
        oss << filterID << "/"
            << test_case << "/"
            << adding_time << "/"
            << key_set << "/"
            << nonkey_set << "/"
            << f1 << "/"
            << f2 << "/"
            << f3 << "/"
            << f4 << "/"
            << f5 << "/"
            << test_size << "/"
            << accuracy << "/"
            << test_time << "/"
            << nof_collision << "/"
            << nof_removable << "/"
            << nof_operand
            << "\n";
        return oss.str();
    }
};

struct Configuration {
    const BloomFilterModels::AbstractFilter* filter = nullptr;

    Configuration() {}

    Configuration(const BloomFilterModels::AbstractFilter* filter) {
        this->filter = filter;
    }

    string getHeader() const {
        return "FilterID/FilterSize/Capacity/NumHashFunctions/FalsePositiveRate/NumItemsAdded/BucketSize/HashAlgo/HashScheme\n";
    }

    string toCSVString() const {
        ostringstream oss;
        oss << filter->getFilterCode() << "/"
            << filter->Size() << "/"
            << filter->Capacity() << "/"
            << filter->K() << "/"
            << filter->FPrate() << "/"
            << filter->Count() << "/"
            << filter->BucketSize() << "/"
            << filter->getHashAlgo() << "/"
            << filter->getHashScheme() << "\n";
        return oss.str();
    }

};

string BITMAP_FILE_NAME = "bitmap.csv";

void saveBitmapToCSV(const BloomFilterModels::AbstractFilter* filter) {
    std::ofstream file(BITMAP_FILE_NAME);
    if (!file.is_open()) {
        throw std::runtime_error("Could not open file for writing.");
    }
    file << "bucketindex,value\n";
    for (uint32_t i=0;i<filter->Size();i++) {
        file << i << "," << filter->buckets->Get(i); file << "\n";
    }
    file.close();
}