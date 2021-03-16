#include <eosio/datastream.hpp>
#include <eosio/eosio.hpp>
#include <eosio/multi_index.hpp>
#include <eosio/privileged.hpp>
#include <eosio/serialize.hpp>
#include <eosio/transaction.hpp>

#include <eosio.system/eosio.system.hpp>
#include <eosio.token/eosio.token.hpp>

#include "name_bidding.cpp"
// Unfortunately, this is needed until CDT fixes the duplicate symbol error with eosio::send_deferred

namespace eosiosystem {

   using eosio::asset;
   using eosio::const_mem_fun;
   using eosio::current_time_point;
   using eosio::indexed_by;
   using eosio::permission_level;
   using eosio::seconds;
   using eosio::time_point_sec;
   using eosio::token;


   // PROTON RAM
   void system_contract::buyrambytes( const name& payer, const name& receiver, uint32_t bytes ) {  
       check(!(bytes == 0), "parameter bytes cannot be zero");
       
       const uint64_t cost = _gstateram.ram_price_per_byte.amount * bytes;

       const double fee_percentage = ((double)_gstateram.ram_fee_percent / ram_fee_precision) / 100;

       const int64_t costwfee = cost / (1.0 - fee_percentage);

       buyram(payer, receiver, asset{ costwfee, _gstateram.ram_price_per_byte.symbol });
   }

   // PROTON RAM
   void system_contract::buyram( const name& payer, const name& receiver, const asset& quant ){
      require_auth(payer);

      check(!(_gstateram.ram_price_per_byte.amount <= 0), "Wrong price per byte. Buy RAM is not configured");
      check(quant.symbol == _gstateram.ram_price_per_byte.symbol, "must buy ram with " + _gstateram.ram_price_per_byte.symbol.code().to_string());
      check(quant.amount > 0, "must purchase a positive amount");    
      update_ram_supply();

      auto fee = quant;
      fee.amount *= ((double)_gstateram.ram_fee_percent / (double)ram_fee_precision) / 100;

      auto quant_after_fee = quant;
      quant_after_fee.amount -= fee.amount;
      {
         token::transfer_action transfer_act{ token_account, { {payer, active_permission}, {ram_account, active_permission} } };
         transfer_act.send( payer, ram_account, quant_after_fee, "buy ram" );
      }
      if ( fee.amount > 0 ) {
         token::transfer_action transfer_act{ token_account, { {payer, active_permission} } };
         transfer_act.send( payer, ramfee_account, fee, "ram fee" );
      }

      const uint64_t bytes_out = quant_after_fee.amount / _gstateram.ram_price_per_byte.amount;

      const auto& market = _rammarket.get(ramcore_symbol.raw(), "ram market does not exist");

      check ( market.base.balance.amount >= bytes_out, "Out of RAM" );
  
      _rammarket.modify( market, same_payer, [&]( auto& es ) {
         es.base.balance  -= asset(bytes_out, ram_symbol);
      });

      check( bytes_out > 0, "must reserve a positive amount" );

      _gstate.total_ram_bytes_reserved += uint64_t(bytes_out);
      _gstateram.total_ram             += bytes_out;
      _gstateram.total_xpr             += quant_after_fee.amount;

      user_resources_table  userres( get_self(), receiver.value );  
      auto res_itr = userres.find( receiver.value );
      if( res_itr ==  userres.end() ) {
         res_itr = userres.emplace( get_self(), [&]( auto& res ) {
               res.owner = receiver;
               res.net_weight = asset( 0, core_symbol() );
               res.cpu_weight = asset( 0, core_symbol() );
               res.ram_bytes = bytes_out;
            });
      } else {
         userres.modify( res_itr, receiver, [&]( auto& res ) {
               res.ram_bytes += bytes_out;
            });
      }

      userram_table userram( get_self(), receiver.value );
      auto resram_itr = userram.find( receiver.value );
      if( resram_itr ==  userram.end() ) {
         resram_itr = userram.emplace( get_self(), [&]( auto& res ) {
               res.account = receiver;
               res.ram = bytes_out;
               res.quantity = quant_after_fee;
               res.ramlimit = 0;
            });
      } else {
         userram.modify( resram_itr, get_self(), [&]( auto& res ) {
               res.ram += bytes_out;
               res.quantity += quant_after_fee;
            });
      }

      check(!(res_itr->ram_bytes > _gstateram.max_per_user_bytes + resram_itr->ramlimit),
           "Maximum allowed to buy: " + to_string(_gstateram.max_per_user_bytes + resram_itr->ramlimit) + " bytes");


      auto voter_itr = _voters.find( res_itr->owner.value );
      if( voter_itr == _voters.end() || !has_field( voter_itr->flags1, voter_info::flags1_fields::ram_managed ) ) {
         int64_t ram_bytes, net, cpu;
         get_resource_limits( res_itr->owner, ram_bytes, net, cpu );
         set_resource_limits( res_itr->owner, res_itr->ram_bytes + ram_gift_bytes, net, cpu );
      }
   }


