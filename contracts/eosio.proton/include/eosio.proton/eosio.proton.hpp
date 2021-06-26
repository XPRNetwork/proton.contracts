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

#include <eosio.system/eosio.system.hpp> //PROTON

#define SYSsym symbol("SYS", 4)  // PROTON

struct kyc_prov {
	name kyc_provider;
	string kyc_level;
	uint64_t kyc_date;
};


namespace eosiosystem {
	class system_contract;
}

namespace eosio {

	class [[eosio::contract("eosio.proton")]] eosioproton : public contract {
		public:
			using contract::contract;

			/**
			* Set Permissions
			*
			* Sets permissions for specified account for whitelisted EOSIO functionality. (no longer used)
			* 
			* @param acc - account name for which to set permissions
			* @param perms - permissions
			*/
			[[eosio::action]]
			void setperm(name acc, const std::map<std::string,uint8_t>& perms);
			using setperm_action = eosio::action_wrapper<"setperm"_n, &eosioproton::setperm>;

			/**
			* Set Permissions 2
			*
			* Sets permissions for specified account for whitelisted EOSIO functionality.
			* 
			* NOTE: The setperm, wasn't used because it didn't work in eosjs when we were deploying. 
			* We created this new action, setperm2, which worked with an array, and that's what is used.
			* 
			* @param acc - account name for which to set permissions
			* @param perms - permissions
			*/
			[[eosio::action]]
			void setperm2(name acc, const std::vector<uint8_t>& perms);
			using setperm2_action = eosio::action_wrapper<"setperm2"_n, &eosioproton::setperm2>;

		
			/**
			* Remove User Info
			*
			* Removes user info data for specified account.
			*
			* @param acc - account name for which to remove user info data 
			*/
			[[eosio::action]]
			void remove(name acc);
			using remove_action = eosio::action_wrapper<"remove"_n, &eosioproton::remove>;

			/**
			* Request Permission
			*
			* Request whitelisted permissions. (sets status to pending)
			* 
			* @param acc
			* @param permission
			*/		
			[[eosio::action]]
			void reqperm(name acc, std::string permission );
			using reqperm_action = eosio::action_wrapper<"reqperm"_n, &eosioproton::reqperm>;

			/**
			* Set User Avatar
			*
			* @param acc
			* @param ava - transcoded image for avatar
			*/		
			[[eosio::action]]
			void setuserava(name acc, std::string ava );
			using setuserava_action = eosio::action_wrapper<"setuserava"_n, &eosioproton::setuserava>;

			/**
			* Set User Name
			*
			* @param acc
			* @param name
			*/		
			[[eosio::action]]
			void setusername(name acc, std::string name );
			using setusername_action = eosio::action_wrapper<"setusername"_n, &eosioproton::setusername>;

			/**
			* Set User Verify
			*
			* @param acc - account name of user to be verified
			* @param verifier - account name of verfier authority 
			* @param verified 
			*/		
			[[eosio::action]]
			void userverify(name acc, name verifier, bool  verified);
			using userverify_action = eosio::action_wrapper<"userverify"_n, &eosioproton::userverify>;

			/**
			* Update Accounts
			*
			* Updates a list of trusted accounts for the specified user
			*
			* @param acc
			* @param raccs
			*/		
			[[eosio::action]]
			void updateraccs(name acc, vector<name> raccs);
			using updateraccs_action = eosio::action_wrapper<"updateraccs"_n, &eosioproton::updateraccs>;

			/**
			* Update Actions
			*
			* Updates a list of trusted actions for the specified user
			*
			* @param acc
			* @param accts
			*/			
			[[eosio::action]]
			void updateaacts(name acc, vector<tuple<name, name>> aacts);
			using updateaacts_action = eosio::action_wrapper<"updateaacts"_n, &eosioproton::updateaacts>;
			
			/**
			* Update Coins
			*
			* Updates a list of trusted tokens for the specified user
			* 
			* @param acc
			* @param name
			*/		
			[[eosio::action]]
			void updateac(name acc, vector<tuple<name, string>> ac);
			using updateac_action = eosio::action_wrapper<"updateac"_n, &eosioproton::updateac>;

