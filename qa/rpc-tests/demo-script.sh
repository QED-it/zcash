#! /bin/sh

echo "=========================================================================="
echo "ZSA DEMO FOR ZCON4                                                        "
echo "=========================================================================="

echo "Please ensure you are in the root directory of the zcash repo, and that"
echo "you are already running python3 qa/rpc-tests/demo-setup.py on another"
echo "terminal."

echo "To confirm the above and proceed to Asset Issuance:"
while true; do
	read -p "Please press y to continue, n to exit: " yn
	case $yn in
		[Yy]* ) break;;
		[Nn]* ) exit;;
		* ) echo "Please answer y or n"
	esac
done

echo "\n--------------------------------------------------------------------------"
echo "Dr. Alice issues Rx to self:                                              "
echo "--------------------------------------------------------------------------\n"

echo "\nWe will run the following curl command:\n\n"

echo "curl --user `cat qa/rpc-tests/testfiles/auth_alice.txt` --data-binary '{\"jsonrpc\": \"1.0\", \"id\":\"curltest\",\"method\": \"issue\", \"params\": [0,\"`cat qa/rpc-tests/testfiles/ua1_alice.txt`\", \"Rx date: 2023-08-01, 10 tabs XYZ\", 0.00000001, true] }' -H 'content-type: text/plain;' http://127.0.0.1:`cat qa/rpc-tests/testfiles/port_alice.txt`/ | jq"
echo "\n................................................................"

curl --user `cat qa/rpc-tests/testfiles/auth_alice.txt` --data-binary '{"jsonrpc": "1.0", "id":"curltest","method": "issue", "params": [0,"'"`cat qa/rpc-tests/testfiles/ua1_alice.txt`"'", "Rx date: 2023-08-01, 10 tabs XYZ", 0.00000001, true] }' -H 'content-type: text/plain;' http://127.0.0.1:`cat qa/rpc-tests/testfiles/port_alice.txt`/ | jq

while true; do
        read -p "Please press y to continue, n to exit: " yn
        case $yn in
                [Yy]* ) break;;
                [Nn]* ) exit;;
                * ) echo "Please answer y or n"
        esac
done

echo "\n......................................................................."
echo "We now generate some blocks:"
echo "Please visit the python script terminal and generate some blocks"
echo ".........................................................................\n"

while true; do
        read -p "Please press y to continue, n to exit: " yn
        case $yn in
                [Yy]* ) break;;
                [Nn]* ) exit;;
                * ) echo "Please answer y or n"
        esac
done

echo "\n......................................................................."
echo "\n Let us now look at Alice's wallet. We run the following command:\n"

echo "curl --user `cat qa/rpc-tests/testfiles/auth_alice.txt` --data-binary '{\"jsonrpc\": \"1.0\", \"id\":\"curltest\", \"method\": \"getwalletinfo\", \"params\": [] }' -H 'content-type: text/plain;' http://127.0.0.1:`cat qa/rpc-tests/testfiles/port_alice.txt`/ | jq"

echo "\n.........................................................................."
echo " ALICE'S WALLET                                                           "
echo "..........................................................................\n"

curl --user `cat qa/rpc-tests/testfiles/auth_alice.txt` --data-binary '{"jsonrpc": "1.0", "id":"curltest", "method": "getwalletinfo", "params": [] }' -H 'content-type: text/plain;' http://127.0.0.1:`cat qa/rpc-tests/testfiles/port_alice.txt`/ | jq

echo "..........................................................................\n"

curl --user `cat qa/rpc-tests/testfiles/auth_alice.txt` --data-binary '{"jsonrpc": "1.0", "id":"curltest", "method": "getwalletinfo", "params": [] }' -H 'content-type: text/plain;' http://127.0.0.1:`cat qa/rpc-tests/testfiles/port_alice.txt`/ | jq '.result.asset_balances |keys |.[0]' -r > qa/rpc-tests/testfiles/asset_base.txt

echo "Notice the presence of the issued prescription asset in Alice's wallet:"
echo "The asset base is `cat qa/rpc-tests/testfiles/asset_base.txt`"

echo ".........................................................................."
echo "..........................................................................\n"

echo "This successfully demonstrates issuance of the Asset. We will now"
echo "demonstrate shielded asset transfer. To proceed:"
while true; do
        read -p "Please press y to continue, n to exit: " yn
        case $yn in
                [Yy]* ) break;;
                [Nn]* ) exit;;
                * ) echo "Please answer y or n"
        esac
done

echo "\n--------------------------------------------------------------------------"
echo "Dr. Alice transfers Rx to Felix:                                          "
echo "--------------------------------------------------------------------------\n"

echo "We will run the following curl command:\n\n"

echo "curl --user `cat qa/rpc-tests/testfiles/auth_alice.txt` --data-binary '{\"jsonrpc\": \"1.0\", \"id\":\"curltest\", \"method\": \"z_sendassets\", \"params\": [\"`cat qa/rpc-tests/testfiles/ua1_alice.txt`\", [{\"address\": \"`cat qa/rpc-tests/testfiles/ua0_felix.txt`\", \"amount\": 0.00000001, \"asset\": \"`cat qa/rpc-tests/testfiles/asset_base.txt`\", "memo": "519a11ce"}], 1] }' -H 'content-type: text/plain;' http://127.0.0.1:`cat qa/rpc-tests/testfiles/port_alice.txt`/ | jq"

