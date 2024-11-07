#pragma once
#include <cstdint>
#include <string>
#include <vector>
#include <stdexcept>
#include <openssl/sha.h>

#include "./rurban-smhasher/Hashes.h"
#include "./HashPair.h"

using namespace Hash;

namespace Hash128
{
    using keyType = const std::vector<uint8_t>;
    uint64_t HashBytesToUInt64(keyType &hashBytes, int offset = 0)
    {
        return static_cast<uint64_t>(hashBytes[offset]) |
               static_cast<uint64_t>(hashBytes[offset + 1]) << 8 |
               static_cast<uint64_t>(hashBytes[offset + 2]) << 16 |
               static_cast<uint64_t>(hashBytes[offset + 3]) << 24 |
               static_cast<uint64_t>(hashBytes[offset + 4]) << 32 |
               static_cast<uint64_t>(hashBytes[offset + 5]) << 40 |
               static_cast<uint64_t>(hashBytes[offset + 6]) << 48 |
               static_cast<uint64_t>(hashBytes[offset + 7]) << 56;
    }
    HashPair128 HashKernelFromHashBytes(keyType &hashBytes)
    {
        return HashPair128::Create(
            Hash128::HashBytesToUInt64(hashBytes, 0),
            Hash128::HashBytesToUInt64(hashBytes, 8));
    }
    HashPair128 HashMurmur3_x86_128(keyType &data)
    {
        uint32_t hash[4];
        MurmurHash3_x86_128(data.data(), data.size(), 0, &hash);
        return HashPair128::Create(hash[0], hash[1], hash[2], hash[3]);
    }
    HashPair128 HashMurmur3_x64_128(keyType &data)
    {
        uint64_t hash[2];
        MurmurHash3_x64_128(data.data(), data.size(), 0, &hash);
        return HashPair128::Create(hash[0], hash[1]);
    }
    HashPair128 HashSHA256_128(keyType &data)
    {
        std::vector<uint8_t> hashBytes(SHA256_DIGEST_LENGTH);
        SHA256(data.data(), data.size(), hashBytes.data());

        return Hash128::HashKernelFromHashBytes(hashBytes);
    }
    HashPair128 HashXXH3_128(keyType &data)
    {
        XXH128_hash_t out = XXH3_128bits_withSeed(data.data(), (size_t)data.size(), 0);
        return HashPair128::Create(
            out.low64,
            out.high64);
    }
    HashPair128 HashKernel(keyType &data, const std::string &algorithm = "sha256")
    {
        if (algorithm == "murmur_x86")
        {
            return HashMurmur3_x86_128(data);
        }
        else if (algorithm == "murmur_x64")
        {
            return HashMurmur3_x64_128(data);
        }
        else if (algorithm == "sha256")
        {
            return HashSHA256_128(data);
        }
        else if (algorithm == "xxh3")
        {
            return HashXXH3_128(data);
        }
        else
        {
            throw std::invalid_argument("Invalid algorithm");
        }
    }
}
