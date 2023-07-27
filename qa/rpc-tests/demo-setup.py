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
    rpc_url,
)

import time

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
        acct1_alice = self.nodes[0].z_getnewaccount()['account']
        ua0_alice = self.nodes[0].z_getaddressforaccount(acct0_alice, ['orchard'])['address']
        ua1_alice = self.nodes[0].z_getaddressforaccount(acct1_alice, ['orchard'])['address']

        # Get a new Orchard account on node 1 (Felix)
        acct0_felix = self.nodes[1].z_getnewaccount()['account']
        ua0_felix = self.nodes[1].z_getaddressforaccount(acct0_felix, ['orchard'])['address']

        # Get a new Orchard account on node 2 (Manufacturer)
        acct0_mfg = self.nodes[2].z_getnewaccount()['account']
        ua0_mfg = self.nodes[2].z_getaddressforaccount(acct0_mfg, ['orchard'])['address']

        # Activate NU5
        self.nodes[0].generate(5)
        self.sync_all()

        # Print details post account setup:

        #Alice:
        print("\n\nAlice's port is:", rpc_url(0))
        print("\nAlice's Unified Address on Account 0 is:", ua0_alice)
        print("\nAlice's Unified Address on Account 1 is:", ua1_alice)
        print("\nAlice's wallet details:\n")
        print(self.nodes[0].getwalletinfo())

        #Felix:
        print("\n\nFelix's port is:", rpc_url(1))
        print("\nFelix's Unified Address on Account 0 is:", ua0_felix)
        print("\nFelix's wallet details:\n")
        print(self.nodes[1].getwalletinfo())

        #Manufacturer:
        print("\n\nThe manufacturer's port is:", rpc_url(2))
        print("\nThe manufacturer's Unified Address on Account 0 is:", ua0_mfg)
        print("\nThe manufacturer's wallet details:\n")
        print(self.nodes[2].getwalletinfo())

        counter = 0
        while True: 
            n = input("\n\nPlease input the number of blocks to generate and press Enter, or press Ctrl+C to exit: ")
            nx = counter % self.num_nodes
            self.nodes[nx].generate(int(n))
            counter += 1


if __name__ == '__main__':
    IssueTest().main()