   // PROTON RAM
   void system_contract::sellram( const name& account, uint64_t bytes ) {
      require_auth( account );
      update_ram_supply();

      check(!(bytes == 0), "parameter bytes cannot be zero");

      user_resources_table  userres( get_self(), account.value );
      auto res_itr = userres.find( account.value );
      check( res_itr != userres.end(), "no resource row" );
      check( res_itr->ram_bytes >= bytes, "insufficient quota" );

      userram_table userram( get_self(), account.value );
      auto resram_itr = userram.find( account.value );
      check( resram_itr != userram.end(), "no purchased ram" );
      check( resram_itr->ram >= bytes, "insufficient purchased quota" );
      
      auto tokens_out = asset(0, _gstateram.ram_price_per_byte.symbol);

      tokens_out.amount = bytes * resram_itr->quantity.amount / resram_itr->ram;

      auto itr = _rammarket.find(ramcore_symbol.raw());
      _rammarket.modify( itr, same_payer, [&]( auto& es ) {
          es.base.balance  += asset(bytes, ram_symbol);
      });

      check( tokens_out.amount > 1, "token amount received from selling ram is too low" );

      _gstate.total_ram_bytes_reserved -= static_cast<decltype(_gstate.total_ram_bytes_reserved)>(bytes); // bytes > 0 is asserted above
      _gstateram.total_ram             -= bytes;
      _gstateram.total_xpr             -= tokens_out.amount;
      //// this shouldn't happen, but just in case it does we should prevent it
      check( _gstateram.total_ram >= 0, "error, attempt to unstake more tokens than previously staked" );

      userres.modify( res_itr, account, [&]( auto& res ) {
          res.ram_bytes -= bytes;
      });

      auto voter_itr = _voters.find( res_itr->owner.value );
      if( voter_itr == _voters.end() || !has_field( voter_itr->flags1, voter_info::flags1_fields::ram_managed ) ) {
         int64_t ram_bytes, net, cpu;
         get_resource_limits( res_itr->owner, ram_bytes, net, cpu );
         set_resource_limits( res_itr->owner, res_itr->ram_bytes + ram_gift_bytes, net, cpu );
      }

      userram.modify( resram_itr, account, [&]( auto& res ) {
          res.ram -= bytes;
          res.quantity -= tokens_out;
      });

      {
         token::transfer_action transfer_act{ token_account, { {ram_account, active_permission}, {account, active_permission} } };
         transfer_act.send( ram_account, account, asset(tokens_out), "sell ram" );
      }
   }


   
   /**
    *  This action will buy an exact amount of ram and bill the payer the current market price.
    */
   void system_contract::buyrambsys( const name& payer, const name& receiver, uint32_t bytes ) {
      auto itr = _rammarket.find(ramcore_symbol.raw());
      const int64_t ram_reserve   = itr->base.balance.amount;
      const int64_t eos_reserve   = itr->quote.balance.amount;
      const int64_t cost          = exchange_state::get_bancor_input( ram_reserve, eos_reserve, bytes );
      const int64_t cost_plus_fee = cost / double(0.995);
      buyramsys( payer, receiver, asset{ cost_plus_fee, core_symbol() } );
   }


