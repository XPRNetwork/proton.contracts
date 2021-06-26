
### Deploy
#./cleos.sh set contract eosio.proton ./eosio.proton -p eosio.proton


### Admins actions ################
#
#ALL PERMISSIONS: ["createacc",1],["vote",1],["regprod",1],["regproxy",1],["setcontract",1],["namebids",1],["rex",1],["delegate",1],["undelegate",1],["sellram",1],["buyram",1]]
#
#./cleos.sh push action eosio.proton setperm '{"name": "cryptoloins", "perms": [["regprod",1]]}' -p eosio.proton@active

#./cleos.sh push action eosio.proton setuserava '{"acc": "tester", "ava": "ava_base64"}' -p wlcm.proton@update
#./cleos.sh push action eosio.proton setusername '{"acc": "tester", "name": "Bohdan CryptoLions"}' -p wlcm.proton@update
#./cleos.sh push action eosio.proton userverify '{"acc": "tester", "verifier": "cryptoloins", "verified": 1}' -p admin.proton@verifier -p cryptolions

#./cleos.sh push action eosio.proton remove '{"acc": "tester"}' -p wlcm.proton@update

#./cleos.sh push action eosio.proton kickbp '{"producer": "tester"}' -p admin.proton@committee


### Users Actions ##################
#
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


## KYC logic ###############################
#
# - Add Permissions
# ./cleos.sh set action permission admin.proton eosio.proton addkycprov committee -p admin.proton
# ./cleos.sh set action permission admin.proton eosio.proton blkycprov committee -p admin.proton
# ./cleos.sh set action permission admin.proton eosio.proton rmvkycprov committee -p admin.proton


## ADD KYC Provider
#
##./cleos.sh push action eosio.proton addkycprov '{"kyc_provider": "test1", "desc": "My desc as KYC testT prov 1", "url": "protonchain.com", "iconurl": "https://www.protonchain.com/static/protonLogo@3x-f30ef900442eabc13be4f1bcf1ad6683.png", "name": "Super Tester"}' -p admin.proton@committee
#./cleos.sh push action eosio.proton addkycprov '{"kyc_provider": "test1", "desc": "My desc as KYC testT prov 1", "url": "protonchain.com", "iconurl": "https://www.protonchain.com/static/protonLogo@3x-f30ef900442eabc13be4f1bcf1ad6683.png", "name": "Super Tester1"}' -p admin.proton@light
#./cleos.sh push action eosio.proton addkycprov '{"kyc_provider": "test2", "desc": "My desc as KYC testT prov 2", "url": "protonchain.com", "iconurl": "https://www.protonchain.com/static/protonLogo@3x-f30ef900442eabc13be4f1bcf1ad6683.png", "name": "Super Tester2"}' -p admin.proton@light
#./cleos.sh push action eosio.proton addkycprov '{"kyc_provider": "test3", "desc": "My desc as KYC testT prov 3", "url": "protonchain.com", "iconurl": "https://www.protonchain.com/static/protonLogo@3x-f30ef900442eabc13be4f1bcf1ad6683.png", "name": "Super Tester3"}' -p admin.proton@light
#./cleos.sh push action eosio.proton addkycprov '{"kyc_provider": "test4", "desc": "My desc as KYC testT prov 4", "url": "protonchain.com", "iconurl": "https://www.protonchain.com/static/protonLogo@3x-f30ef900442eabc13be4f1bcf1ad6683.png", "name": "Super Tester4"}' -p admin.proton@light
#./cleos.sh push action eosio.proton addkycprov '{"kyc_provider": "test5", "desc": "My desc as KYC testT prov 5", "url": "protonchain.com", "iconurl": "https://www.protonchain.com/static/protonLogo@3x-f30ef900442eabc13be4f1bcf1ad6683.png", "name": "Super Tester5"}' -p admin.proton@light


## BList KYC Provider
#
##./cleos.sh push action eosio.proton blkycprov '{"kyc_provider": "tester", "state": true}' -p admin.proton@committee
#./cleos.sh push action eosio.proton blkycprov '{"kyc_provider": "tester", "state": true}' -p admin.proton@light
#./cleos.sh push action eosio.proton blkycprov '{"kyc_provider": "tester", "state": false}' -p admin.proton@light


## Remove KYC Provider
#
##./cleos.sh push action eosio.proton rmvkycprov '{"kyc_provider": "tester"}' -p admin.proton@committee
#./cleos.sh push action eosio.proton rmvkycprov '{"kyc_provider": "tester"}' -p admin.proton@light


## Add KYC
#./cleos.sh push action eosio.proton addkyc '{"acc": "cryptolions", "kyc":{ "kyc_provider": "test1", "kyc_level": "level 1, some data", "kyc_date": 1602532882}}' -p test1
#./cleos.sh push action eosio.proton addkyc '{"acc": "cryptolions", "kyc":{ "kyc_provider": "test2", "kyc_level": "level 2, some data", "kyc_date": 1602532882}}' -p test2
#./cleos.sh push action eosio.proton addkyc '{"acc": "bohdan", "kyc":{ "kyc_provider": "test3", "kyc_level": "level 3, some data", "kyc_date": 1602532882}}' -p test3
#./cleos.sh push action eosio.proton addkyc '{"acc": "bohdan", "kyc":{ "kyc_provider": "test4", "kyc_level": "level 4, some data", "kyc_date": 1602532882}}' -p test4
#./cleos.sh push action eosio.proton addkyc '{"acc": "bohdan", "kyc":{ "kyc_provider": "test5", "kyc_level": "level 5, some data", "kyc_date": 1602532882}}' -p test5

## Update KYC
#./cleos.sh push action eosio.proton updatekyc '{"acc": "bohdan", "kyc":{ "kyc_provider": "test3", "kyc_level": "level 333, some data", "kyc_date": 1602532882}}' -p test3

## Remove KYC
#./cleos.sh push action eosio.proton removekyc '{"acc": "bohdan", "kyc_provider": "test5"}' -p test5



### Tables ################
#
#./cleos.sh get table eosio.proton eosio.proton usersinfo
#./cleos.sh get table eosio.proton eosio.proton permissions
#./cleos.sh get table eosio.proton eosio.proton kycproviders


####

