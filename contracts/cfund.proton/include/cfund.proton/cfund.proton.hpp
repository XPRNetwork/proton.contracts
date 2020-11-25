/*##################################*\
#
#
# Created by CryptoLions.io
#
#
\*##################################*/

#pragma once

#include <eosio/asset.hpp>
#include <eosio/eosio.hpp>
#include <eosio/symbol.hpp>
#include <eosio/singleton.hpp>
#include <eosio/transaction.hpp>

#include <string>

using namespace eosio;
using std::string;

#define TOKEN_SYMB symbol("XPR", 4)

const name TOKEN_CONTRACT			= "eosio.token"_n;


const uint64_t claimInterval 		= 60 * 60 * 24 * 1; 	//1 day
const uint64_t processIntreval 		= 60 * 60 * 6; 			//6 h

//const uint64_t claimInterval 		= 2 * 60 ; 				//2m TEST !!!!!!!!!!
//const uint64_t processIntreval 	= 10 * 60 ; 			//10min TEST !!!!!!!!!!


namespace eosiosystem {
   class system_contract;
}

namespace eosio {

	class [[eosio::contract("cfund.proton")]] cfundproton : public contract {
		public:
			using contract::contract;

			/**
			* Register (Add) New Committee Member
			*
			* 
			* @param name
			*/				
			[[eosio::action]]
			void reg( const name& account);
			using reg_action = eosio::action_wrapper<"reg"_n, &cfundproton::reg>;

		
			/**
			* Unregister (Remove) Commitee Member
			*
			* 
			* @param name
			*/
			[[eosio::action]]
			void unreg( const name& account );
			using unreg_action = eosio::action_wrapper<"unreg"_n, &cfundproton::unreg>;

			/**
			* Activate Comittee Member 
			*
			* Set Committee Member to active or not active.
			*
			* @param account
			* @param status
			*/			
			[[eosio::action]]
			void activate( const name& account, const bool& status );
			using activate_action = eosio::action_wrapper<"activate"_n, &cfundproton::activate>;
			
			/**
			* Claim Reward
			*
			* Called by Committee Members to claim their inflation-based rewards.
			* 
			* @param name
			*/			
			[[eosio::action]]
			void claimreward( const name& account);
			using claimreward_action = eosio::action_wrapper<"claimreward"_n, &cfundproton::claimreward>;

			/**
			* Process
			*
			* Internal function used to process rewards distribution.
			* 
			*/			
			[[eosio::action]]
			void process( );
			using process_action = eosio::action_wrapper<"process"_n, &cfundproton::process>;
	
			void onTokenReceive( const name& from, const name& to, const asset& quantity, const string& memo );	
			
		private:

			TABLE user {
				name		account;
				uint8_t		rate;
				bool		active;
				uint64_t	claimamount;
				uint64_t	lastclaim;
				
				auto primary_key() const {
					return account.value;
				}
			};
			
			typedef eosio::multi_index< "users"_n, user> users;	
			
			TABLE global {
				global(){}
				uint64_t 	totalusr = 0;
				uint64_t 	totalausr = 0;
				uint64_t 	notclaimed = 0;
				uint64_t 	pool = 0;
				uint64_t 	processtime = 0;	
				
				bool	 	isprocessing = false;
				name 	 	processFrom = ""_n;
				uint8_t  	processBy = 10;
				uint64_t 	processQuant = 0;
				uint64_t 	processed = 0;

				EOSLIB_SERIALIZE( global, (totalusr)(totalausr)(notclaimed)(pool)(processtime)(isprocessing)(processFrom)(processBy)(processQuant)(processed) ) //(data)
			};
			
			typedef eosio::singleton< "global"_n, global> conf;	
			global _gstate;
							
	};
} /// namespace eosio

extern "C"
void apply( uint64_t receiver, uint64_t code, uint64_t action ){

	if ( code == TOKEN_CONTRACT.value && action == "transfer"_n.value ) {
		eosio::execute_action( eosio::name( receiver ), eosio::name( code ), &cfundproton::onTokenReceive );
	} else if ( code == receiver ) {
		switch ( action ) {
			EOSIO_DISPATCH_HELPER( cfundproton, ( reg )( unreg )( activate )( claimreward )( process ) )
		}
	}
}
