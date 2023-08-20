// Copyright (c) 2021-2022 The Zcash developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or https://www.opensource.org/licenses/mit-license.php .

#ifndef ZCASH_PRIMITIVES_ISSUE_H
#define ZCASH_PRIMITIVES_ISSUE_H

#include "streams.h"

#include <amount.h>
#include <rust/bridge.h>
#include "rust/orchard/issuance.h"
#include <rust/orchard/wallet.h>
#include "zcash/address/orchard.hpp"

    class IssuanceAuthorizingKey {
    private:
        std::unique_ptr<IssuanceAuthorizingKeyPtr, decltype(&issuance_authorizing_key_free)> inner;

        IssuanceAuthorizingKey() : inner(nullptr, issuance_authorizing_key_free) {}

        friend class OrchardSpendingKey;
        friend class OrchardWallet;
        friend class IssueBundle;
        friend class Asset;
    public:
        IssuanceAuthorizingKey(IssuanceAuthorizingKey &&key) : inner(std::move(key.inner)) {}

        IssuanceAuthorizingKey(IssuanceAuthorizingKeyPtr *ptr) :
                inner(ptr, issuance_authorizing_key_free) {}

        IssuanceAuthorizingKey(const IssuanceAuthorizingKey &key) :
                inner(issuance_authorizing_key_clone(key.inner.get()), issuance_authorizing_key_free) {}

        IssuanceAuthorizingKey &operator=(IssuanceAuthorizingKey &&key) {
            if (this != &key) {
                inner = std::move(key.inner);
            }
            return *this;
        }

        IssuanceAuthorizingKey &operator=(const IssuanceAuthorizingKey &key) {
            if (this != &key) {
                inner.reset(issuance_authorizing_key_clone(key.inner.get()));
            }
            return *this;
        }
    };

    class IssuanceKey {
    private:
        std::unique_ptr<IssuanceKeyPtr, decltype(&issuance_key_free)> inner;

        IssuanceKey() : inner(nullptr, issuance_key_free) {}

        friend class OrchardSpendingKey;
        friend class OrchardWallet;
        friend class IssueBundle;
        friend class Asset;
    public:
        IssuanceKey(IssuanceKey &&key) : inner(std::move(key.inner)) {}

        IssuanceKey(IssuanceKeyPtr *ptr) :
                inner(ptr, issuance_key_free) {}

        IssuanceKey(const IssuanceKey &key) :
                inner(issuance_key_clone(key.inner.get()), issuance_key_free) {}

        IssuanceKey &operator=(IssuanceKey &&key) {
            if (this != &key) {
                inner = std::move(key.inner);
            }
            return *this;
        }

        IssuanceKey &operator=(const IssuanceKey &key) {
            if (this != &key) {
                inner.reset(issuance_key_clone(key.inner.get()));
            }
            return *this;
        }

        IssuanceAuthorizingKey ToIssuanceAuthorizingKey() const {
            return IssuanceAuthorizingKey(issuance_key_to_issuance_authorizing_key(inner.get()));
        }

        static IssuanceKey ForAccount(
                const HDSeed& seed,
                uint32_t bip44CoinType,
                libzcash::AccountId accountId) {

            auto ptr = issuance_key_for_account(
                    seed.RawSeed().data(),
                    seed.RawSeed().size(),
                    bip44CoinType,
                    accountId);

            if (ptr == nullptr) {
                throw std::ios_base::failure("Unable to generate Issuance key.");
            }

            return IssuanceKey(ptr);
        }
    };


/**
 * The Issue component of an authorized transaction.
 */
    class IssueBundle {
    private:
        /// An optional Issue bundle (with `nullptr` corresponding to `None`).
        /// Memory is allocated by Rust.
        rust::Box<issue_bundle::IssueBundle> inner;

        IssueBundle(IssueBundlePtr *bundle) : inner(issue_bundle::from_raw_box(bundle)) {}

        friend class OrchardWallet;
    public:
        IssueBundle() : inner(issue_bundle::none()) {}

        IssueBundle(IssueBundle &&bundle) : inner(std::move(bundle.inner)) {}

        IssueBundle(const IssueBundle &bundle) :
                inner(bundle.inner->box_clone()) {}

        IssueBundle(
            IssuanceAuthorizingKey isk,
            uint64_t value,
            const libzcash::OrchardRawAddress& recipient,
            const char *asset_descr
        ) : inner(issue_bundle::create_issue_bundle(isk.inner.get(), value, recipient.inner.get(), asset_descr)) {}

        IssueBundle &operator=(IssueBundle &&bundle)
        {
            if (this != &bundle) {
                inner = std::move(bundle.inner);
            }
            return *this;
        }

        IssueBundle &operator=(const IssueBundle &bundle)
        {
            if (this != &bundle) {
                inner = bundle.inner->box_clone();
            }
            return *this;
        }

        const rust::Box<issue_bundle::IssueBundle>& GetDetails() const {
            return inner;
        }

        size_t RecursiveDynamicUsage() const {
            return inner->recursive_dynamic_usage();
        }


        template<typename Stream>
        void Serialize(Stream& s) const {
            try {
                inner->serialize(*ToRustStream(s));
            } catch (const std::exception& e) {
                throw std::ios_base::failure(e.what());
            }
        }

        template<typename Stream>
        void Unserialize(Stream& s) {
            try {
                inner = issue_bundle::parse(*ToRustStream(s));
            } catch (const std::exception& e) {
                throw std::ios_base::failure(e.what());
            }
        }

        /// Returns true if this contains an Issue bundle, or false if there is no
        /// Issue component.
        bool IsPresent() const { return inner->is_present(); }

        const size_t GetNumActions() const {
            return inner->num_actions();
        }

        /// Queues this bundle's authorization for validation.
        ///
        /// `sighash` must be for the transaction this bundle is within.
        void QueueAuthValidation(
            issue_bundle::BatchValidator& batch, const uint256& sighash) const
        {
            batch.add_bundle(inner->box_clone(), sighash.GetRawBytes());
        }
    };

#endif // ZCASH_PRIMITIVES_ISSUE_H

