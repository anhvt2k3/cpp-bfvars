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
    long long fp = 0;
    size_t test_size = 0;
    double accuracy = 0;
    double test_time = 0;
    int nof_collision = 0;
    int nof_removable = 0;
    int nof_operand = 0;
    double operation_time = 0;
    double binsearch_time = 0;

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
        fp = 0;
        test_size = 0;
        accuracy = 0;
        test_time = 0;
        nof_collision = 0;
        nof_removable = 0;
        nof_operand = 0;
        operation_time = 0;
        binsearch_time = 0;
    }

    // Function to return the CSV header
    std::string getHeader() const {
        return "FilterID/TestCase/AddOrRemoveTime/KeySet/NonkeySet/F1/F2/F3/F4/F5/TestSize/Accuracy/TestTime/NofCollision/NofRemovable/NofOperand/OperationTime/BinarySearchTime\n";
        return "FilterID/TestCase/AddOrRemoveTime/KeySet/NonkeySet/F1/F2/F3/F4/fp/TestSize/Accuracy/TestTime/NofCollision/NofRemovable/NofOperand/OperationTime/BinarySearchTime\n";
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
            << fp << "/"
            << test_size << "/"
            << accuracy << "/"
            << test_time << "/"
            << nof_collision << "/"
            << nof_removable << "/"
            << nof_operand << "/"
            << std::fixed << std::setprecision(6) << operation_time << "/"
            << binsearch_time << "/"
            << "\n";
        return oss.str();
    }

    string getHeader_v1() const {
        return "filter_name, test_time, test_size, fp, accuracy";
    }

    string toCSVString_v1() const {
        ostringstream oss;
        oss << filterID << ","
            << test_time << ","
            << test_size << ","
            << fp << ","
            << std::fixed << std::setprecision(6)  // Format as float with 4 decimal places
            << static_cast<double>((test_size - fp)/test_size) << ","
            << "\n";
        return oss.str();
    }
};

struct Configuration {
    const BloomFilterModels::AbstractFilter* filter = nullptr;
    long int bs_memory;

    Configuration() {}

    Configuration(const BloomFilterModels::AbstractFilter* filter) {
        this->filter = filter;
    }

    string getHeader() const {
        return "FilterID/FilterSize/Capacity/NumHashFunctions/FalsePositiveRate/NumItemsAdded/BucketSize/HashAlgo/HashScheme/BSMemory\n";
    }

    string toCSVString() const {
        ostringstream oss;
        oss << filter->getFilterCode() << "/"
            << filter->Size() << "/" //# total number of bits used, including the bitmap for the elements and the colliding status regions managing required bitmap
            << filter->Capacity() << "/" //# max number of element the filter capable of holding
            << filter->K() << "/"
            << filter->FPrate() << "/" //# target FP rate
            << filter->Count() << "/"
            << filter->BucketSize() << "/"
            << filter->getHashAlgo() << "/"
            << filter->getHashScheme() << "/"
            << bs_memory << "\n"; //# min number of bits needed for all the keys (a.k.a max number of bits needed for all the elements in key set)
        return oss.str();
    }

    string getHeader_v1() const {
        return "filter_name, memory, occupied_rate\n";
    }

    string toCSVString_v1() const {
        std::ostringstream oss;
        oss << filter->getFilterCode() << ","
            << filter->Size() << ","
            << std::fixed << std::setprecision(6)  // Format as float with 4 decimal places
            << static_cast<double>(filter->Count()) / filter->Capacity() << ","
            << "\n";
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