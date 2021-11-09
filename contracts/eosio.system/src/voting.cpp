#include <eosio/crypto.hpp>
#include <eosio/datastream.hpp>
#include <eosio/eosio.hpp>
#include <eosio/multi_index.hpp>
#include <eosio/permission.hpp>
#include <eosio/privileged.hpp>
#include <eosio/serialize.hpp>
#include <eosio/singleton.hpp>

#include <eosio.system/eosio.system.hpp>
#include <eosio.token/eosio.token.hpp>

#include <type_traits>
#include <limits>
#include <set>
#include <algorithm>
#include <cmath>

namespace eosiosystem {

   using eosio::const_mem_fun;
   using eosio::current_time_point;
   using eosio::indexed_by;
   using eosio::microseconds;
   using eosio::singleton;

   void system_contract::register_producer( const name& producer, const eosio::block_signing_authority& producer_authority, const std::string& url, uint16_t location ) {
      auto prod = _producers.find( producer.value );
      const auto ct = current_time_point();

      eosio::public_key producer_key{};

      std::visit( [&](auto&& auth ) {
         if( auth.keys.size() == 1 ) {
            // if the producer_authority consists of a single key, use that key in the legacy producer_key field
            producer_key = auth.keys[0].key;
         }
      }, producer_authority );

      if ( prod != _producers.end() ) {
         _producers.modify( prod, producer, [&]( producer_info& info ){
            info.producer_key       = producer_key;
            info.is_active          = true;
            info.url                = url;
            info.location           = location;
            info.producer_authority.emplace( producer_authority );
            if ( info.last_claim_time == time_point() )
               info.last_claim_time = ct;
         });

         auto prod2 = _producers2.find( producer.value );
         if ( prod2 == _producers2.end() ) {
            _producers2.emplace( producer, [&]( producer_info2& info ){
               info.owner                     = producer;
               info.last_votepay_share_update = ct;
            });
            update_total_votepay_share( ct, 0.0, prod->total_votes );
            // When introducing the producer2 table row for the first time, the producer's votes must also be accounted for in the global total_producer_votepay_share at the same time.
         }
      } else {
         _producers.emplace( producer, [&]( producer_info& info ){
            info.owner              = producer;
            info.total_votes        = 0;
            info.producer_key       = producer_key;
            info.is_active          = true;
            info.url                = url;
            info.location           = location;
            info.last_claim_time    = ct;
            info.producer_authority.emplace( producer_authority );
         });
         _producers2.emplace( producer, [&]( producer_info2& info ){
            info.owner                     = producer;
            info.last_votepay_share_update = ct;
         });
      }

   }

   void system_contract::regproducer( const name& producer, const eosio::public_key& producer_key, const std::string& url, uint16_t location ) {
      require_auth( producer );

      // if not disabled after kickbp, allow regproducer
      check (checkPermission(producer, "regprod") == 1, "You are not authorised to register as producer");  // PROTON Check Permissions

      check( url.size() < 512, "url too long" );

      register_producer( producer, convert_to_block_signing_authority( producer_key ), url, location );
   }

   void system_contract::regproducer2( const name& producer, const eosio::block_signing_authority& producer_authority, const std::string& url, uint16_t location ) {
      require_auth( producer );
      check( url.size() < 512, "url too long" );

      //PROTON
      check (checkPermission(producer, "regprod") == 1, "You are not authorised to register as producer");  // PROTON Check Permissions
	   
      std::visit( [&](auto&& auth ) {
         check( auth.is_valid(), "invalid producer authority" );
      }, producer_authority );

      register_producer( producer, producer_authority, url, location );
   }

   void system_contract::unregprod( const name& producer ) {

      require_auth( producer );

      const auto& prod = _producers.get( producer.value, "producer not found" );
      _producers.modify( prod, same_payer, [&]( producer_info& info ){
         info.deactivate();
      });
   }


   // PROTON
   void system_contract::kickbp( const name& producer ) {
         require_auth(permission_level("eosio"_n, "committee"_n));

         const auto& prod = _producers.get( producer.value, "producer not found" );
         _producers.modify( prod, get_self(), [&]( producer_info& info ){
            info.deactivate();
         });

         auto act = action(
            permission_level{ get_self(), "active"_n },
            "eosio.proton"_n,
            "kickbp"_n,
            producer

         );
         act.send();
   }


