#pragma once
#include <cstdint>
#include <string>
#include <vector>
#include <stdexcept>
#include <openssl/sha.h>

#include "./rurban-smhasher/Hashes.h"
#include "./HashPair.h"

using namespace Hash;

namespace Hash64
{
    using keyType = const std::vector<uint8_t>;
    uint32_t HashBytesToUInt32(keyType &hashBytes, int offset = 0)
    {
        return hashBytes[offset] |
               static_cast<uint32_t>(hashBytes[offset + 1]) << 8 |
               static_cast<uint32_t>(hashBytes[offset + 2]) << 16 |
               static_cast<uint32_t>(hashBytes[offset + 3]) << 24;
    }
    HashPair64 HashKernelFromHashBytes(keyType &hashBytes)
    {
        return HashPair64::Create(
            Hash64::HashBytesToUInt32(hashBytes, 0),
            Hash64::HashBytesToUInt32(hashBytes, 4));
    }
    HashPair64 HashMurmur3_x86_64(keyType &data)
    {
        uint32_t hash[2];
        MurmurHash3_x86_32(data.data(), data.size(), 0, &hash[0]);
        MurmurHash3_x86_32(data.data(), data.size(), data.size(), &hash[1]);
        return HashPair64::Create(hash[0], hash[1]);
    }
    HashPair64 HashMurmur3_x86_32(keyType &data)
    {
        uint32_t hash[2];
        MurmurHash3_x86_32(data.data(), data.size(), 0, &hash[0]);
        return HashPair64::Create(hash[0], 0);
    }
    HashPair64 HashSHA256_64(keyType &data)
    {
        std::vector<uint8_t> hashBytes(SHA256_DIGEST_LENGTH);
        SHA256(data.data(), data.size(), hashBytes.data());

        return Hash64::HashKernelFromHashBytes(hashBytes);
    }
    HashPair64 HashXXH3_64(keyType &data)
    {
        uint64_t out = (uint64_t)XXH3_64bits_withSeed(data.data(), (size_t)data.size(), 0);
        return HashPair64::Create(
            static_cast<uint32_t>(out & 0xFFFFFFFF), // lower
            static_cast<uint32_t>(out >> 32)         // upper
        );
    }
    // HashPair64 HashFNV1A_Pippip_Yurii_128(keyType &data)
    // {
    //     uint32_t out = FNV1A_Pippip_Yurii(
    //         reinterpret_cast<const char *>(data.data()),
    //         data.size(),
    //         0);

    //     return HashPair64::Create(out, 0);
    // }
    HashPair64 HashKernel(keyType &data, const std::string &algorithm = "sha256")
    {
        if (algorithm == "murmur_x86")
        {
            return HashMurmur3_x86_64(data);
        }
        else if (algorithm == "single")
        {
            return HashMurmur3_x86_32(data);
        }
        else if (algorithm == "sha256")
        {
            return HashSHA256_64(data);
        }
        else if (algorithm == "xxh3")
        {
            return HashXXH3_64(data);
        }
        // else if (algorithm == "fnv1a_pipip_yurii")
        // {
        //     return HashFNV1A_Pippip_Yurii_128(data);
        // }
        else
        {
            throw std::invalid_argument("Invalid algorithm");
        }
    }
}
