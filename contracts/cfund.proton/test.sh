
### Deploy
#./cleos.sh set account permission cfund.proton active --add-code -p cfund.proton@active
#./cleos.sh set contract cfund.proton ./cfund.proton -p cfund.proton

### Admins actions
#./cleos.sh push action cfund.proton reg '{"account":"cryptolions"}' -p cfund.proton@active
#./cleos.sh push action cfund.proton unreg '{"account":"cryptolions"}' -p cfund.proton@active
#./cleos.sh push action cfund.proton activate '{"account":"cryptolions", "status": 0}' -p cfund.proton@active
#./cleos.sh push action cfund.proton activate '{"account":"cryptolions", "status": 1}' -p cfund.proton@active
#./cleos.sh push action cfund.proton process '{}' -p cfund.proton@active

### Users Actions
#./cleos.sh push action cfund.proton claimreward '{"account":"cryptolions"}' -p cryptolions@active


### Tables
#./cleos.sh get table cfund.proton cfund.proton users
#./cleos.sh get table cfund.proton cfund.proton global