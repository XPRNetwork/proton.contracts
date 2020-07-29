
### Deploy
#./cleos.sh set contract eosio.proton ./eosio.proton -p eosio.proton


### Admins actions ################
#
#ALL PERMISSIONS: ["createacc",1],["vote",1],["regprod",1],["regproxy",1],["setcontract",1],["namebids",1],["rex",1],["delegate",1],["undelegate",1],["sellram",1],["buyram",1]]
#
#./cleos.sh push action eosio.proton setperm '{"name": "cryptoloins", "perms": [["regprod",1]]}' -p eosio.proton@active
#./cleos.sh push action eosio.proton setdappconf '[2097152, 200000, 200000]' -p eosio.proton

#./cleos.sh push action eosio.proton setuserava '{"acc": "tester", "ava": "ava_base64"}' -p wlcm.proton@update
#./cleos.sh push action eosio.proton setusername '{"acc": "tester", "name": "Bohdan CryptoLions"}' -p wlcm.proton@update
#./cleos.sh push action eosio.proton userverify '{"acc": "tester", "verifier": "cryptoloins", "verified": 1}' -p admin.proton@verifier -p cryptolions

#./cleos.sh push action eosio.proton remove '{"acc": "tester"}' -p wlcm.proton@update

#./cleos.sh push action eosio.proton kickbp '{"producer": "tester"}' -p admin.proton@committee



### Users Actions ##################
#
#./cleos.sh push action eosio.proton dappreg '{"account": "cryptolions"}' -p cryptolions
#./cleos.sh push action eosio.proton reqperm '{"acc": "cryptolions", "permission": "namebids"}' -p cryptolions

#./cleos.sh push action eosio.proton updateraccs '{"acc": "tester1", "raccs": ["taskly", "tasklybank", "tasklyother"]}' -p tester1
#./cleos.sh push action eosio.proton updateaacts '{"acc": "tester1", "aacts": [["eosio.token","transfer"], ["taskly", "markcheck"], ["taskly", "removecheck"]]}' -p tester1
#./cleos.sh push action eosio.proton updateac '{"acc": "tester1", "ac": [["eosio.token","XPR"], ["eosio.token","XYZ"], ["usdtoken","USD"]]}' -p tester1


## Committee KickBP Permissions ##################
# ./cleos.sh push action eosio updateauth '{"account": "eosio", "permission": "committee",  "parent": "active",  "auth": { "threshold": 1, "keys": [], "waits": [], "accounts": [{ "weight": 1, "permission": {"actor": "admin.proton", "permission": "committee"} }] } } ' -p eosio
# ./cleos.sh set action permission eosio eosio kickbp committee
# ./cleos.sh set action permission eosio eosio.proton kickbp committee
#
# ./cleos.sh push action eosio kickbp '{"producer": "protonbpa"}' -p eosio@committee


### Tables ################
#
#./cleos.sh get table eosio.proton eosio.proton usersinfo
#./cleos.sh get table eosio.proton eosio.proton permissions
#./cleos.sh get table eosio.proton eosio.proton dappconf