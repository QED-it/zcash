#!/usr/bin/env python3
# Copyright (c) 2022 The Zcash developers
# Distributed under the MIT software license, see the accompanying
# file COPYING or https://www.opensource.org/licenses/mit-license.php .

from test_framework.test_framework import BitcoinTestFramework
from test_framework.util import (
    NU5_BRANCH_ID,
    assert_equal,
    nuparams,
    start_nodes,
)

class IssueTest(BitcoinTestFramework):
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

        # Get a new Orchard account on node 0 (Alice)
        acct0_alice = self.nodes[0].z_getnewaccount()['account']
        ua0_alice = self.nodes[0].z_getaddressforaccount(acct0_alice, ['orchard'])['address']

        # Get a new Orchard account on node 1 (Felix)
        acct0_felix = self.nodes[1].z_getnewaccount()['account']
        ua0_felix = self.nodes[1].z_getaddressforaccount(acct0_felix, ['orchard'])['address']

        # Get a new Orchard account on node 2 (Bob)
        acct0_bob = self.nodes[2].z_getnewaccount()['account']
        ua0_bob = self.nodes[2].z_getaddressforaccount(acct0_bob, ['orchard'])['address']

        # Activate NU5
        self.nodes[0].generate(5)
        self.sync_all()

        # Print details post account setup:

        #Alice:
        print("\n\nAlice's Unified Address on Account 0 is:", ua0_alice)
        print("\nAlice's wallet details:\n")
        print(self.nodes[0].getwalletinfo())

        #Felix:
        print("\n\nFelix's Unified Address on Account 0 is:", ua0_felix)
        print("\nFelix's wallet details:\n")
        print(self.nodes[1].getwalletinfo())

        #Bob:
        print("\n\nBob's Unified Address on Account 0 is:", ua0_bob)
        print("\nBob's wallet details:\n")
        print(self.nodes[2].getwalletinfo())

        # Issue assets to an address on node 0
        self.nodes[0].issue(0, ua0_alice, "WBTC", 4001, True)

        # Issue assets to an address on node 1
        self.nodes[0].issue(0, ua0_felix, "WBTC", 42, True)

        self.sync_all()
        self.nodes[0].generate(1)
        self.sync_all()

        walletinfo0 = self.nodes[0].getwalletinfo()
        print(walletinfo0)
        assert_equal(len(walletinfo0['asset_balances'].items()), 1)
        for key, value in walletinfo0['asset_balances'].items():
            assert_equal(value['confirmed_balance'], 4001)


        walletinfo1 = self.nodes[1].getwalletinfo()
        print(walletinfo1)
        assert_equal(len(walletinfo1['asset_balances'].items()), 1)
        for key, value in walletinfo1['asset_balances'].items():
            assert_equal(value['confirmed_balance'], 42)

if __name__ == '__main__':
    IssueTest().main()