   /**
    *  When buying ram the payer irreversiblly transfers quant to system contract and only
    *  the receiver may reclaim the tokens via the sellram action. The receiver pays for the
    *  storage of all database records associated with this action.
    *
    *  RAM is a scarce resource whose supply is defined by global properties max_ram_size. RAM is
    *  priced using the bancor algorithm such that price-per-byte with a constant reserve ratio of 100:1.
    */
   void system_contract::buyramsys( const name& payer, const name& receiver, const asset& quant )
   {
      require_auth( payer );

      check (checkPermission(payer, "buyram")==1, "You are not authorised to buyram.");  // PROTON Check Permissions

      update_ram_supply();

      check( quant.symbol == core_symbol(), "must buy ram with core token" );
      check( quant.amount > 0, "must purchase a positive amount" );

      auto fee = quant;
      fee.amount = ( fee.amount + 199 ) / 200; /// .5% fee (round up)
      // fee.amount cannot be 0 since that is only possible if quant.amount is 0 which is not allowed by the assert above.
      // If quant.amount == 1, then fee.amount == 1,
      // otherwise if quant.amount > 1, then 0 < fee.amount < quant.amount.
      auto quant_after_fee = quant;
      quant_after_fee.amount -= fee.amount;
      // quant_after_fee.amount should be > 0 if quant.amount > 1.
      // If quant.amount == 1, then quant_after_fee.amount == 0 and the next inline transfer will fail causing the buyram action to fail.
      {
         token::transfer_action transfer_act{ token_account, { {payer, active_permission}, {ram_account, active_permission} } };
         transfer_act.send( payer, ram_account, quant_after_fee, "buy ram" );
      }
      if ( fee.amount > 0 ) {
         token::transfer_action transfer_act{ token_account, { {payer, active_permission} } };
         transfer_act.send( payer, ramfee_account, fee, "ram fee" );
         channel_to_rex( ramfee_account, fee );
      }

      int64_t bytes_out = 0;

      const auto& market = _rammarket.get(ramcore_symbol.raw(), "ram market does not exist");
      _rammarket.modify( market, same_payer, [&]( auto& es ) {
         bytes_out = es.direct_convert( quant_after_fee,  ram_symbol ).amount;
      });

      check( bytes_out > 0, "must reserve a positive amount" );

      _gstate.total_ram_bytes_reserved += uint64_t(bytes_out);
      _gstate.total_ram_stake          += quant_after_fee.amount;

      user_resources_table  userres( get_self(), receiver.value );
      auto res_itr = userres.find( receiver.value );
      if( res_itr ==  userres.end() ) {
         res_itr = userres.emplace( receiver, [&]( auto& res ) {
               res.owner = receiver;
               res.net_weight = asset( 0, core_symbol() );
               res.cpu_weight = asset( 0, core_symbol() );
               res.ram_bytes = bytes_out;
            });
      } else {
         userres.modify( res_itr, receiver, [&]( auto& res ) {
               res.ram_bytes += bytes_out;
            });
      }

      auto voter_itr = _voters.find( res_itr->owner.value );
      if( voter_itr == _voters.end() || !has_field( voter_itr->flags1, voter_info::flags1_fields::ram_managed ) ) {
         int64_t ram_bytes, net, cpu;
         get_resource_limits( res_itr->owner, ram_bytes, net, cpu );
         set_resource_limits( res_itr->owner, res_itr->ram_bytes + ram_gift_bytes, net, cpu );
      }
   }

