// Copyright (c) 2021-2022 The Zcash developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or https://www.opensource.org/licenses/mit-license.php .

#ifndef ZCASH_RUST_INCLUDE_RUST_ORCHARD_ISSUANCE_H
#define ZCASH_RUST_INCLUDE_RUST_ORCHARD_ISSUANCE_H

#include "rust/orchard/keys.h"

#ifdef __cplusplus
extern "C" {
#endif


//
// Issuance Keys
//

struct IssuanceKeyPtr;
typedef struct IssuanceKeyPtr IssuanceKeyPtr;

struct IssuanceAuthorizingKeyPtr;
typedef struct IssuanceAuthorizingKeyPtr IssuanceAuthorizingKeyPtr;

struct IssuanceValidatingKeyPtr;
typedef struct IssuanceValidatingKeyPtr IssuanceValidatingKeyPtr;


/**
 * Free the memory allocated for the given issuance key.
 */
void issuance_key_free(IssuanceKeyPtr* ptr);

/**
 * Clones the given issuance key and returns
 * a pointer to the newly created value. Both the original
 * one's memory and the newly allocated one need to be freed
 * independently.
 */
IssuanceKeyPtr* issuance_key_clone(
        const IssuanceKeyPtr* ptr);

/**
 * Returns the issuance authorizing key for the specified issuance key.
 *
 * The resulting pointer must be ultimately freed by the caller
 * using `issuance_authorizing_key_free`.
 */
IssuanceAuthorizingKeyPtr* issuance_key_to_issuance_authorizing_key(
        const IssuanceKeyPtr* key);

/**
 * Free the memory allocated for the given issuance authorizing key.
 */
void issuance_authorizing_key_free(IssuanceAuthorizingKeyPtr* ptr);

/**
 * Clones the given issuance authorizing key and returns
 * a pointer to the newly created value. Both the original
 * one's memory and the newly allocated one need to be freed
 * independently.
 */
IssuanceAuthorizingKeyPtr* issuance_authorizing_key_clone(
        const IssuanceAuthorizingKeyPtr* ptr);

/**
 * Returns the issuance authorizing key for the specified spending key.
 *
 * The resulting pointer must be ultimately freed by the caller
 * using `issuance_authorizing_key_free`.
 */
IssuanceAuthorizingKeyPtr* issuance_key_to_issuance_authorizing_key(
        const IssuanceKeyPtr* key);

/**
 * Free the memory allocated for the given issuance validating key.
 */
void issuance_validating_key_free(IssuanceValidatingKeyPtr* ptr);

///**
// * Clones the given issuance validating key and returns
// * a pointer to the newly created value. Both the original
// * one's memory and the newly allocated one need to be freed
// * independently.
// */
//IssuanceValidatingKeyPtr* issuance_validating_key_clone(
//        const IssuanceValidatingKeyPtr* ptr);

/**
 * Returns the issuance validating key for the specified issuance authorizing key.
 *
 * The resulting pointer must be ultimately freed by the caller
 * using `orchard_issuance_validating_key_free`.
 */
IssuanceValidatingKeyPtr* issuance_authorizing_key_to_issuance_validating_key(
        const IssuanceAuthorizingKeyPtr* key);

/**
 * Constructs a spending key by ZIP-32 derivation to the account
 * level from the provided seed.
 */
IssuanceKeyPtr* issuance_key_for_account(
        const unsigned char* seed,
        size_t seed_len,
        uint32_t bip44_coin_type,
        uint32_t account_id);


struct IssueBundlePtr;
typedef struct IssueBundlePtr IssueBundlePtr;

#ifdef __cplusplus
}
#endif

#endif // ZCASH_RUST_INCLUDE_RUST_ORCHARD_ISSUANCE_H