   void system_contract::update_elected_producers( const block_timestamp& block_time ) {
      _gstate.last_producer_schedule_update = block_time;

      auto idx = _producers.get_index<"prototalvote"_n>();

      using value_type = std::pair<eosio::producer_authority, uint16_t>;
      std::vector< value_type > top_producers;
      top_producers.reserve(21);

      for( auto it = idx.cbegin(); it != idx.cend() && top_producers.size() < 21 && 0 < it->total_votes && it->active(); ++it ) {
         top_producers.emplace_back(
            eosio::producer_authority{
               .producer_name = it->owner,
               .authority     = it->get_producer_authority()
            },
            it->location
         );
      }

      if( top_producers.size() == 0 || top_producers.size() < _gstate.last_producer_schedule_size ) {
         return;
      }

      std::sort( top_producers.begin(), top_producers.end(), []( const value_type& lhs, const value_type& rhs ) {
         return lhs.first.producer_name < rhs.first.producer_name; // sort by producer name
         // return lhs.second < rhs.second; // sort by location
      } );

      std::vector<eosio::producer_authority> producers;

      producers.reserve(top_producers.size());
      for( auto& item : top_producers )
         producers.push_back( std::move(item.first) );

      if( set_proposed_producers( producers ) >= 0 ) {
         _gstate.last_producer_schedule_size = static_cast<decltype(_gstate.last_producer_schedule_size)>( top_producers.size() );
      }
   }

   double stake2vote( int64_t staked ) {
      /// TODO subtract 2080 brings the large numbers closer to this decade
      double weight = int64_t( (current_time_point().sec_since_epoch() - (block_timestamp::block_timestamp_epoch / 1000)) / (seconds_per_day * 7) )  / double( 52 );
      return double(staked) * std::pow( 2, weight );
   }

   double system_contract::update_total_votepay_share( const time_point& ct,
                                                       double additional_shares_delta,
                                                       double shares_rate_delta )
   {
      double delta_total_votepay_share = 0.0;
      if( ct > _gstate3.last_vpay_state_update ) {
         delta_total_votepay_share = _gstate3.total_vpay_share_change_rate
                                       * double( (ct - _gstate3.last_vpay_state_update).count() / 1E6 );
      }

      delta_total_votepay_share += additional_shares_delta;
      if( delta_total_votepay_share < 0 && _gstate2.total_producer_votepay_share < -delta_total_votepay_share ) {
         _gstate2.total_producer_votepay_share = 0.0;
      } else {
         _gstate2.total_producer_votepay_share += delta_total_votepay_share;
      }

      if( shares_rate_delta < 0 && _gstate3.total_vpay_share_change_rate < -shares_rate_delta ) {
         _gstate3.total_vpay_share_change_rate = 0.0;
      } else {
         _gstate3.total_vpay_share_change_rate += shares_rate_delta;
      }

      _gstate3.last_vpay_state_update = ct;

      return _gstate2.total_producer_votepay_share;
   }

   double system_contract::update_producer_votepay_share( const producers_table2::const_iterator& prod_itr,
                                                          const time_point& ct,
                                                          double shares_rate,
                                                          bool reset_to_zero )
   {
      double delta_votepay_share = 0.0;
      if( shares_rate > 0.0 && ct > prod_itr->last_votepay_share_update ) {
         delta_votepay_share = shares_rate * double( (ct - prod_itr->last_votepay_share_update).count() / 1E6 ); // cannot be negative
      }

      double new_votepay_share = prod_itr->votepay_share + delta_votepay_share;
      _producers2.modify( prod_itr, same_payer, [&](auto& p) {
         if( reset_to_zero )
            p.votepay_share = 0.0;
         else
            p.votepay_share = new_votepay_share;

         p.last_votepay_share_update = ct;
      } );

      return new_votepay_share;
   }

   // PROTON. action changed to work with XPR tokens. Original to vote with SYS ranamed to `voteprodsys`
   void system_contract::voteproducer( const name& voter_name, const name& proxy, const std::vector<name>& producers ) {
      require_auth( voter_name );

      update_xpr_votes( voter_name, voter_name, proxy, producers, true );
   }

   void system_contract::voteprodsys( const name& voter_name, const name& proxy, const std::vector<name>& producers ) {
      require_auth( voter_name );

      check (checkPermission(voter_name, "vote")==1, "You are not authorised to Vote");  // PROTON Check Permissions

      vote_stake_updater( voter_name );
      update_votes( voter_name, proxy, producers, true );
      auto rex_itr = _rexbalance.find( voter_name.value );
      if( rex_itr != _rexbalance.end() && rex_itr->rex_balance.amount > 0 ) {
         check_voting_requirement( voter_name, "voter holding REX tokens must vote for at least 21 producers or for a proxy" );
      }
   }