  /**
    *  The system contract now buys and sells RAM allocations at prevailing market prices.
    *  This may result in traders buying RAM today in anticipation of potential shortages
    *  tomorrow. Overall this will result in the market balancing the supply and demand
    *  for RAM over time.
    */
   void system_contract::sellramsys( const name& account, int64_t bytes ) {
      require_auth( account );

      check (system_contract::checkPermission(account, "sellram")==1, "You are not authorised to sellram.");  // PROTON Check Permissions

      update_ram_supply();

      check( bytes > 0, "cannot sell negative byte" );

      user_resources_table  userres( get_self(), account.value );
      auto res_itr = userres.find( account.value );
      check( res_itr != userres.end(), "no resource row" );
      check( res_itr->ram_bytes >= bytes, "insufficient quota" );

      asset tokens_out;
      auto itr = _rammarket.find(ramcore_symbol.raw());
      _rammarket.modify( itr, same_payer, [&]( auto& es ) {
         /// the cast to int64_t of bytes is safe because we certify bytes is <= quota which is limited by prior purchases
         tokens_out = es.direct_convert( asset(bytes, ram_symbol), core_symbol());
      });

      check( tokens_out.amount > 1, "token amount received from selling ram is too low" );

      _gstate.total_ram_bytes_reserved -= static_cast<decltype(_gstate.total_ram_bytes_reserved)>(bytes); // bytes > 0 is asserted above
      _gstate.total_ram_stake          -= tokens_out.amount;

      //// this shouldn't happen, but just in case it does we should prevent it
      check( _gstate.total_ram_stake >= 0, "error, attempt to unstake more tokens than previously staked" );

      userres.modify( res_itr, account, [&]( auto& res ) {
          res.ram_bytes -= bytes;
      });

      auto voter_itr = _voters.find( res_itr->owner.value );
      if( voter_itr == _voters.end() || !has_field( voter_itr->flags1, voter_info::flags1_fields::ram_managed ) ) {
         int64_t ram_bytes, net, cpu;
         get_resource_limits( res_itr->owner, ram_bytes, net, cpu );
         set_resource_limits( res_itr->owner, res_itr->ram_bytes + ram_gift_bytes, net, cpu );
      }

      {
         token::transfer_action transfer_act{ token_account, { {ram_account, active_permission}, {account, active_permission} } };
         transfer_act.send( ram_account, account, asset(tokens_out), "sell ram" );
      }
      auto fee = ( tokens_out.amount + 199 ) / 200; /// .5% fee (round up)
      // since tokens_out.amount was asserted to be at least 2 earlier, fee.amount < tokens_out.amount
      if ( fee > 0 ) {
         token::transfer_action transfer_act{ token_account, { {account, active_permission} } };
         transfer_act.send( account, ramfee_account, asset(fee, core_symbol()), "sell ram fee" );
         channel_to_rex( ramfee_account, asset(fee, core_symbol() ));
      }
   }

   void validate_b1_vesting( int64_t stake ) {
      const int64_t base_time = 1527811200; /// 2018-06-01
      const int64_t max_claimable = 100'000'000'0000ll;
      const int64_t claimable = int64_t(max_claimable * double(current_time_point().sec_since_epoch() - base_time) / (10*seconds_per_year) );

      check( max_claimable - claimable <= stake, "b1 can only claim their tokens over 10 years" );
   }

