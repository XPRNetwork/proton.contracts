

#./cleos.sh push action token.proton reg '{"tcontract":"eosio.token","tname":"Proton", "url":"https://protonchain.com", "desc":"Proton (XPR) is a new public blockchain and smart contract platform designed for both consumer applications and peer-peer payments." "iconurl":"https://firebasestorage.googleapis.com/v0/b/eos-lynx.appspot.com/o/eosio-tokenXPR-testnet.png?alt=media&token=d48f1e4d-4aa0-4ea7-8afe-c73266eb140e", "symbol":"4,XPR"}' -p eosio.token

#./cleos.sh get table token.proton token.proton tokens

#./cleos.sh push action token.proton update '{"id": 1000001, "tcontract":"eosio.token","tname":"Proton", "url":"https://protonchain.com", "desc":"Proton (XPR) is a new public blockchain and smart contract platform designed for both consumer applications and peer-peer payments." "iconurl":"https://firebasestorage.googleapis.com/v0/b/eos-lynx.appspot.com/o/eosio-tokenXPR-testnet.png?alt=media&token=d48f1e4d-4aa0-4ea7-8afe-c73266eb140e", "symbol":"4,XPR"}' -p eosio.token

#./cleos.sh push action token.proton remove '{"id": 1000001}' -p eosio.token