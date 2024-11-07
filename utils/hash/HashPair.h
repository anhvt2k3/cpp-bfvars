#pragma once
#include <cstdint>
#include <string>
#include <vector>
#include <stdexcept>
#include <openssl/sha.h>

namespace Hash
{
    struct HashPair64
    {
        uint32_t UpperBaseHash;
        uint32_t LowerBaseHash;

        static HashPair64 Create(uint32_t lowerBaseHash, uint32_t upperBaseHash)
        {
            HashPair64 result;
            result.UpperBaseHash = upperBaseHash;
            result.LowerBaseHash = lowerBaseHash;
            return result;
        }
    };

    struct HashPair128
    {
        uint64_t UpperBaseHash;
        uint64_t LowerBaseHash;

        static HashPair128 Create(uint32_t h1, uint32_t h2, uint32_t h3, uint32_t h4)
        {
            uint64_t lowerHash = static_cast<uint64_t>(h1) | (static_cast<uint64_t>(h2) << 32);
            uint64_t upperHash = static_cast<uint64_t>(h3) | (static_cast<uint64_t>(h4) << 32);

            return HashPair128::Create(lowerHash, upperHash);
        }

        static HashPair128 Create(uint64_t lowerBaseHash, uint64_t upperBaseHash)
        {
            HashPair128 result;
            result.UpperBaseHash = upperBaseHash;
            result.LowerBaseHash = lowerBaseHash;
            return result;
        }
    };
}