			/**
			* New account minimum resources
			*
			* Gives minimum resources to new account
			* 
			* @param account
			*/		
			[[eosio::action]]
			void newaccres(name account);
			using newaccres_action = eosio::action_wrapper<"newaccres"_n, &eosioproton::newaccres>;

			/**
			* Kick BP
			*
			* Removes the ability of specified BP to register as a block producer. This is probably called from
			* kickbp in eosio.system, which also unregisters them.
			* 
			* @param producer
			*/		
 			[[eosio::action]]
			void kickbp( name producer );
			using kickbp_action = eosio::action_wrapper<"kickbp"_n, &eosioproton::kickbp>;
         
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


 			// KYC Logic	
		
			/**
			* Add KYC Approval
			*
			* Adds KYC Approval to specified account
			* 
			* @param acc - account name receiving approval
			* @param kyc - account name of kyc provider
			*/		
 			[[eosio::action]]
			void addkyc( name acc, kyc_prov kyc );
			using addkyc_action = eosio::action_wrapper<"addkyc"_n, &eosioproton::addkyc>;

		
			/**
			* Update KYC Approval
			*
			* Update KYC approval for specified account
			* 
			* @param acc - account name whose approval gets modified
			* @param kyc - account name of kyc provider
			*/
 			[[eosio::action]]
			void updatekyc( name acc, kyc_prov kyc );
			using updatekyc_action = eosio::action_wrapper<"updatekyc"_n, &eosioproton::updatekyc>;

		
			/**
			* Remove KYC Approval
			*
			* Remove KYC approval for specified account
			* 
			* @param acc - account name losing approval
			* @param kyc - account name of kyc provider
			*/
			[[eosio::action]]
			void removekyc( name acc, name kyc_provider );
			using removekyc_action = eosio::action_wrapper<"removekyc"_n, &eosioproton::removekyc>;

 			/**
			* Add KYC Provider
			*
			* @param kyc_provider
			* @param desc
			* @param url
			* @param iconurl
			* @param name
			* 
			*/
			[[eosio::action]]
			void addkycprov( name kyc_provider, std::string desc, std::string url, std::string iconurl, std::string name );
			using addkycprov_action = eosio::action_wrapper<"addkycprov"_n, &eosioproton::addkycprov>;

			/**
			* Blacklist KYC Provider
			*
			* This is an alternative to removing them. 
			* 
			* @param kyc_provider
			* @param state
			*/		
			[[eosio::action]]
			void blkycprov( name kyc_provider, bool state );
			using blkycprov_action = eosio::action_wrapper<"blkycprov"_n, &eosioproton::blkycprov>;

			/**
			* Remove KYC Provider
			*
			* 
			* @param kyc_provider
			*/		
 			[[eosio::action]]
			void rmvkycprov( name kyc_provider );
			using rmvkycprov_action = eosio::action_wrapper<"rmvkycprov"_n, &eosioproton::rmvkycprov>;

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
			name                                     acc;
			std::string                              name;
			std::string                              avatar;
			bool                                     verified;
			uint64_t                                 date;
			uint64_t                                 verifiedon;
			eosio::name                              verifier;

			vector<eosio::name>                      raccs;
			vector<tuple<eosio::name, eosio::name>>  aacts;
			vector<tuple<eosio::name, string>>       ac;

			vector<kyc_prov>                         kyc;
			
			uint64_t primary_key()const { return acc.value; }
		};

		typedef eosio::multi_index< "usersinfo"_n, userinfo > usersinfo;

		struct [[eosio::table]] kyc_providers_list {
			//uint64_t index
			name            kyc_provider;
			std::string     desc;
			std::string     url;
			std::string     iconurl;
			std::string     name;
			bool            blisted;
			
			uint64_t primary_key()const { return kyc_provider.value; }
		};

		typedef eosio::multi_index< "kycproviders"_n, kyc_providers_list > kycproviders;

		//add singelton for producer pay config
	};

} /// namespace eosio
