#pragma once
#include <fstream>
#include <vector>
#include "../csbf/Buckets.h"
#include "../csbf/BaseBF.h"


struct TestCase {
    string test_case = "";
    double adding_time = 0;
    string key_set = "";
    string nonkey_set = "";
    double f1 = 0;
    double f2 = 0;
    double f3 = 0;
    double f4 = 0;
    double f5 = 0;
    size_t test_size = 0;
    double accuracy = 0;
    double test_time = 0;

    // Function to return the CSV header
    std::string getHeader() const {
        return "TestCase/AddOrRemoveTime/KeySet/NonkeySet/F1/F2/F3/F4/F5/TestSize/Accuracy/TestTime\n";
    }

    // Function to format the TestCase object as a CSV string
    std::string toCSVString() const {
        std::ostringstream oss;
        oss << test_case << "/"
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
            << test_time << "\n";
        return oss.str();
    }
};

struct Configuration {
    size_t filter_size = 0;
    size_t capacity = 0;
    size_t num_hash_functions = 0;
    double false_positive_rate = 0;
    size_t num_items_added = 0;
    size_t bucket_size = 0;
    size_t bucket_max_value = 0;
    size_t bucket_count = 0;

    string getHeader() const {
        return "FilterSize/Capacity/NumHashFunctions/FalsePositiveRate/NumItemsAdded/BucketSize/BucketMaxValue/BucketCount\n";
    }

    string toCSVString() const {
        ostringstream oss;
        oss << filter_size << "/"
            << capacity << "/"
            << num_hash_functions << "/"
            << false_positive_rate << "/"
            << num_items_added << "/"
            << bucket_size << "/"
            << bucket_max_value << "/"
            << bucket_count << "\n";
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