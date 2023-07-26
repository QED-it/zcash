// Copyright (c) 2021-2022 The Zcash developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or https://www.opensource.org/licenses/mit-license.php .

#ifndef ZCASH_RUST_INCLUDE_RUST_ORCHARD_ASSET_H
#define ZCASH_RUST_INCLUDE_RUST_ORCHARD_ASSET_H

#include "rust/orchard/keys.h"
#include "rust/orchard/issuance.h"

#ifdef __cplusplus
extern "C" {
#endif

/// Returns an Asset ID for ZEC
bool zsa_native_asset(unsigned char* asset_ret);

/// Returns an Asset ID for a ZSA
bool zsa_derive_asset(unsigned char* ik_bytes, unsigned char* asset_desc_ptr, unsigned char* asset_ret);

/// Returns an Asset ID for a ZSA
bool zsa_derive_asset_from_isk(IssuanceAuthorizingKeyPtr* isk, unsigned char* asset_desc_ptr, unsigned char* asset_ret);

#ifdef __cplusplus
}
#endif

#endif // ZCASH_RUST_INCLUDE_RUST_ORCHARD_ASSET_H