   void system_contract::update_votes( const name& voter_name, const name& proxy, const std::vector<name>& producers, bool voting ) {
      //validate input
      if ( proxy ) {
         check( producers.size() == 0, "cannot vote for producers and proxy at same time" );
         check( voter_name != proxy, "cannot proxy to self" );
      } else {
         check( producers.size() <= 30, "attempt to vote for too many producers" );
         for( size_t i = 1; i < producers.size(); ++i ) {
            check( producers[i-1] < producers[i], "producer votes must be unique and sorted" );
         }
      }

      auto voter = _voters.find( voter_name.value );
      check( voter != _voters.end(), "user must stake before they can vote" ); /// staking creates voter object
      check( !proxy || !voter->is_proxy, "account registered as a proxy is not allowed to use a proxy" );

      /**
       * The first time someone votes we calculate and set last_vote_weight. Since they cannot unstake until
       * after the chain has been activated, we can use last_vote_weight to determine that this is
       * their first vote and should consider their stake activated.
       */
      if( _gstate.thresh_activated_stake_time == time_point() && voter->last_vote_weight <= 0.0 ) {
         _gstate.total_activated_stake += voter->staked;
         if( _gstate.total_activated_stake >= min_activated_stake ) {
            _gstate.thresh_activated_stake_time = current_time_point();
         }
      }

      auto new_vote_weight = stake2vote( voter->staked );
      if( voter->is_proxy ) {
         new_vote_weight += voter->proxied_vote_weight;
      }

      std::map<name, std::pair<double, bool /*new*/> > producer_deltas;
      if ( voter->last_vote_weight > 0 ) {
         if( voter->proxy ) {
            auto old_proxy = _voters.find( voter->proxy.value );
            check( old_proxy != _voters.end(), "old proxy not found" ); //data corruption
            _voters.modify( old_proxy, same_payer, [&]( auto& vp ) {
                  vp.proxied_vote_weight -= voter->last_vote_weight;
               });
            propagate_weight_change( *old_proxy );
         } else {
            for( const auto& p : voter->producers ) {
               auto& d = producer_deltas[p];
               d.first -= voter->last_vote_weight;
               d.second = false;
            }
         }
      }

      if( proxy ) {
         auto new_proxy = _voters.find( proxy.value );
         check( new_proxy != _voters.end(), "invalid proxy specified" ); //if ( !voting ) { data corruption } else { wrong vote }
         check( !voting || new_proxy->is_proxy, "proxy not found" );
         if ( new_vote_weight >= 0 ) {
            _voters.modify( new_proxy, same_payer, [&]( auto& vp ) {
                  vp.proxied_vote_weight += new_vote_weight;
               });
            propagate_weight_change( *new_proxy );
         }
      } else {
         if( new_vote_weight >= 0 ) {
            for( const auto& p : producers ) {
               auto& d = producer_deltas[p];
               d.first += new_vote_weight;
               d.second = true;
            }
         }
      }

      const auto ct = current_time_point();
      double delta_change_rate         = 0.0;
      double total_inactive_vpay_share = 0.0;
      for( const auto& pd : producer_deltas ) {
         auto pitr = _producers.find( pd.first.value );
         if( pitr != _producers.end() ) {
            if( voting && !pitr->active() && pd.second.second /* from new set */ ) {
               check( false, ( "producer " + pitr->owner.to_string() + " is not currently registered" ).data() );
            }
            double init_total_votes = pitr->total_votes;
            _producers.modify( pitr, same_payer, [&]( auto& p ) {
               p.total_votes += pd.second.first;
               if ( p.total_votes < 0 ) { // floating point arithmetics can give small negative numbers
                  p.total_votes = 0;
               }
               _gstate.total_producer_vote_weight += pd.second.first;
               //check( p.total_votes >= 0, "something bad happened" );
            });
            auto prod2 = _producers2.find( pd.first.value );
            if( prod2 != _producers2.end() ) {
               const auto last_claim_plus_3days = pitr->last_claim_time + microseconds(3 * useconds_per_day);
               const auto crossed_threshold       = (last_claim_plus_3days <= ct);
               const auto updated_after_threshold = (last_claim_plus_3days <= prod2->last_votepay_share_update);
               // Note: updated_after_threshold implies cross_threshold

               double new_votepay_share = update_producer_votepay_share( prod2,
                                             ct,
                                             updated_after_threshold ? 0.0 : init_total_votes,
                                             crossed_threshold && !updated_after_threshold // only reset votepay_share once after threshold
                                          );

               if( !crossed_threshold ) {
                  delta_change_rate += pd.second.first;
               } else if( !updated_after_threshold ) {
                  total_inactive_vpay_share += new_votepay_share;
                  delta_change_rate -= init_total_votes;
               }
            }
         } else {
            if( pd.second.second ) {
               check( false, ( "producer " + pd.first.to_string() + " is not registered" ).data() );
            }
         }
      }

      update_total_votepay_share( ct, -total_inactive_vpay_share, delta_change_rate );

      _voters.modify( voter, same_payer, [&]( auto& av ) {
         av.last_vote_weight = new_vote_weight;
         av.producers = producers;
         av.proxy     = proxy;
      });
   }