   void system_contract::changebw( name from, const name& receiver,
                                   const asset& stake_net_delta, const asset& stake_cpu_delta, bool transfer )
   {
      require_auth( from );
      check( stake_net_delta.amount != 0 || stake_cpu_delta.amount != 0, "should stake non-zero amount" );
      check( std::abs( (stake_net_delta + stake_cpu_delta).amount )
             >= std::max( std::abs( stake_net_delta.amount ), std::abs( stake_cpu_delta.amount ) ),
             "net and cpu deltas cannot be opposite signs" );

      name source_stake_from = from;
      if ( transfer ) {
         from = receiver;
      }

      // update stake delegated from "from" to "receiver"
      {
         del_bandwidth_table     del_tbl( get_self(), from.value );
         auto itr = del_tbl.find( receiver.value );
         if( itr == del_tbl.end() ) {
            itr = del_tbl.emplace( from, [&]( auto& dbo ){
                  dbo.from          = from;
                  dbo.to            = receiver;
                  dbo.net_weight    = stake_net_delta;
                  dbo.cpu_weight    = stake_cpu_delta;
               });
         }
         else {
            del_tbl.modify( itr, same_payer, [&]( auto& dbo ){
                  dbo.net_weight    += stake_net_delta;
                  dbo.cpu_weight    += stake_cpu_delta;
               });
         }
         check( 0 <= itr->net_weight.amount, "insufficient staked net bandwidth" );
         check( 0 <= itr->cpu_weight.amount, "insufficient staked cpu bandwidth" );
         if ( itr->is_empty() ) {
            del_tbl.erase( itr );
         }
      } // itr can be invalid, should go out of scope

      // update totals of "receiver"
      {
         user_resources_table   totals_tbl( get_self(), receiver.value );
         auto tot_itr = totals_tbl.find( receiver.value );
         if( tot_itr ==  totals_tbl.end() ) {
            tot_itr = totals_tbl.emplace( from, [&]( auto& tot ) {
                  tot.owner = receiver;
                  tot.net_weight    = stake_net_delta;
                  tot.cpu_weight    = stake_cpu_delta;
               });
         } else {
            totals_tbl.modify( tot_itr, from == receiver ? from : same_payer, [&]( auto& tot ) {
                  tot.net_weight    += stake_net_delta;
                  tot.cpu_weight    += stake_cpu_delta;
               });
         }
         check( 0 <= tot_itr->net_weight.amount, "insufficient staked total net bandwidth" );
         check( 0 <= tot_itr->cpu_weight.amount, "insufficient staked total cpu bandwidth" );

         {
            bool ram_managed = false;
            bool net_managed = false;
            bool cpu_managed = false;

            auto voter_itr = _voters.find( receiver.value );
            if( voter_itr != _voters.end() ) {
               ram_managed = has_field( voter_itr->flags1, voter_info::flags1_fields::ram_managed );
               net_managed = has_field( voter_itr->flags1, voter_info::flags1_fields::net_managed );
               cpu_managed = has_field( voter_itr->flags1, voter_info::flags1_fields::cpu_managed );
            }

            if( !(net_managed && cpu_managed) ) {
               int64_t ram_bytes, net, cpu;
               get_resource_limits( receiver, ram_bytes, net, cpu );

               set_resource_limits( receiver,
                                    ram_managed ? ram_bytes : std::max( tot_itr->ram_bytes + ram_gift_bytes, ram_bytes ),
                                    net_managed ? net : tot_itr->net_weight.amount,
                                    cpu_managed ? cpu : tot_itr->cpu_weight.amount );
            }
         }

         if ( tot_itr->is_empty() ) {
            totals_tbl.erase( tot_itr );
         }
      } // tot_itr can be invalid, should go out of scope

      // create refund or update from existing refund
      if ( stake_account != source_stake_from ) { //for eosio both transfer and refund make no sense
         refunds_table refunds_tbl( get_self(), from.value );
         auto req = refunds_tbl.find( from.value );

         //create/update/delete refund
         auto net_balance = stake_net_delta;
         auto cpu_balance = stake_cpu_delta;
         bool need_deferred_trx = false;


         // net and cpu are same sign by assertions in delegatebw and undelegatebw
         // redundant assertion also at start of changebw to protect against misuse of changebw
         bool is_undelegating = (net_balance.amount + cpu_balance.amount ) < 0;
         bool is_delegating_to_self = (!transfer && from == receiver);

         if( is_delegating_to_self || is_undelegating ) {
            if ( req != refunds_tbl.end() ) { //need to update refund
               refunds_tbl.modify( req, same_payer, [&]( refund_request& r ) {
                  if ( net_balance.amount < 0 || cpu_balance.amount < 0 ) {
                     r.request_time = current_time_point();
                  }
                  r.net_amount -= net_balance;
                  if ( r.net_amount.amount < 0 ) {
                     net_balance = -r.net_amount;
                     r.net_amount.amount = 0;
                  } else {
                     net_balance.amount = 0;
                  }
                  r.cpu_amount -= cpu_balance;
                  if ( r.cpu_amount.amount < 0 ){
                     cpu_balance = -r.cpu_amount;
                     r.cpu_amount.amount = 0;
                  } else {
                     cpu_balance.amount = 0;
                  }
               });

               check( 0 <= req->net_amount.amount, "negative net refund amount" ); //should never happen
               check( 0 <= req->cpu_amount.amount, "negative cpu refund amount" ); //should never happen

               if ( req->is_empty() ) {
                  refunds_tbl.erase( req );
                  need_deferred_trx = false;
               } else {
                  need_deferred_trx = true;
               }
            } else if ( net_balance.amount < 0 || cpu_balance.amount < 0 ) { //need to create refund
               refunds_tbl.emplace( from, [&]( refund_request& r ) {
                  r.owner = from;
                  if ( net_balance.amount < 0 ) {
                     r.net_amount = -net_balance;
                     net_balance.amount = 0;
                  } else {
                     r.net_amount = asset( 0, core_symbol() );
                  }
                  if ( cpu_balance.amount < 0 ) {
                     r.cpu_amount = -cpu_balance;
                     cpu_balance.amount = 0;
                  } else {
                     r.cpu_amount = asset( 0, core_symbol() );
                  }
                  r.request_time = current_time_point();
               });
               need_deferred_trx = true;
            } // else stake increase requested with no existing row in refunds_tbl -> nothing to do with refunds_tbl
         } /// end if is_delegating_to_self || is_undelegating

         if ( need_deferred_trx ) {
            // PROTON (from deferred to inline)
            action(
               permission_level{ from, active_permission },
               get_self(),
               "refund"_n,
               from
            ).send();
            /*
            eosio::transaction out;
            out.actions.emplace_back( permission_level{from, active_permission},
                                      get_self(), "refund"_n,
                                      from
            );
            out.delay_sec = refund_delay_sec;
            eosio::cancel_deferred( from.value ); // TODO: Remove this line when replacing deferred trxs is fixed
            out.send( from.value, from, true );
         */
         } else {
            //eosio::cancel_deferred( from.value );
         }

         auto transfer_amount = net_balance + cpu_balance;
         if ( 0 < transfer_amount.amount ) {
            token::transfer_action transfer_act{ token_account, { {source_stake_from, active_permission} } };
            transfer_act.send( source_stake_from, stake_account, asset(transfer_amount), "stake bandwidth" );
         }
      }

      vote_stake_updater( from );
      update_voting_power( from, stake_net_delta + stake_cpu_delta );   // PROTON !!!! Remove after voting system will be switched only to XPR
   }


