#include <cmath>
#include <cstdint>
#include <limits>
#include <algorithm>
#include <string>
#include <vector>

#include <openssl/sha.h>
#include "./smhasher-master/src/Hashes.h"

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

        static uint32_t OptimalKCounting(double fpRate)
        {
            double optimalK = -std::ceil(std::log(fpRate) / std::log(2));
            return static_cast<uint32_t>(optimalK);
        }

        static HashKernelReturnValue HashKernel(const std::vector<uint8_t>& data, const std::string& algorithm = "sha256")
        {
            if (algorithm == "sha256")
            {
                std::vector<uint8_t> hashBytes(SHA256_DIGEST_LENGTH);
                SHA256_CTX sha256;
                SHA256_Init(&sha256);
                SHA256_Update(&sha256, data.data(), data.size());
                SHA256_Final(hashBytes.data(), &sha256);

                return HashKernelFromHashBytes(hashBytes);
            }
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