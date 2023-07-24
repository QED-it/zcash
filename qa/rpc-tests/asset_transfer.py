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
    wait_and_assert_operationid_status
)
from asset_issuance import issue_asset, check_asset_balance, orchard_address


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
        address2 = orchard_address(self.nodes[2])

        issued = 4003
        transfer1 = 1
        transfer2 = 2

        # Issue assets to an address on node 0
        asset = issue_asset(self, 0, address0, "WBTC", issued, True)
        check_asset_balance(self, 0, asset, issued)

        # Send assets from node 0 to node 1
        recipients = [{"address": address1, "amount": transfer1, "asset": asset}, {"address": address2, "amount": transfer2, "asset": asset}]
        opid = self.nodes[0].z_sendassets(address0, recipients, 1)
        wait_and_assert_operationid_status(self.nodes[0], opid)

        self.sync_all()
        self.nodes[0].generate(1)
        self.sync_all()

        check_asset_balance(self, 0, asset, issued - transfer1 - transfer2)
        check_asset_balance(self, 1, asset, transfer1)
        check_asset_balance(self, 2, asset, transfer2)


if __name__ == '__main__':
    IssueTest().main()

