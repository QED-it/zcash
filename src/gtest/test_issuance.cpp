#include "chainparams.h"
#include "consensus/params.h"
#include "consensus/consensus.h"
#include "consensus/validation.h"
#include "key_io.h"
#include "main.h"
#include "pubkey.h"
#include "rpc/protocol.h"
#include "transaction_builder.h"
#include "gtest/test_transaction_builder.h"
#include "gtest/utils.h"
#include "util/test.h"
#include "zcash/Address.hpp"
#include "zcash/address/mnemonic.h"
#include "Asset.h"
#include "rust/orchard/keys.h"
#include "rust/orchard/issuance.h"
#include <rust/issue_bundle.h>
#include <primitives/issue.h>

#include <optional>

#include <gmock/gmock.h>
#include <gtest/gtest.h>


TEST(Issuance, BasicIssuanceFlow)
{
    auto consensusParams = RegtestActivateNU5();

    CBasicKeyStore keystore;
    CKey tsk = AddTestCKeyToKeyStore(keystore);
    auto scriptPubKey = GetScriptForDestination(tsk.GetPubKey().GetID());

    auto coinType = Params().BIP44CoinType();
    auto seed = MnemonicSeed::Random(coinType);
    auto sk = libzcash::OrchardSpendingKey::ForAccount(seed, coinType, 0);
    auto fvk = sk.ToFullViewingKey();
    auto ivk = fvk.ToIncomingViewingKey();
    auto isk = sk.ToIssuanceAuthorizingKey();
    libzcash::diversifier_index_t j(0);
    auto recipient = ivk.Address(j);
    const char *asset_descr = (const char *)"Asset description";

    IssueBundle bundle = IssueBundle(isk);

    bundle.AddRecipient(42, recipient, asset_descr, false);

    bundle.Sign(isk);

    auto rustBundle = bundle.GetDetails();

    EXPECT_EQ(rustBundle->num_actions(), 1);

    for (const auto& action : rustBundle->actions()) {
        EXPECT_FALSE(action.is_finalized());
        for (const auto& note : action.notes()) {
            EXPECT_EQ(note.value(), 42);
            // TODO check asset id and recipient
        }
    }

    // Revert to default
    RegtestDeactivateNU5();
}