curl --user `cat qa/rpc-tests/testfiles/auth_alice.txt` --data-binary '{"jsonrpc": "1.0", "id":"curltest","method": "z_sendassets", "params": ["'"`cat qa/rpc-tests/testfiles/ua1_alice.txt`"'",[{"address":"'"`cat qa/rpc-tests/testfiles/ua0_felix.txt`"'", "amount": 0.00000001, "asset": "'"`cat qa/rpc-tests/testfiles/asset_base.txt`"'", "memo": "519a11ce"}], 1] }' -H 'content-type: text/plain;' http://127.0.0.1:`cat qa/rpc-tests/testfiles/port_alice.txt`/ | jq

sleep 15

echo "\n.........................................................................."
echo "Let us now generate blocks and look at the wallets of Alice and Felix:\n"
echo "Please visit the python script terminal and generate some blocks"
echo "..........................................................................\n"

while true; do
        read -p "Please press y to continue, n to exit: " yn
        case $yn in
                [Yy]* ) break;;
                [Nn]* ) exit;;
                * ) echo "Please answer y or n"
        esac
done


echo "\n.........................................................................."
echo " ALICE'S WALLET "
echo "..........................................................................\n"

curl --user `cat qa/rpc-tests/testfiles/auth_alice.txt` --data-binary '{"jsonrpc": "1.0", "id":"curltest", "method": "getwalletinfo", "params": [] }' -H 'content-type: text/plain;' http://127.0.0.1:`cat qa/rpc-tests/testfiles/port_alice.txt`/ | jq

echo "\n.........................................................................."
echo " FELIX'S WALLET "
echo "..........................................................................\n"

curl --user `cat qa/rpc-tests/testfiles/auth_felix.txt` --data-binary '{"jsonrpc": "1.0", "id":"curltest", "method": "getwalletinfo", "params": [] }' -H 'content-type: text/plain;' http://127.0.0.1:`cat qa/rpc-tests/testfiles/port_felix.txt`/ | jq

echo "..........................................................................\n"

echo "This successfully demonstrates transfer of the Asset from Dr. Alice to Felix."
echo "We will now demonstrate shielded asset transfer from Felix to the manufacturer."
echo "To proceed:"
while true; do
        read -p "Please press y to continue, n to exit: " yn
        case $yn in
                [Yy]* ) break;;
                [Nn]* ) exit;;
                * ) echo "Please answer y or n"
        esac
done

echo "\n--------------------------------------------------------------------------"
echo "Felix transfers Rx to the manufacturer:                                "
echo "--------------------------------------------------------------------------\n"

echo "We will run the following curl command:\n\n"

echo "curl --user `cat qa/rpc-tests/testfiles/auth_felix.txt` --data-binary '{\"jsonrpc\": \"1.0\", \"id\":\"curltest\", \"method\": \"z_sendassets\", \"params\": [\"`cat qa/rpc-tests/testfiles/ua0_felix.txt`\", [{\"address\": \"`cat qa/rpc-tests/testfiles/ua0_mfg.txt`\", \"amount\": 0.00000001, \"asset\": \"`cat qa/rpc-tests/testfiles/asset_base.txt`\", , "memo": "519a11ce"}], 1] }' -H 'content-type: text/plain;' http://127.0.0.1:`cat qa/rpc-tests/testfiles/port_felix.txt`/ | jq"

curl --user `cat qa/rpc-tests/testfiles/auth_felix.txt` --data-binary '{"jsonrpc": "1.0", "id":"curltest","method": "z_sendassets", "params": ["'"`cat qa/rpc-tests/testfiles/ua0_felix.txt`"'",[{"address":"'"`cat qa/rpc-tests/testfiles/ua0_mfg.txt`"'", "amount": 0.00000001, "asset": "'"`cat qa/rpc-tests/testfiles/asset_base.txt`"'", "memo": "519a11ce"}], 1] }' -H 'content-type: text/plain;' http://127.0.0.1:`cat qa/rpc-tests/testfiles/port_felix.txt`/ | jq

sleep 15

echo "\n.........................................................................."
echo "Let us now generate blocks and look at the wallets of Felix and" 
echo "the manufacturer:"
echo "Please visit the python script terminal and generate some blocks"
echo "..........................................................................\n"

while true; do
        read -p "Please press y to continue, n to exit: " yn
        case $yn in
                [Yy]* ) break;;
                [Nn]* ) exit;;
                * ) echo "Please answer y or n"
        esac
done

echo "\n.........................................................................."
echo " FELIX'S WALLET "
echo "..........................................................................\n"

curl --user `cat qa/rpc-tests/testfiles/auth_felix.txt` --data-binary '{"jsonrpc": "1.0", "id":"curltest", "method": "getwalletinfo", "params": [] }' -H 'content-type: text/plain;' http://127.0.0.1:`cat qa/rpc-tests/testfiles/port_felix.txt`/ | jq

echo "\n.........................................................................."
echo " MANUFACTURER'S WALLET "
echo "..........................................................................\n"

curl --user `cat qa/rpc-tests/testfiles/auth_mfg.txt` --data-binary '{"jsonrpc": "1.0", "id":"curltest", "method": "getwalletinfo", "params": [] }' -H 'content-type: text/plain;' http://127.0.0.1:`cat qa/rpc-tests/testfiles/port_mfg.txt`/ | jq

echo "This successfully demonstrates transfer of the Asset from Felix to the manufacturer."

echo "============================================================================"
echo "THANK YOU!                                                                  "
echo "============================================================================"

#rm -rf qa/rpc-tests/testfiles
