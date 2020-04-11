/*##################################*\
#
#
# Created by CryptoLions.io
#
#
\*##################################*/

#pragma once

#include <eosio/eosio.hpp>
#include <eosio/asset.hpp>
#include <eosio/singleton.hpp>

using namespace eosio;
using namespace std;

namespace eosiosystem {
   class system_contract;
}

namespace eosio {

	class [[eosio::contract("token.proton")]] tokenproton : public contract {
		public:
			using contract::contract;

			[[eosio::action]]
			void reg(name tcontract, string tname, string url, string desc, string iconurl, symbol symbol);
			using reg_action = eosio::action_wrapper<"reg"_n, &tokenproton::reg>;

			//[[eosio::action]]
			//void reglog(uint64_t id, name tcontract, string tname, string url, string desc, string iconurl, symbol symbol);
			//using reglog_action = eosio::action_wrapper<"reglog"_n, &tokenproton::reglog>;

			[[eosio::action]]
			void update(uint64_t id, name tcontract, string tname, string url, string desc, string iconurl, symbol symbol);
			using update_action = eosio::action_wrapper<"update"_n, &tokenproton::update>;

			[[eosio::action]]
			void remove(uint64_t id);
			using remove_action = eosio::action_wrapper<"remove"_n, &tokenproton::remove>;

			[[eosio::action]]
			void updblacklist(uint64_t id, bool blisted);
			using updblacklist_action = eosio::action_wrapper<"updblacklist"_n, &tokenproton::updblacklist>;

			
		private:
			uint64_t getid( );
			
			
			struct [[eosio::table]] token {
				uint64_t	id;
				name		tcontract;
				string		tname;
				string		url;
				string		desc;
				string		iconurl;
				symbol		symbol;
				bool		blisted;
				
				auto primary_key() const {
					return id;
				}
				uint64_t by_tcontract() const {
					return tcontract.value;
				}
				
				
			};


			typedef eosio::multi_index< "tokens"_n, token,
				eosio::indexed_by< "tcontract"_n, eosio::const_mem_fun<token, uint64_t, &token::by_tcontract> >
				> tokens;
				


			/*
			* global singelton table, used for id building. Scope: self
			*/
			TABLE global {
				global() {}
				uint64_t tid = 1000000;
		
				uint64_t spare1 = 0;
				uint64_t spare2 = 0;

				EOSLIB_SERIALIZE( global, ( tid )( spare1 )( spare2 ) )
			};

			typedef eosio::singleton< "global"_n, global > conf; /// singleton
			global _cstate; /// global state

	};

} /// namespace eosio