   // PROTON
   void system_contract::update_xpr_votes( const name& from, const name& voter_name, const name& proxy, const std::vector<name>& producers, bool voting ) {
      //validate input
      if ( proxy ) {
         check( producers.size() == 0, "cannot vote for producers and proxy at same time" );
         check( voter_name != proxy, "cannot proxy to self" );
      } else {
         check( producers.size() <= _gstatesxpr.max_bp_per_vote , "attempt to vote for too many producers" ); // PROTON
         for( size_t i = 1; i < producers.size(); ++i ) {
            check( producers[i-1] < producers[i], "producer votes must be unique and sorted" );
         }
      }

      auto voter = _voters.require_find( voter_name.value, string("user must stake XPR before they can vote").c_str() );
      check( !proxy || !voter->is_proxy, "account registered as a proxy is not allowed to use a proxy" );

      auto new_vote_weight = stake2vote( voter->staked );
      if( voter->is_proxy ) {
         new_vote_weight += voter->proxied_vote_weight;
      }

      auto lastVotedBPs  = voter->producers;

      std::map<name, std::pair<double, bool /*new*/> > producer_deltas;
      if ( voter->last_vote_weight > 0 ) {
         if( voter->proxy ) {
            auto old_proxy = _voters.require_find( voter->proxy.value, string("old proxy not found").c_str() );
            _voters.modify( old_proxy, same_payer, [&]( auto& vp ) {
                  vp.proxied_vote_weight -= voter->last_vote_weight;
               });
            propagate_weight_change( *old_proxy );
         } else {
            for( const auto& p : voter->producers ) {
               auto& d = producer_deltas[p];
               d.first -= voter->last_vote_weight;
               d.second = false;
            }
         }
      }

      if( proxy ) {
         auto new_proxy = _voters.require_find( proxy.value, string("invalid proxy specified").c_str() );
         check( !voting || new_proxy->is_proxy, "proxy not found" );
         if ( new_vote_weight >= 0 ) {
            _voters.modify( new_proxy, same_payer, [&]( auto& vp ) {
                  vp.proxied_vote_weight += new_vote_weight;
               });
            propagate_weight_change( *new_proxy );
         }
      } else {
         if( new_vote_weight >= 0 ) {
            for( const auto& p : producers ) {
               auto& d = producer_deltas[p];
               d.first += new_vote_weight;
               d.second = true;
            }
         }
      }

      const auto ct = current_time_point();
      double delta_change_rate         = 0.0;
      double total_inactive_vpay_share = 0.0;
      for( const auto& pd : producer_deltas ) {
         auto pitr = _producers.find( pd.first.value );
         if( pitr != _producers.end() ) {
            if( voting && !pitr->active() && pd.second.second /* from new set */ ) {
               check( false, ( "producer " + pitr->owner.to_string() + " is not currently registered" ).data() );
            }
            double init_total_votes = pitr->total_votes;
            _producers.modify( pitr, same_payer, [&]( auto& p ) {
               p.total_votes += pd.second.first;
               if ( p.total_votes < 0 ) { // floating point arithmetics can give small negative numbers
                  p.total_votes = 0;
               }
               _gstate.total_producer_vote_weight += pd.second.first;
               //check( p.total_votes >= 0, "something bad happened" );
            });
            auto prod2 = _producers2.find( pd.first.value );
            if( prod2 != _producers2.end() ) {
               const auto last_claim_plus_3days = pitr->last_claim_time + microseconds(3 * useconds_per_day);
               bool crossed_threshold       = (last_claim_plus_3days <= ct);
               bool updated_after_threshold = (last_claim_plus_3days <= prod2->last_votepay_share_update);
               // Note: updated_after_threshold implies cross_threshold

               double new_votepay_share = update_producer_votepay_share( prod2,
                                             ct,
                                             updated_after_threshold ? 0.0 : init_total_votes,
                                             crossed_threshold && !updated_after_threshold // only reset votepay_share once after threshold
                                          );

               if( !crossed_threshold ) {
                  delta_change_rate += pd.second.first;
               } else if( !updated_after_threshold ) {
                  total_inactive_vpay_share += new_votepay_share;
                  delta_change_rate -= init_total_votes;
               }
            }
         } else {
            if( pd.second.second ) {
               check( false, ( "producer " + pd.first.to_string() + " is not registered" ).data() );
            }
         }
      }


      auto voter_info_itr = vxpr_tbl.find( from.value );
      optional<bool>   startqualif = std::nullopt;

      //del_xpr_table     sxpr_tbl( get_self(), voter_name.value );
      //auto sxpr_itr = sxpr_tbl.find( voter_name.value );

      if (lastVotedBPs.size() < _gstatesxpr.min_bp_reward) {
         if (producers.size() >= _gstatesxpr.min_bp_reward) {
            //new qualified voter
            _gstatesd.totalrstaked += voter->staked;
            _gstatesd.totalrvoters++;
            if ( voter_info_itr != vxpr_tbl.end() ){

               vxpr_tbl.modify( voter_info_itr, same_payer, [&]( auto& dbo ){
                   if (_gstatesd.isprocessing && _gstatesd.processFrom.value < from.value && dbo.startqualif == std::nullopt) {
                      dbo.startqualif = voter_info_itr->isqualified; //std::nullopt;
                   }

                  dbo.isqualified = true;
               });
            }
         }
      } else {
        if (producers.size() < _gstatesxpr.min_bp_reward) {
           //remove qualified voter
           _gstatesd.totalrstaked -= voter->staked;
           _gstatesd.totalrvoters--;

           if ( voter_info_itr != vxpr_tbl.end() ){
              vxpr_tbl.modify( voter_info_itr, same_payer, [&]( auto& dbo ){
                 if (_gstatesd.isprocessing && _gstatesd.processFrom.value < from.value && dbo.startqualif == std::nullopt) {
                    dbo.startqualif = voter_info_itr->isqualified; //std::nullopt;
                 }

                 dbo.isqualified = false;
              });
           }
        }
      }

      update_total_votepay_share( ct, -total_inactive_vpay_share, delta_change_rate );

      _voters.modify( voter, same_payer, [&]( auto& av ) {
         av.last_vote_weight = new_vote_weight;
         av.producers = producers;
         av.proxy     = proxy;
      });
   }

