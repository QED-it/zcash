#!/usr/bin/env python3
# Copyright (c) 2022 The Zcash developers
# Distributed under the MIT software license, see the accompanying
# file COPYING or https://www.opensource.org/licenses/mit-license.php .
import json

from test_framework.test_framework import BitcoinTestFramework
from test_framework.util import (
    NU5_BRANCH_ID,
    assert_equal,
    get_coinbase_address,
    nuparams,
    start_nodes,
    wait_and_assert_operationid_status,
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

        # Get a new Orchard account on node 0
        acct0 = self.nodes[0].z_getnewaccount()['account']
        ua0 = self.nodes[0].z_getaddressforaccount(acct0, ['orchard'])['address']

        # Activate NU5
        self.nodes[0].generate(5)
        self.sync_all()

        # Send a transaction to node 1 so that it has an Orchard note to spend.
        self.nodes[0].issue(0, ua0, "WBTC", 4001, True)

        self.sync_all()
        self.nodes[0].generate(1)
        self.sync_all()

        walletinfo = self.nodes[0].getwalletinfo()

        print(walletinfo)

        assert_equal(len(walletinfo['asset_balances'].items()), 1)
        for key, value in walletinfo['asset_balances'].items():
            assert_equal(value['confirmed_balance'], 4001)

if __name__ == '__main__':
    IssueTest().main()

