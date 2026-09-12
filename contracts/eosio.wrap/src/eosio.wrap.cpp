#include <eosio.wrap/eosio.wrap.hpp>

namespace eosio {

void wrap::exec( ignore<name>, ignore<transaction> ) {
   require_auth( get_self() );

   name executer;
   _ds >> executer;

   require_auth( executer );

   // Execute the wrapped transaction inline rather than scheduling it as a
   // deferred transaction. Producers running Leap 5+/Spring no longer process
   // deferred transactions, so the previous send_deferred() approach silently
   // never executed the wrapped transaction. This mirrors the AntelopeIO
   // reference-contracts implementation: reject context-free actions and send
   // each regular action inline (eosio.wrap remains privileged, so the inline
   // actions bypass regular authorization checks exactly as before).
   transaction_header trx_header;
   std::vector<action> context_free_actions;
   std::vector<action> actions;
   _ds >> trx_header;
   _ds >> context_free_actions;
   check( context_free_actions.empty(), "not allowed to `exec` a transaction with context-free actions" );
   _ds >> actions;

   for ( const auto& act : actions ) {
      act.send();
   }
}

} /// namespace eosio
