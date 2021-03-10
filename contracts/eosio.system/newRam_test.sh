#### Base ram buy/sell actions which allow buy RAM for SYS was renamed:
#
# buyram -> buyramsys
# buyrambytes -> buyrambsys
# sellram -> sellramsys


##### Buying 100 bytes using XPR amount
#
# ./cleos.sh system buyram tester1 tester1 "2.2222 XPR"
# ./cleos.sh system sellram tester1 100


##### Buy 1 KB= 1024 bytes RAM
#
# ./cleos.sh system buyram tester1 tester1 -k 1
# ./cleos.sh system sellram tester1 1024


##### Increase users RAM limit
#
# ./cleos.sh push action eosio ramlimitset '["tester4", 1048576]' -p admin.proton@light



#### Increase RAM limit for user
#
# ./cleos.sh push action eosio ramlimitset '["tester4", 1048576]' -p admin.proton@light

#### Set RAM Options
#
#./cleos.sh push action eosio setramoption '{"ram_price_per_byte": "0.0200 XPR", "max_per_user_bytes": 3145728, "ram_fee_percent": 1000}' -p admin.proton@light


#### allow admin.proton@llight to call ram confi actions (add permissions, require msig)
#
# ./cleos.sh set action permission admin.proton eosio ramlimitset light
# ./cleos.sh set action permission admin.proton eosio setramoption light




##### TABLES ####################

# ./cleos.sh get table eosio eosio globalram

# ACC=tester1
# ./cleos.sh get table eosio $ACC usersram

# ./cleos.sh get table eosio eosio rammarket | grep balance
