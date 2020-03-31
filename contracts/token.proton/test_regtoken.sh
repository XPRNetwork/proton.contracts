

#./cleos.sh push action token.proton reg '{"tcontract":"eosio.token","tname":"Proton", "url":"https://protonchain.com", "desc":"Proton (XPR) is a new public blockchain and smart contract platform designed for both consumer applications and peer-peer payments." "iconurl":"", "symbol":"4,XPR"}' -p eosio.token

#./cleos.sh get table token.proton token.proton tokens
#./cleos.sh get table --index 2 --key-type name token.proton token.proton tokens -L eosio.token

#./cleos.sh push action token.proton update '{"id": 1000001, "tcontract":"eosio.token","tname":"Proton", "url":"https://protonchain.com", "desc":"Proton (XPR) is a new public blockchain and smart contract platform designed for both consumer applications and peer-peer payments." "iconurl":"", "symbol":"4,XPR"}' -p eosio.token

#./cleos.sh push action token.proton remove '{"id": 1000001}' -p eosio.token

#./cleos.sh push action token.proton updblacklist '{"id": 1000003, "blisted":1}' -p token.proton