   void system_contract::regproxy( const name& proxy, bool isproxy ) {
      require_auth( proxy );

      check (checkPermission(proxy, "regproxy")==1, "You are not authorised to register as proxy");  // PROTON Check Permissions

      auto pitr = _voters.find( proxy.value );
      if ( pitr != _voters.end() ) {
         check( isproxy != pitr->is_proxy, "action has no effect" );
         check( !isproxy || !pitr->proxy, "account that uses a proxy is not allowed to become a proxy" );
         _voters.modify( pitr, same_payer, [&]( auto& p ) {
               p.is_proxy = isproxy;
            });
         propagate_weight_change( *pitr );
      } else {
         _voters.emplace( proxy, [&]( auto& p ) {
               p.owner  = proxy;
               p.is_proxy = isproxy;
            });
      }
   }

   void system_contract::propagate_weight_change( const voter_info& voter ) {
      check( !voter.proxy || !voter.is_proxy, "account registered as a proxy is not allowed to use a proxy" );
      double new_weight = stake2vote( voter.staked );
      if ( voter.is_proxy ) {
         new_weight += voter.proxied_vote_weight;
      }

      /// don't propagate small changes (1 ~= epsilon)
      if ( fabs( new_weight - voter.last_vote_weight ) > 1 )  {
         if ( voter.proxy ) {
            auto& proxy = _voters.get( voter.proxy.value, "proxy not found" ); //data corruption
            _voters.modify( proxy, same_payer, [&]( auto& p ) {
                  p.proxied_vote_weight += new_weight - voter.last_vote_weight;
               }
            );
            propagate_weight_change( proxy );
         } else {
            auto delta = new_weight - voter.last_vote_weight;
            const auto ct = current_time_point();
            double delta_change_rate         = 0;
            double total_inactive_vpay_share = 0;
            for ( auto acnt : voter.producers ) {
               auto& prod = _producers.get( acnt.value, "producer not found" ); //data corruption
               const double init_total_votes = prod.total_votes;
               _producers.modify( prod, same_payer, [&]( auto& p ) {
                  p.total_votes += delta;
                  _gstate.total_producer_vote_weight += delta;
               });
               auto prod2 = _producers2.find( acnt.value );
               if ( prod2 != _producers2.end() ) {
                  const auto last_claim_plus_3days = prod.last_claim_time + microseconds(3 * useconds_per_day);
                  bool crossed_threshold       = (last_claim_plus_3days <= ct);
                  bool updated_after_threshold = (last_claim_plus_3days <= prod2->last_votepay_share_update);
                  // Note: updated_after_threshold implies cross_threshold

                  double new_votepay_share = update_producer_votepay_share( prod2,
                                                ct,
                                                updated_after_threshold ? 0.0 : init_total_votes,
                                                crossed_threshold && !updated_after_threshold // only reset votepay_share once after threshold
                                             );

                  if( !crossed_threshold ) {
                     delta_change_rate += delta;
                  } else if( !updated_after_threshold ) {
                     total_inactive_vpay_share += new_votepay_share;
                     delta_change_rate -= init_total_votes;
                  }
               }
            }

            update_total_votepay_share( ct, -total_inactive_vpay_share, delta_change_rate );
         }
      }
      _voters.modify( voter, same_payer, [&]( auto& v ) {
            v.last_vote_weight = new_weight;
         }
      );
   }

