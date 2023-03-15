// Copyright (c) 2021-2022 The Zcash developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or https://www.opensource.org/licenses/mit-license.php .

#ifndef ZCASH_PRIMITIVES_ISSUE_H
#define ZCASH_PRIMITIVES_ISSUE_H

#include "streams.h"

#include <amount.h>
#include <rust/issue_bundle.h>
#include "rust/orchard/issuance.h"
#include "zcash/address/orchard.hpp"


    class IssuanceAuthorizingKey {
    private:
        std::unique_ptr<IssuanceAuthorizingKeyPtr, decltype(&issuance_authorizing_key_free)> inner;

        IssuanceAuthorizingKey() : inner(nullptr, issuance_authorizing_key_free) {}

        friend class OrchardSpendingKey;

        friend class IssueBundle;

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


/**
 * The Issue component of an authorized transaction.
 */
    class IssueBundle {
    private:
        /// An optional Issue bundle (with `nullptr` corresponding to `None`).
        /// Memory is allocated by Rust.
        std::unique_ptr<IssueBundlePtr, decltype(&issue_bundle_free)> inner;

        IssueBundle(IssueBundlePtr *bundle) : inner(bundle, issue_bundle_free) {}

    public:
        IssueBundle() : inner(nullptr, issue_bundle_free) {}

        IssueBundle(IssueBundle &&bundle) : inner(std::move(bundle.inner)) {}

        IssueBundle(const IssueBundle &bundle) :
                inner(issue_bundle_clone(bundle.inner.get()), issue_bundle_free) {}

        IssueBundle(IssuanceAuthorizingKey isk) : inner(nullptr, issue_bundle_free) {
            IssueBundlePtr* ptr = create_issue_bundle(isk.inner.get());
            inner.reset(ptr);
        }

        IssueBundle &operator=(IssueBundle &&bundle) {
            if (this != &bundle) {
                inner = std::move(bundle.inner);
            }
            return *this;
        }

        IssueBundle &operator=(const IssueBundle &bundle) {
            if (this != &bundle) {
                inner.reset(issue_bundle_clone(bundle.inner.get()));
            }
            return *this;
        }

        template<typename Stream>
        void Serialize(Stream &s) const {
            RustStream rs(s);
            if (!issue_bundle_serialize(inner.get(), &rs, RustStream<Stream>::write_callback)) {
                throw std::ios_base::failure("Failed to serialize v5 Issue bundle");
            }
        }

        template<typename Stream>
        void Unserialize(Stream &s) {
            RustStream rs(s);
            IssueBundlePtr *bundle;
            if (!issue_bundle_parse(&rs, RustStream<Stream>::read_callback, &bundle)) {
                throw std::ios_base::failure("Failed to parse v5 Issue bundle");
            }
            inner.reset(bundle);
        }

        /// Returns true if this contains an Issue bundle, or false if there is no
        /// Issue component.
        bool IsPresent() const { return (bool) inner; }

        rust::Box <issue_bundle::IssueBundle> GetDetails() const {
            return issue_bundle::from_tx_bundle(reinterpret_cast<issue_bundle::TxIssueBundle *>(inner.get()));
        }

        void AddRecipient(
                uint64_t value,
                libzcash::OrchardRawAddress recipient,
                const char *asset_descr,
                bool finalize
        ) {
            add_recipient(inner.get(), value, recipient.inner.get(), asset_descr, finalize);
        }

        void Sign(IssuanceAuthorizingKey isk) {
            IssueBundlePtr *ptr = sign_issue_bundle(inner.release(), isk.inner.get());
            inner.reset(ptr);
        }
    };

#endif // ZCASH_PRIMITIVES_ISSUE_H

