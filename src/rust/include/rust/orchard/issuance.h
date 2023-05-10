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

struct IssuanceAuthorizingKeyPtr;
typedef struct IssuanceAuthorizingKeyPtr IssuanceAuthorizingKeyPtr;

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
IssuanceAuthorizingKeyPtr* orchard_spending_key_to_issuance_authorizing_key(
        const OrchardSpendingKeyPtr* key);


struct IssuanceValidatingKeyPtr;
typedef struct IssuanceValidatingKeyPtr IssuanceValidatingKeyPtr;

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


struct IssueBundlePtr;
typedef struct IssueBundlePtr IssueBundlePtr;

/**
 * Free the memory allocated for the given issuance bundle.
 */
void issue_bundle_free(IssueBundlePtr* ptr);

/// Clones the given Issue bundle.
///
/// Both bundles need to be separately freed when they go out of scope.
IssueBundlePtr* issue_bundle_clone(IssueBundlePtr* ptr);

IssueBundlePtr* create_issue_bundle(IssuanceAuthorizingKeyPtr* isk);

void add_recipient(
        IssueBundlePtr* bundle,
        uint64_t value,
        OrchardRawAddressPtr *recipient,
        const char *asset_descr,
        bool finalize
);

IssueBundlePtr* sign_issue_bundle(
        IssueBundlePtr* bundle,
        IssuanceAuthorizingKeyPtr* isk
);


/// Parses an authorized Issue bundle from the given stream.
///
/// - If no error occurs, `bundle_ret` will point to a Rust-allocated Issue bundle.
/// - If an error occurs, `bundle_ret` will be unaltered.
bool issue_bundle_parse(
        void* stream,
        read_callback_t read_cb,
        IssueBundlePtr** bundle_ret);

/// Serializes an authorized Issue bundle to the given stream
///
/// If `bundle == nullptr`, this serializes `nActions = 0`.
bool issue_bundle_serialize(
        const IssueBundlePtr* bundle,
        void* stream,
        write_callback_t write_cb);

#ifdef __cplusplus
}
#endif

#endif // ZCASH_RUST_INCLUDE_RUST_ORCHARD_ISSUANCE_H
