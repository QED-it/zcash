#!/usr/bin/env python3
# Copyright (c) 2022 The Zcash developers
# Distributed under the MIT software license, see the accompanying
# file COPYING or https://www.opensource.org/licenses/mit-license.php .

from test_framework.test_framework import BitcoinTestFramework
from test_framework.util import (
    NU5_BRANCH_ID,
    assert_equal,
    get_coinbase_address,
    nuparams,
    start_nodes,
    wait_and_assert_operationid_status,
)

class ZecTest(BitcoinTestFramework):
    def __init__(self):
        super().__init__()
        self.num_nodes = 4

    def setup_nodes(self):
        return start_nodes(self.num_nodes, self.options.tmpdir, [[
            nuparams(NU5_BRANCH_ID, 205),
            '-regtestwalletsetbestchaineveryblock'
        ]] * self.num_nodes)

    def run_test(self):
        # Sanity-check the test harness
        assert_equal(self.nodes[0].getblockcount(), 200)

        # Get a new Orchard account on node 0
        acct0 = self.nodes[0].z_getnewaccount()['account']
        ua0 = self.nodes[0].z_getaddressforaccount(acct0, ['orchard'])['address']

        # Activate NU5
        self.nodes[0].generate(5)
        self.sync_all()

        # Get a recipient address
        acct1 = self.nodes[1].z_getnewaccount()['account']
        ua1 = self.nodes[1].z_getaddressforaccount(acct1, ['orchard'])['address']

        # Send a transaction to node 1 so that it has an Orchard note to spend.
        recipients = [{"address": ua1, "amount": 10}]
        myopid = self.nodes[0].z_sendmany(get_coinbase_address(self.nodes[0]), recipients, 1, 0, 'AllowRevealedSenders')
        wait_and_assert_operationid_status(self.nodes[0], myopid)

        self.sync_all()
        self.nodes[0].generate(1)
        self.sync_all()

        # Check the value sent to ua1 was received
        assert_equal(
            {'pools': {'orchard': {'valueZat': 10_0000_0000}}, 'minimum_confirmations': 1},
            self.nodes[1].z_getbalanceforaccount(acct1))

        # Create an Orchard spend, so that the note commitment tree root gets altered.
        recipients = [{"address": ua0, "amount": 1}]
        myopid = self.nodes[1].z_sendmany(ua1, recipients, 1, 0)
        wait_and_assert_operationid_status(self.nodes[1], myopid)

        self.sync_all()
        self.nodes[0].generate(1)
        self.sync_all()

        # Verify the balance on both nodes
        assert_equal(
            {'pools': {'orchard': {'valueZat': 9_0000_0000}}, 'minimum_confirmations': 1},
            self.nodes[1].z_getbalanceforaccount(acct1))

        assert_equal(
            {'pools': {'orchard': {'valueZat': 1_0000_0000}}, 'minimum_confirmations': 1},
            self.nodes[0].z_getbalanceforaccount(acct0))

        walletinfo = self.nodes[0].getwalletinfo()
        print(walletinfo)


if __name__ == '__main__':
    ZecTest().main()

