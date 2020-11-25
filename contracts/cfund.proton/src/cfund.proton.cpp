/*##################################*\
#
#
# Created by CryptoLions.io
#
#
\*##################################*/


#include <cfund.proton/cfund.proton.hpp>
#include <eosio/system.hpp>

namespace eosio {
		
	ACTION cfundproton::reg( const name& account ){

		require_auth( get_self() );
		check( is_account( account ), "Account does not exist.");
		
		conf config(get_self(), get_self().value);
		_gstate = config.exists() ? config.get() : global{};

		users users_( get_self(), get_self().value );	
		auto itr = users_.find( account.value );
		check ( itr == users_.end(), "committee already registered.");

		// now = current_time_point().sec_since_epoch();			
		if ( itr == users_.end() ) {
			users_.emplace( get_self(), [&](auto& s) {
				s.account = account;
				s.rate = 100;
				s.active = true;
				s.claimamount = 0;
				s.lastclaim = 0;
			});			
			
			_gstate.totalusr++;
			_gstate.totalausr++;
			
			config.set( _gstate, get_self() );
		}
	}


	ACTION cfundproton::unreg( const name& account ){
		
		require_auth( get_self() );
		
		conf config( get_self(), get_self().value);
		_gstate = config.exists() ? config.get() : global{};
		
		users users_( get_self(), get_self().value );
		auto itr = users_.find( account.value );
		
		check(itr != users_.end(), "committee not registered.");	

		if ( itr->active ) {
			_gstate.totalausr--;
		}
		
		users_.erase(itr);

		_gstate.totalusr--;
		config.set( _gstate, get_self() );
	}

	
	ACTION cfundproton::activate( const name& account, const bool& status ){
		require_auth( get_self() );

		conf config( get_self(), get_self().value);
		_gstate = config.exists() ? config.get() : global{};

		users users_( get_self(), get_self().value );
		auto itr = users_.find( account.value );
		
		check(itr != users_.end(), "Committee not found.");	
		
		check(status != itr->active, "status is the same");	

		if (status) {
			_gstate.totalausr++;
		} else {
			_gstate.totalausr--;			
		}
			
		users_.modify(itr, get_self(), [&](auto& s) {
			s.active = status;
		});	
			
		config.set( _gstate, get_self() );
	}
	
	
	ACTION cfundproton::claimreward( const name& account){
		require_auth( account );

		conf config( get_self(), get_self().value);
		_gstate = config.exists() ? config.get() : global{};
		
		users users_( get_self(), get_self().value );		
		auto itr = users_.find(account.value);

		check(itr != users_.end(), "Committee not found.");	
		check ( current_time_point().sec_since_epoch() - itr->lastclaim  > claimInterval, "You last claim was less than 24h ago.");
		check ( itr->claimamount > 0, "Nothing to claim.");
		check ( itr->active == true, "committee member is not activated.");
		
		auto sendClaim = action(
			permission_level{ get_self(), "active"_n },
			TOKEN_CONTRACT,
			"transfer"_n,
			std::make_tuple( get_self(), itr->account, asset(itr->claimamount, TOKEN_SYMB), std::string("Committee claim reward") )
		);

		sendClaim.send();

		if ( _gstate.notclaimed > itr->claimamount) // should be always greater .
			_gstate.notclaimed -= itr->claimamount;
		
		users_.modify(itr, account, [&](auto& s) {
			s.claimamount = 0;
			s.lastclaim = current_time_point().sec_since_epoch();
		});
		
		config.set( _gstate, get_self() );
	}


	ACTION cfundproton::process( ){
		require_auth( get_self() );

		conf config( get_self(), get_self().value);
		_gstate = config.exists() ? config.get() : global{};
		
		users users_( get_self(), get_self().value );
		
		uint64_t now = current_time_point().sec_since_epoch();
		
		bool isWork = false;

		if (_gstate.isprocessing == true)  {
			isWork = true;
		} else if ( now - _gstate.processtime >=  processIntreval){
			isWork = true;
			_gstate.processtime = now;
			_gstate.processQuant = _gstate.pool;
			_gstate.pool = 0;
			_gstate.isprocessing = true;
			_gstate.processFrom = ""_n;
			
			//Deferred for auto next call
			eosio::transaction out;
			out.actions.emplace_back( permission_level{get_self(), "active"_n}, get_self(), "process"_n, get_self() );
			out.delay_sec = processIntreval + 1;
			out.send( get_self().value, get_self(), true );		
		} else {
			print("nothing to do.");
		}

		if (isWork ){
			auto itr = users_.begin();
			if (_gstate.processFrom.value) {
				itr = users_.lower_bound(_gstate.processFrom.value);
			}
			
			uint64_t count = 0;
			
			while ( itr != users_.end() && count < _gstate.processBy ){
				count++;
				if ( itr->active ){
					uint64_t claim =  0;
					if ( _gstate.totalausr > 0 ) {
						claim = _gstate.processQuant / _gstate.totalausr ;
					}
					
					_gstate.processed += claim;
				
					users_.modify(itr, get_self(), [&](auto& s) {
						s.claimamount += claim;			
					});
				}
				itr++;
			}
			
			if ( itr != users_.end() ){
				_gstate.processFrom = itr->account;	
				print("Not Finished. ");
				//Deferred for auto next call
				eosio::transaction out;
				out.actions.emplace_back( permission_level{get_self(), "active"_n}, get_self(), "process"_n, get_self() );
				out.delay_sec = 0;
				out.send( get_self().value, get_self(), true );		
			} else {
				_gstate.isprocessing = false;
				_gstate.processFrom = ""_n;
				
				if (_gstate.processed < _gstate.processQuant){
					print("Finished. Tokens returned to pool: ", (_gstate.processQuant - _gstate.processed));
					_gstate.pool += _gstate.processQuant - _gstate.processed;
				}
				
				_gstate.processQuant = 0;
				_gstate.processed = 0;
			}		
			config.set( _gstate, get_self() );
		}
	}

	
	
	void cfundproton::onTokenReceive( const name& from, const name& to, const asset& quantity, const string& memo ){
		if ( to != get_self() ) {
			return;
		}
		if ( from == get_self() ) {
			return;
		}
		
		if ( quantity.symbol.code().raw() != TOKEN_SYMB.code().raw() ) {
			return;
		}
		
		conf config(get_self(), get_self().value);
		_gstate = config.exists() ? config.get() : global{};

		_gstate.pool += quantity.amount;
		_gstate.notclaimed += quantity.amount;

		config.set( _gstate, get_self() );
	}
	
}