   void system_contract::update_voting_power( const name& voter, const asset& total_update )
   {
      auto voter_itr = _voters.find( voter.value );
      if( voter_itr == _voters.end() ) {
         voter_itr = _voters.emplace( voter, [&]( auto& v ) {
            v.owner  = voter;
            v.staked = total_update.amount;
         });
      } else {
         _voters.modify( voter_itr, same_payer, [&]( auto& v ) {
            v.staked += total_update.amount;
         });
      }

      check( 0 <= voter_itr->staked, "stake for voting cannot be negative" );

      if( voter == "b1"_n ) {
         validate_b1_vesting( voter_itr->staked );
      }

      if( voter_itr->producers.size() || voter_itr->proxy ) {
         update_votes( voter, voter_itr->proxy, voter_itr->producers, false );
      }
   }

   void system_contract::delegatebw( const name& from, const name& receiver,
                                     const asset& stake_net_quantity,
                                     const asset& stake_cpu_quantity, bool transfer )
   {
      asset zero_asset( 0, core_symbol() );
      check( stake_cpu_quantity >= zero_asset, "must stake a positive amount" );
      check( stake_net_quantity >= zero_asset, "must stake a positive amount" );
      check( stake_net_quantity.amount + stake_cpu_quantity.amount > 0, "must stake a positive amount" );
      check( !transfer || from != receiver, "cannot use transfer flag if delegating to self" );

      check (system_contract::checkPermission(from, "delegate")==1, "You are not authorised to delegate.");  // PROTON Check Permissions


      changebw( from, receiver, stake_net_quantity, stake_cpu_quantity, transfer);
   } // delegatebw

   void system_contract::undelegatebw( const name& from, const name& receiver,
                                       const asset& unstake_net_quantity, const asset& unstake_cpu_quantity )
   {
      asset zero_asset( 0, core_symbol() );
      check( unstake_cpu_quantity >= zero_asset, "must unstake a positive amount" );
      check( unstake_net_quantity >= zero_asset, "must unstake a positive amount" );
      check( unstake_cpu_quantity.amount + unstake_net_quantity.amount > 0, "must unstake a positive amount" );
      check( _gstate.thresh_activated_stake_time != time_point(),
             "cannot undelegate bandwidth until the chain is activated (at least 15% of all tokens participate in voting)" );

      check (system_contract::checkPermission(from, "undelegate")==1, "You are not authorised to undelegate.");  // PROTON Check Permissions


      changebw( from, receiver, -unstake_net_quantity, -unstake_cpu_quantity, false);
   } // undelegatebw


   void system_contract::refund( const name& owner ) {
      require_auth( owner );

      refunds_table refunds_tbl( get_self(), owner.value );
      auto req = refunds_tbl.find( owner.value );
      check( req != refunds_tbl.end(), "refund request not found" );
      check( req->request_time + seconds(refund_delay_sec) <= current_time_point(),
             "refund is not available yet" );
      token::transfer_action transfer_act{ token_account, { {stake_account, active_permission}, {req->owner, active_permission} } };
      transfer_act.send( stake_account, req->owner, req->net_amount + req->cpu_amount, "unstake" );
      refunds_tbl.erase( req );
   }
   
   
   /* PROTON VOTING LOGIC */
   
