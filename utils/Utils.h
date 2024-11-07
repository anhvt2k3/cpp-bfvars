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
#include <openssl/sha.h>

#include "../csbf/Buckets.h"
#include "./hash/Hash64.h"
#include "./hash/Hash128.h"

namespace BloomFilterApp
{
    // Utils
    class Utils
    {
    public:
        static uint32_t OptimalMCounting(uint32_t n, double fpRate)
        {
            double optimalM = -std::ceil(static_cast<double>(n) * std::log(fpRate) / std::pow(std::log(2), 2));
            return static_cast<uint32_t>(optimalM);
        }

        static uint32_t OptimalKCounting(double fpRate)
        {
            double optimalK = -std::ceil(std::log(fpRate) / std::log(2));
            return static_cast<uint32_t>(optimalK);
        }

        static Hash::HashPair64 HashKernel64(
            const std::vector<uint8_t> &data,
            const std::string &algorithm = "sha256")
        {
            return Hash64::HashKernel(data, algorithm);
        }

        static Hash::HashPair128 HashKernel128(
            const std::vector<uint8_t> &data,
            const std::string &algorithm = "sha256")
        {
            return Hash128::HashKernel(data, algorithm);
        }

        template <typename HashPairType>
        static auto GenerateHashKernel(const std::vector<uint8_t>& data, string algorithm)
        {
            if constexpr (std::is_same<HashPairType, Hash::HashPair64>::value) {
                return HashKernel64(data, algorithm);
            }
            else if constexpr (std::is_same<HashPairType, Hash::HashPair128>::value) {
                return HashKernel128(data, algorithm);
            }
        }
    };
}