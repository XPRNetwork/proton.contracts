#pragma once

#include <eosio/eosio.hpp>
#include <eosio/asset.hpp>
#include <eosio/singleton.hpp>


namespace eosiosystem {
   class system_contract;
}

namespace eosio {

	class [[eosio::contract("eosio.proton")]] eosioproton : public contract {
		public:
			using contract::contract;

			[[eosio::action]]
			void setperm(name acc, const std::map<std::string,uint8_t>& perms);
			using setperm_action = eosio::action_wrapper<"setperm"_n, &eosioproton::setperm>;

			[[eosio::action]]
			void setperm2(name acc, const std::vector<uint8_t>& perms);
			using setperm2_action = eosio::action_wrapper<"setperm2"_n, &eosioproton::setperm2>;

			[[eosio::action]]
			void remove(name acc);
			using remove_action = eosio::action_wrapper<"remove"_n, &eosioproton::remove>;

			[[eosio::action]]
			void reqperm(name acc, std::string permission );
			using reqperm_action = eosio::action_wrapper<"reqperm"_n, &eosioproton::reqperm>;
			
			[[eosio::action]]
			void setuserava(name acc, std::string ava );
			using setuserava_action = eosio::action_wrapper<"setuserava"_n, &eosioproton::setuserava>;

			[[eosio::action]]
			void setuserdata(name acc, std::string data );
			using setuserdata_action = eosio::action_wrapper<"setuserdata"_n, &eosioproton::setuserdata>;
			
			[[eosio::action]]
			void setusername(name acc, std::string name );
			using setusername_action = eosio::action_wrapper<"setusername"_n, &eosioproton::setusername>;
			
			[[eosio::action]]
			void userverify(name acc, bool  verified);
			using userverify_action = eosio::action_wrapper<"userverify"_n, &eosioproton::userverify>;

			[[eosio::action]]
			void dappreg(name account);
			using dappreg_action = eosio::action_wrapper<"dappreg"_n, &eosioproton::dappreg>;

			[[eosio::action]]
			void setdappconf(uint64_t ram, uint64_t cpu, uint64_t net);
			using setdappconf_action = eosio::action_wrapper<"setdappconf"_n, &eosioproton::setdappconf>;

 
			static std::map<std::string,uint8_t> get_priv( name contract_account, name acc ){
				std::map<std::string,uint8_t> res;
				
				//exception for eosio account
				if ( acc == "eosio"_n ) {
					res["createacc"] = 1; res["vote"] = 1; res["regprod"] = 1; res["regproxy"] = 1; res["setcontract"] = 1; res["namebids"] = 1; res["rex"] = 1; res["delegate"] = 1; res["undelegate"] = 1; res["sellram"] = 1; res["buyram"] = 1;
					return res;			
				}
				
				res["createacc"] = 0; res["vote"] = 0; res["regprod"] = 0; res["regproxy"] = 0; res["setcontract"] = 0; res["namebids"] = 0; res["rex"] = 0; res["delegate"] = 0; res["undelegate"] = 0; res["sellram"] = 0; res["buyram"] = 0;

				permissions perm( contract_account, contract_account.value );
				auto existing = perm.find( acc.value );
				if ( existing != perm.end() ) {
					res["createacc"] = existing->createacc;
					res["vote"] = existing->vote;
					res["regprod"] = existing->regprod;
					res["regproxy"] = existing->regproxy;
					res["setcontract"] = existing->setcontract;
					res["namebids"] = existing->namebids;
					res["rex"] = existing->rex;
					
					res["delegate"] = existing->delegate;
					res["undelegate"] = existing->undelegate;					
					res["sellram"] = existing->sellram;
					res["buyram"] = existing->buyram;					
					
				}
				return res;			
			}

	private:

		// 0 = none, 1 = on, 2 = pending, 3 = off, 4 = banned
		struct [[eosio::table]] permission {
			name		acc;
			uint8_t		createacc;
			uint8_t		vote;
			uint8_t		regprod;
			uint8_t		regproxy;
			uint8_t		setcontract;
			uint8_t		namebids;
			uint8_t		rex;
			uint8_t		delegate;
			uint8_t		undelegate;
			uint8_t		sellram;
			uint8_t		buyram;

			uint64_t primary_key()const { return acc.value; }
		};

		typedef eosio::multi_index< "permissions"_n, permission > permissions;


		struct [[eosio::table]] userinfo {
			name			acc;
			std::string		name;			
			std::string		avatar;
			bool			verified;
			uint64_t		date;
			std::string		data;
			bool			primary;  ///!!!!!!!!!!!!!! REMOVE On New CHAIN

			uint64_t primary_key()const { return acc.value; }
		};

		typedef eosio::multi_index< "usersinfo"_n, userinfo > usersinfo;
		
		struct [[eosio::table]] user_resources {
			name          owner;
			asset         net_weight;
			asset         cpu_weight;
			int64_t       ram_bytes = 0;

			bool is_empty()const { return net_weight.amount == 0 && cpu_weight.amount == 0 && ram_bytes == 0; }
			uint64_t primary_key()const { return owner.value; }

			// explicit serialization macro is not necessary, used here only to improve compilation time
			EOSLIB_SERIALIZE( user_resources, (owner)(net_weight)(cpu_weight)(ram_bytes) )
		};
		typedef eosio::multi_index< "userres"_n, user_resources >      user_resources_table;
		
		
		TABLE dappconf {
			dappconf(){}
			uint64_t dappramm = 2 * 1024 * 1024;
			uint64_t dappcpu = 200000;
			uint64_t dappnet = 200000;

			EOSLIB_SERIALIZE( dappconf, (dappramm)(dappcpu)(dappnet))
		};
		typedef eosio::singleton< "dappconf"_n, dappconf> dappconfig;
		dappconf _dcstate;



		//add singelton for producer pay config

	};

} /// namespace eosio