   void system_contract::stakexpr ( const name& from, const name& receiver, const asset& stake_xpr_quantity){     
      check( stake_xpr_quantity >= asset ( 0, XPRsym ), "must stake a positive amount" );      
      check( stake_xpr_quantity.amount > 0, "must stake a positive amount" );

      updstakexpr( from, receiver, stake_xpr_quantity);
   }
   
   void system_contract::unstakexpr ( const name& from, const name& receiver, const asset& unstake_xpr_quantity ){
      
      check( unstake_xpr_quantity >= asset ( 0, XPRsym ), "must unstake a positive amount" );
      check( unstake_xpr_quantity.amount > 0, "must unstake a positive amount" );

      updstakexpr( from, receiver, -unstake_xpr_quantity);
   }

   void system_contract::updstakexpr( name from, const name& receiver, const asset& xpr_quantity )
   {
      require_auth( from );
      check( xpr_quantity.amount != 0, "should stake non-zero amount" );
      
      check ( from == receiver, "At this point you can only stake to yourself" ); 

      // update stake delegated from "from" to "receiver"
      asset old_balance = asset(0, XPRsym);
      
      {
         del_xpr_table     sxpr_tbl( get_self(), from.value );
         auto itr = sxpr_tbl.find( receiver.value );

         if ( itr == sxpr_tbl.end() ) {
            itr = sxpr_tbl.emplace( from, [&]( auto& dbo ){
                  dbo.from          = from;
                  dbo.to            = receiver;
                  dbo.quantity      = xpr_quantity;
            });
         } else {
            old_balance = itr->quantity;
            
            sxpr_tbl.modify( itr, same_payer, [&]( auto& dbo ){
               dbo.quantity    += xpr_quantity;
            });
         }

         check( 0 <= itr->quantity.amount, "insufficient staked XPR tokens. You tried to unstake " + (-xpr_quantity).to_string() + " but available amount is " + old_balance.to_string());
         
         if ( itr->is_empty() ) {
            sxpr_tbl.erase( itr );
         }
      } 

      if ( xpr_stake_account != from ) { //for eosio both transfer and refund make no sense
         xpr_refunds_table xpr_refunds_tbl( get_self(), from.value );
         auto req = xpr_refunds_tbl.find( from.value );

         //create/update/delete refund
         auto xpr_balance = xpr_quantity;
 
         auto need_deferred_trx = false;

         const auto is_undelegating = xpr_balance.amount  < 0;
         const auto is_delegating_to_self = (from == receiver);

         if( is_delegating_to_self || is_undelegating ) {
            if ( req != xpr_refunds_tbl.end() ) { //need to update refund
               xpr_refunds_tbl.modify( req, same_payer, [&]( xpr_refund_request& r ) {
                  if ( xpr_balance.amount < 0 ) {
                     r.request_time = current_time_point();
                  }

                  r.quantity -= xpr_balance;
                  if ( r.quantity.amount < 0 ) {
                     xpr_balance = -r.quantity;
                     r.quantity.amount = 0;
                  } else {
                     xpr_balance.amount = 0;
                  }
                  
               });

               check( 0 <= req->quantity.amount, "negative refund amount" ); //should never happen

               if ( req->is_empty() ) {
                  xpr_refunds_tbl.erase( req );
               } else {
                  need_deferred_trx = true;
               }
            } else if ( xpr_balance.amount < 0 ) { //need to create refund
               xpr_refunds_tbl.emplace( from, [&]( xpr_refund_request& r ) {
                  r.owner = from;
                  if ( xpr_balance.amount < 0 ) {
                     r.quantity = -xpr_balance;
                     xpr_balance.amount = 0;
                  } else {
                     r.quantity = asset( 0, XPRsym );
                  }
                  r.request_time = current_time_point();
               });
               need_deferred_trx = true;
            }
         } 

         if ( need_deferred_trx ) {
            eosio::transaction out;
            out.actions.emplace_back( permission_level{from, active_permission},
                                      get_self(), "refundxpr"_n,
                                      from
            );
            out.delay_sec = _gstatesxpr.unstake_period; //refund_delay_sec;
            eosio::cancel_deferred( from.value );       // TODO: Remove this line when replacing deferred trxs is fixed
            out.send( from.value, from, true );
         } else {
            eosio::cancel_deferred( from.value );
         }

         auto transfer_amount = xpr_balance;
         if ( 0 < transfer_amount.amount ) {
            token::transfer_action transfer_act{ token_account, { {from, active_permission} } };
            transfer_act.send( from, xpr_stake_account, asset(transfer_amount), "Stake" );
         }
      }

      update_xpr_voting_power( from, receiver, xpr_quantity );    // PROTON 
   }


