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
from test_framework.mininode import CTransaction
from io import BytesIO
from binascii import unhexlify

def issue_asset(self, node, address, asset_descr, amount, finalize):
    txid = self.nodes[node].issue(0, address, asset_descr, amount, finalize)

    tx = CTransaction()
    f = BytesIO(unhexlify(self.nodes[node].getrawtransaction(txid).encode('ascii')))
    tx.deserialize(f)

    self.sync_all()
    self.nodes[0].generate(1)
    self.sync_all()

    return tx.issueBundle.actions[0].notes[0].asset.hex()


def check_asset_balance(self, node, asset, expected_balance):
    walletinfo = self.nodes[node].getwalletinfo()
    assert_equal(walletinfo['asset_balances'][asset]['confirmed_balance'], expected_balance)


def orchard_address(node):
    acct = node.z_getnewaccount()['account']
    return node.z_getaddressforaccount(acct, ['orchard'])['address']


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

        # Activate NU5
        self.nodes[0].generate(5)
        self.sync_all()

        address0 = orchard_address(self.nodes[0])
        address1 = orchard_address(self.nodes[1])

        # Issue assets to an address on node 0
        asset0 = issue_asset(self, 0, address0, "WBTC", 4000, False)
        issue_asset(self, 0, address0, "WBTC", 2, False)
        issue_asset(self, 0, address1, "WBTC", 23, False)

        # Issue assets to an address on node 1
        asset1 = issue_asset(self, 0, address1, "WETH", 42, False)

        check_asset_balance(self, 0, asset0, 4002)
        check_asset_balance(self, 1, asset0, 23)
        check_asset_balance(self, 1, asset1, 42)

if __name__ == '__main__':
    IssueTest().main()

