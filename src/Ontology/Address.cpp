// Copyright © 2017-2019 Trust Wallet.
//
// This file is part of Trust. The full Trust copyright notice, including
// terms governing use, modification, and redistribution, is contained in the
// file LICENSE at the root of the source code distribution tree.

#include "../Hash.h"
#include "Address.h"

#include <string>
#include <stdexcept>

#include <TrezorCrypto/base58.h>

using namespace TW;
using namespace TW::Ontology;

Address::Address(const PublicKey &publicKey) {
    std::vector<uint8_t> builder(publicKey.bytes);
    uint8_t pushBytes33 = 0x21;
    builder.insert(builder.begin(), pushBytes33);
    uint8_t checkSig = 0xAC;
    builder.push_back(checkSig);
    auto builderData = toScriptHash(builder);
    std::copy(builderData.begin(), builderData.end(), data.begin());
}

Address::Address(const std::string &b58Address) {
    if (!Address::isValid(b58Address)) {
        throw std::runtime_error("Invalid base58 encode address.");
    }
    Data addressWithVersion(size + 1);
    base58_decode_check(b58Address.c_str(), HASHER_SHA2D, addressWithVersion.data(), size + 1);
    std::copy(addressWithVersion.begin() + 1, addressWithVersion.end(), data.begin());
}

Address::Address(const std::vector<uint8_t> &bytes) {
    if (bytes.size() != size) {
        throw std::runtime_error("Invalid bytes data.");
    }
    std::copy(bytes.begin(), bytes.end(), data.begin());
}


std::vector<uint8_t> Address::toScriptHash(std::vector<uint8_t> &data) {
    return Hash::ripemd(Hash::sha256(data));
}

bool Address::isValid(const std::string &b58Address) noexcept {
    if (b58Address.length() != 34) {
        return false;
    }
    Data addressWithVersion(size + 1);
    auto len = base58_decode_check(b58Address.c_str(), HASHER_SHA2D, addressWithVersion.data(), size + 1);
    return len == size + 1;
}

std::string Address::string() const {
    std::vector<uint8_t> encodeData(size + 1);
    encodeData[0] = version;
    std::copy(data.begin(), data.end(), encodeData.begin() + 1);
    size_t b58StrSize = 34;
    std::string b58Str(b58StrSize, ' ');
    base58_encode_check(encodeData.data(), (int) encodeData.size(), HASHER_SHA2D, &b58Str[0], (int) b58StrSize + 1);
    return b58Str;
}
