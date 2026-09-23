#include <eosio.wrap/eosio.wrap.hpp>

namespace eosio {

void wrap::exec( ignore<name>, ignore<transaction> ) {
   require_auth( get_self() );

   name executer;
   _ds >> executer;

   require_auth( executer );

   // The wrapped actions are dispatched inline rather than through send_deferred().
   // This chain runs Leap 5.0.3, which does not execute deferred transactions: the
   // scheduled transaction was created and then discarded at expiry without ever
   // running, so `exec` reported success while having no effect. eosio.wrap is a
   // privileged account, so inline authorization is not checked and the wrapped
   // actions still execute with the authorizations they declare -- which is the
   // superuser capability this contract exists to provide. Same approach as
   // eosio.msig::exec.
   //
   // Consequences of dispatching inline: the wrapped actions share the calling
   // transaction's CPU/NET budget instead of receiving their own, and a failure now
   // reverts `exec` and surfaces an error instead of disappearing silently.
   transaction_header  trx_header;
   std::vector<action> context_free_actions;
   std::vector<action> actions;

   _ds >> trx_header;
   check( trx_header.expiration >= eosio::time_point_sec(current_time_point()),
          "transaction expired" );
   check( trx_header.delay_sec.value == 0,
          "`delay_sec` is not supported; the wrapped actions are executed inline" );
   _ds >> context_free_actions;
   check( context_free_actions.empty(),
          "not allowed to `exec` a transaction with context-free actions" );
   _ds >> actions;

   for( const auto& act : actions ) {
      act.send();
   }
}

} /// namespace eosio
