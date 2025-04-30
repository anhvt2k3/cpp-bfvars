#pragma once
#include <cmath>
#include <cstdint>
#include <limits>
#include <algorithm>
#include <string>
#include <vector>
#include <list>
#include <ctime>
#include <memory>

#include "../csbf/Buckets.h"
// #include <openssl/sha.h>
#include "./hasher/hash-suite/MurmurHash1.cpp"
#include "./hasher/hash-suite/MurmurHash2.cpp"
#include "./hasher/hash-suite/MurmurHash3.cpp"

namespace BloomFilterApp
{
    // HashKernelReturnValue
    struct HashKernelReturnValue
    {
        uint32_t UpperBaseHash;
        uint32_t LowerBaseHash;

        static HashKernelReturnValue Create(uint32_t lowerBaseHash, uint32_t upperBaseHash)
        {
            HashKernelReturnValue result;
            result.UpperBaseHash = upperBaseHash;
            result.LowerBaseHash = lowerBaseHash;
            return result;
        }
    };

    // Utils
    class Utils
    {
    public:
        static uint32_t OptimalMCounting(uint32_t n, double fpRate)
        {
            double optimalM = -std::ceil(static_cast<double>(n) * std::log(fpRate) / std::pow(std::log(2), 2));
            return static_cast<uint32_t>(optimalM);
        }
        
        static uint32_t OptimalM_perKCounting(uint32_t n, double fpRate, uint32_t k)
        {
            double optimalM = -std::ceil(static_cast<double>(n) * std::log(fpRate) / k * std::pow(std::log(2), 2));
            return static_cast<uint32_t>(optimalM);
        }

        static uint32_t OptimalK_Counting(
            long long m, // total number of bits (buckets)
            long long n // max number of elements (n,maxCApacity,Capacity())
        )
        {
            double optimalK = std::ceil(m * std::log(2) / n);
            return static_cast<uint32_t>(optimalK);
        }

        static uint32_t OptimalKCounting(double fpRate)
        {
            double optimalK = -std::ceil(std::log(fpRate) / std::log(2));
            return static_cast<uint32_t>(optimalK);
        }

        // : Optimal K calculation with respect to m and n

        static HashKernelReturnValue HashKernel(const std::vector<uint8_t>& data, const std::string& algorithm = "sha256")
        {
            return HashKernelReturnValue{};
            // if (algorithm == "murmur")
            // {
            //     uint32_t hash[2];
            //     // @Combo C (best)
            //     hash[1] = MurmurHash1(data.data(), data.size(), 0);
            //     MurmurHash3_x86_32(data.data(), data.size(), 0, &hash[0]);
            //     // @Combo B
            //     // hash[0] = MurmurHash2(data.data(), data.size(), 0);
            //     // MurmurHash3_x86_32(data.data(), data.size(), 0, &hash[1]);
            //     // @Combo A
            //     // hash[1] = MurmurHash1(data.data(), data.size(), 0);
            //     // hash[0] = MurmurHash2(data.data(), data.size(), 0);
            //     return HashKernelReturnValue::Create(hash[0], hash[1]);
            // }
            // else if (algorithm == "single")
            // {
            //     uint32_t hash[2];
            //     MurmurHash3_x86_32(data.data(), data.size(), 0, &hash[0]);
            //     return HashKernelReturnValue::Create(hash[0], 0);
            // }
            // else if (algorithm == "sha256")
            // {
            //     std::vector<uint8_t> hashBytes(SHA256_DIGEST_LENGTH);
            //     SHA256_CTX sha256;
            //     SHA256_Init(&sha256);
            //     SHA256_Update(&sha256, data.data(), data.size());
            //     SHA256_Final(hashBytes.data(), &sha256);

            //     return HashKernelFromHashBytes(hashBytes);
            // }
            // else
            // {
            //     throw std::invalid_argument("Invalid algorithm");
            // }
        }

        static HashKernelReturnValue HashKernelFromHashBytes(const std::vector<uint8_t>& hashBytes)
        {
            return HashKernelReturnValue::Create(
                HashBytesToUInt32(hashBytes, 0),
                HashBytesToUInt32(hashBytes, 4)
            );
        }

        static uint32_t HashBytesToUInt32(const std::vector<uint8_t>& hashBytes, int offset = 0)
        {
            return
                hashBytes[offset] |
                static_cast<uint32_t>(hashBytes[offset + 1]) << 8 |
                static_cast<uint32_t>(hashBytes[offset + 2]) << 16 |
                static_cast<uint32_t>(hashBytes[offset + 3]) << 24;
        }
    };
}