   void system_contract::refundxpr( const name& owner ) {
      require_auth( owner );

      xpr_refunds_table xpr_refunds_tbl( get_self(), owner.value );
      auto req = xpr_refunds_tbl.require_find( owner.value, string("refund request not found").c_str() );
     
      check(req->request_time + seconds(_gstatesxpr.unstake_period) < current_time_point(), "refund is not available yet. Please wait " + timeToWait(abs((int)( (uint64_t)( (uint64_t)req->request_time.sec_since_epoch() + _gstatesxpr.unstake_period) - (uint64_t)current_time_point().sec_since_epoch() )) ));   
      token::transfer_action transfer_act{ token_account, { {xpr_stake_account, active_permission}, {req->owner, active_permission} } };
      transfer_act.send( xpr_stake_account, req->owner, req->quantity, "Unstake" );
      xpr_refunds_tbl.erase( req );
   }
   


   
   void system_contract::update_xpr_voting_power( const name& from, const name& voter, const asset& total_update )
   {
      auto voter_itr = _voters.find( voter.value );
      auto voter_info_itr = vxpr_tbl.find( from.value );

      auto               isqualified = false;
      optional<bool>     startqualif = std::nullopt;
      optional<uint64_t> startstake = std::nullopt;

      if( voter_itr == _voters.end() ) {
         voter_itr = _voters.emplace( voter, [&]( auto& v ) {
            v.owner  = voter;
            v.staked = total_update.amount;
         });
      } else {
         _voters.modify( voter_itr, same_payer, [&]( auto& v ) {
            v.staked += total_update.amount;
         });
      }

      if ( voter_info_itr == vxpr_tbl.end() ) {
         if ( voter_itr->producers.size() >= _gstatesxpr.min_bp_reward  && voter_itr->producers.size() <= _gstatesxpr.max_bp_per_vote ){
            isqualified = true;
         }
         if (_gstatesd.isprocessing && _gstatesd.processFrom.value < from.value) {
            startstake = 0;
            startqualif = false;
         }

         voter_info_itr = vxpr_tbl.emplace( from, [&]( auto& v ) {
            v.owner  = from;
            v.staked = total_update.amount;
            v.isqualified = isqualified;
            v.claimamount = 0;
            v.lastclaim = 0;
            v.startstake =  startstake;
            v.startqualif = startqualif;
         });
      } else {
         vxpr_tbl.modify( voter_info_itr, same_payer, [&]( auto& v ) {
            if (_gstatesd.isprocessing && _gstatesd.processFrom.value < from.value && v.startstake == std::nullopt) {
               v.startstake    = voter_info_itr->staked;
            }
            v.staked += total_update.amount;
         });
      }

      if ( voter_itr->producers.size() >= _gstatesxpr.min_bp_reward ) {
         _gstatesd.totalrstaked += total_update.amount;
      }

      check( 0 <= voter_itr->staked, "stake for voting cannot be negative" );

      _gstatesd.totalstaked += total_update.amount;

      if( voter_itr->producers.size() || voter_itr->proxy ) {
         update_xpr_votes( from, voter, voter_itr->proxy, voter_itr->producers, false );
      }
   }
   
   std::string system_contract::timeToWait( const uint64_t& time_in_seconds ){
      uint64_t s, h, m = 0;
      m = time_in_seconds / 60;
      h = m / 60;
      return std::to_string(int(h)) + " hours " + std::to_string(int(m % 60)) + " minutes " + std::to_string(int(time_in_seconds % 60)) + " seconds";
   }
   /* END PROTON VOTING LOGIC */
   
} //namespace eosiosystem
