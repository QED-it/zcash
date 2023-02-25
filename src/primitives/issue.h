// Copyright (c) 2021-2022 The Zcash developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or https://www.opensource.org/licenses/mit-license.php .

#ifndef ZCASH_PRIMITIVES_ISSUE_H
#define ZCASH_PRIMITIVES_ISSUE_H

#include "streams.h"

#include <amount.h>

/**
 * The Issue component of an authorized transaction.
 */
class IssueBundle
{
private:
    /// An optional Issue bundle (with `nullptr` corresponding to `None`).
    /// Memory is allocated by Rust.
    std::unique_ptr<IssueBundlePtr, decltype(&issue_bundle_free)> inner;

    IssueBundle(IssueBundlePtr* bundle) : inner(bundle, issue_bundle_free) {}
    
public:
    IssueBundle() : inner(nullptr, issue_bundle_free) {}

    IssueBundle(IssueBundle&& bundle) : inner(std::move(bundle.inner)) {}

    IssueBundle(const IssueBundle& bundle) :
        inner(issue_bundle_clone(bundle.inner.get()), issue_bundle_free) {}

    IssueBundle& operator=(IssueBundle&& bundle)
    {
        if (this != &bundle) {
            inner = std::move(bundle.inner);
        }
        return *this;
    }

    IssueBundle& operator=(const IssueBundle& bundle)
    {
        if (this != &bundle) {
            inner.reset(issue_bundle_clone(bundle.inner.get()));
        }
        return *this;
    }

    template<typename Stream>
    void Serialize(Stream& s) const {
        RustStream rs(s);
        if (!issue_bundle_serialize(inner.get(), &rs, RustStream<Stream>::write_callback)) {
            throw std::ios_base::failure("Failed to serialize v5 Issue bundle");
        }
    }

    template<typename Stream>
    void Unserialize(Stream& s) {
        RustStream rs(s);
        IssueBundlePtr* bundle;
        if (!issue_bundle_parse(&rs, RustStream<Stream>::read_callback, &bundle)) {
            throw std::ios_base::failure("Failed to parse v5 Issue bundle");
        }
        inner.reset(bundle);
    }

    /// Returns true if this contains an Issue bundle, or false if there is no
    /// Issue component.
    bool IsPresent() const { return (bool)inner; }

//    const size_t GetNumActions() const {
//        return issue_bundle_actions_len(inner.get());
//    }

//    void AddRecipient(
//            uint64_t value,
//            libzcash::OrchardRawAddress recipient,
//            const char *asset_descr,
//            size_t asset_descr_len,
//            bool finalize
//    ) {
//        add_recipient(inner.get(), value, recipient.inner.get(), asset_descr, asset_descr_len, finalize);
//    }
//
//    void Sign(IssuanceAuthorizingKey isk) {
//        IssueBundlePtr* ptr = sign_issue_bundle(inner.release(), isk.inner.get());
//        inner.reset(ptr);
//    }
};

#endif // ZCASH_PRIMITIVES_ISSUE_H