   // PROTON
   void system_contract::voterclaimst( const name& owner ) {
      voterclaim_internal( owner, true );   
   }
	  
	  
   void system_contract::voterclaim( const name& owner ) {
      voterclaim_internal( owner, false );   
   }
   
   void system_contract::voterclaim_internal( const name& owner, const bool& tostake ) {
      require_auth( owner );
      auto voter_info_itr = vxpr_tbl.require_find( owner.value, string("Voter not found.").c_str() );

      check ( current_time_point().sec_since_epoch() - voter_info_itr->lastclaim  > _gstatesxpr.voters_claim_interval, "Your last claim was less than 24h ago. Please wait " + timeToWait( abs((int)( _gstatesxpr.voters_claim_interval - (uint64_t)current_time_point().sec_since_epoch() + voter_info_itr->lastclaim)) ) );
      check ( voter_info_itr->claimamount > 0, "Nothing to claim.");

      token::transfer_action transfer_act{ token_account, { {saving_account, active_permission}, {owner, active_permission} } };
      transfer_act.send( saving_account, voter_info_itr->owner, asset(voter_info_itr->claimamount, XPRsym), std::string("Voter claim reward") ); //PROTON

      if ( tostake ) {
         system_contract::stakexpr_action stake_act{ get_self(), { {owner, active_permission} } };
         stake_act.send( owner, owner, asset(voter_info_itr->claimamount, XPRsym) ); //PROTON

      }
      _gstatesd.notclaimed -= voter_info_itr->claimamount;

      vxpr_tbl.modify(voter_info_itr, owner, [&](auto& s) {
            s.claimamount = 0;
            s.lastclaim = current_time_point().sec_since_epoch();
      });
   }
   

