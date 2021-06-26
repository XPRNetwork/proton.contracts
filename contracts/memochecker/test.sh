## Contract which reject transfers with not whitelisted memos.
## Allowed memos are stored in contract table so can be added and removed.

## Usage
##  - stake.proton

##


##-----------------------------------------------

### Deploy
#./cleos.sh set contract stake.proton ./build/memochecker -p stake.proton

### Add memo
#./cleos.sh push action stake.proton addmemo '{"memo":"Stake"}' -p stake.proton
#./cleos.sh push action stake.proton addmemo '{"memo":"Unstake"}' -p stake.proton
#./cleos.sh push action stake.proton addmemo '{"memo":"stake bandwidth"}' -p stake.proton
#./cleos.sh push action stake.proton addmemo '{"memo":"unstake XPR"}' -p stake.proton


### Del memo
#./cleos.sh push action stake.proton delmemo '{"memo":"Stake"}' -p stake.proton
#./cleos.sh push action stake.proton delmemo '{"memo":"Unstake"}' -p stake.proton
#./cleos.sh push action stake.proton delmemo '{"memo":"stake bandwidth"}' -p stake.proton
#./cleos.sh push action stake.proton delmemo '{"memo":"unstake XPR"}' -p stake.proton


### Del by ID
#./cleos.sh push action stake.proton delid '{"id":1}' -p stake.proton