   // PROTON
   void system_contract::vrwrdsharing(  ) {
      require_auth( get_self() );

      const auto now = current_time_point().sec_since_epoch();
      bool isWork = false;

      if (_gstatesd.isprocessing)  {
         isWork = true;
      } else if ( now - _gstatesd.processtime >=  _gstatesxpr.process_interval){
         isWork = true;
         _gstatesd.processtime = now;
         _gstatesd.processQuant = _gstatesd.pool;
         _gstatesd.pool = 0;
         _gstatesd.isprocessing = true;
         _gstatesd.processFrom = ""_n;
         _gstatesd.processrstaked = _gstatesd.totalrstaked;
      } else {
         print ("No scheduled work");
      }
      if (isWork){
          _gstatesd.processtimeupd = now;

         auto itr = vxpr_tbl.begin();
         if (_gstatesd.processFrom.value) {
            //itr = vxpr_tbl.find(_gstatesd.processFrom.value);
            itr = vxpr_tbl.lower_bound(_gstatesd.processFrom.value);
         }

         uint64_t count = 0;

         while ( itr != vxpr_tbl.end() && count < _gstatesxpr.process_by ){
            count++;

            optional<bool> userQualify = itr->isqualified;
            if ( itr->startqualif != std::nullopt ) {
               userQualify = itr->startqualif;
            }

            if ( userQualify == true){
               optional<uint64_t> userStake =itr->startstake != std::nullopt ? itr->startstake : itr->staked;

               uint64_t claim = 0;
               if (_gstatesd.processrstaked != 0) {
                   claim = (uint128_t)((uint128_t)_gstatesd.processQuant * (uint128_t)userStake.value()) / _gstatesd.processrstaked;
               }

               _gstatesd.processed += claim;
               _gstatesd.notclaimed += claim;

               vxpr_tbl.modify(itr, get_self(), [&](auto& s) {
                  s.claimamount += claim;
                  s.startstake = std::nullopt;
                  s.startqualif = std::nullopt;
               });
            }
		 
            if ( itr->startqualif != std::nullopt || itr->startstake != std::nullopt ) {
               vxpr_tbl.modify(itr, get_self(), [&](auto& s) {
                  s.startstake = std::nullopt;
                  s.startqualif = std::nullopt;
               });
	    }
		 
            itr++;
         }

         if ( itr != vxpr_tbl.end() ){
            _gstatesd.processFrom = itr->owner;

         } else {
            _gstatesd.isprocessing = false;
            _gstatesd.processFrom = ""_n;
            _gstatesd.processtimeupd = 0;

            if (_gstatesd.processed < _gstatesd.processQuant){
               _gstatesd.pool += _gstatesd.processQuant - _gstatesd.processed;
            }

            _gstatesd.processQuant = 0;
            _gstatesd.processed = 0;
            _gstatesd.processrstaked = 0;

         }

      }
   }


   // PROTON
   void system_contract::check_vote_sharing(  ) {
      const auto now = current_time_point().sec_since_epoch();

      if (_gstatesd.isprocessing)  {
         if (now - _gstatesd.processtimeupd >  0) {
             vrwrdsharing();
         }

      } else {
         if ( now - _gstatesd.processtime >=  _gstatesxpr.process_interval) {
            vrwrdsharing();
         }
      }
   }


   // PROTON
   void system_contract::setxprvconf( const uint64_t&  max_bp_per_vote, const uint64_t& min_bp_reward, const uint64_t& unstake_period, const uint64_t& process_by, const uint64_t& process_interval, const uint64_t& voters_claim_interval ) {
      require_auth( get_self() );

      check ( process_by <= 1000, "max 1000 for process_by" );
      check ( max_bp_per_vote <= 50, "max 50 for max_bp_per_vote" );
      check ( process_interval >= 10800, "min 10800 for process_interval" );

      _gstatesxpr.max_bp_per_vote = max_bp_per_vote;
      _gstatesxpr.min_bp_reward = min_bp_reward;
      _gstatesxpr.unstake_period = unstake_period;
      _gstatesxpr.process_by = process_by;
      _gstatesxpr.process_interval = process_interval;
      _gstatesxpr.voters_claim_interval = voters_claim_interval;
   }


   // PROTON
   void system_contract::ontransfer(name from, name to, asset quantity, string memo) {
      if (to != get_self()) {
         return;
      }
	  
      if (from == stake_account) {
         return;
      }
	  
      require_auth(from);
      auto contract = get_first_receiver();

      check ( contract == "eosio.token"_n, "Token is not supported");
      //check (from == "swapfee"_n, "You are not allowed to transfer");         // Allow only swap fee contract to send here
      check ( memo == "spf", "Wrong memo.");                                    // Allow transfers only with correct memo
      if ( contract == "eosio.token"_n && quantity.symbol == XPRsym ){
          
         // transfer to savings account & add amount to votersreward pool
         if ( quantity.amount > 0 ) {
            token::transfer_action transfer_act{ token_account, { {get_self(), active_permission} } };
            transfer_act.send( get_self(), saving_account, quantity, "spf" ); 
            _gstatesd.pool += quantity.amount;
         }
      }
}

} /// namespace eosiosystem
