/*##################################*\
#
#
# Created by CryptoLions.io
#
#
\*##################################*/


#include <eosio.proton/eosio.proton.hpp>
#include <eosio/system.hpp>

namespace eosio {

	void eosioproton::setperm2(name acc, const std::vector<uint8_t>& perms ){

		require_auth( get_self() );
		
		require_recipient( acc );
		check( is_account( acc ), "Account does not exist.");
		
		permissions perm( get_self(), get_self().value );
		auto existing = perm.find( acc.value );

		if ( existing != perm.end() ) {

			perm.modify( existing, get_self(), [&]( auto& p ){
				p.createacc = perms[0];
				p.vote = perms[1]; 
				p.regprod = perms[2]; 
				p.regproxy = perms[3]; 
				p.setcontract = perms[4]; 
				p.namebids = perms[5]; 
				p.rex = perms[6]; 

				p.delegate = perms[7];
				p.undelegate = perms[8];
				p.sellram = perms[9];
				p.buyram = perms[10];

			});
		} else {
			perm.emplace( get_self(), [&]( auto& p ){
				p.acc = acc;
				p.createacc = perms[0];
				p.vote = perms[1]; 
				p.regprod = perms[2]; 
				p.regproxy = perms[3]; 
				p.setcontract = perms[4]; 
				p.namebids = perms[5]; 
				p.rex = perms[6]; 
				p.delegate = perms[7];
				p.undelegate = perms[8];
				p.sellram = perms[9];
				p.buyram = perms[10];
			});
		}
	}
	

	void eosioproton::setperm(name acc, const std::map<std::string,uint8_t>& perms ){
		require_auth( get_self() );

		require_recipient( acc );
		check( is_account( acc ), "Account does not exist.");

		permissions perm( get_self(), get_self().value );
		auto existing = perm.find( acc.value );

		if ( existing != perm.end() ) {
			perm.modify( existing, get_self(), [&]( auto& p ){
				for (auto it=perms.begin(); it!=perms.end(); ++it){
					if(it->first == "createacc") { p.createacc = it->second; }
					if(it->first == "vote") { p.vote = it->second; }
					if(it->first == "regprod") { p.regprod = it->second; }
					if(it->first == "regproxy") { p.regproxy = it->second; }
					if(it->first == "setcontract") { p.setcontract = it->second; }
					if(it->first == "namebids") { p.namebids = it->second; }
					if(it->first == "rex") { p.rex = it->second; }
					
					if(it->first == "delegate") { p.delegate = it->second; }
					if(it->first == "undelegate") { p.undelegate = it->second; }
					if(it->first == "sellram") { p.sellram = it->second; }
					if(it->first == "buyram") { p.buyram = it->second; }
				}
			});
		} else {
			perm.emplace( get_self(), [&]( auto& p ){
				p.acc = acc;
				p.createacc = 0;
				p.vote = 0;
				p.regprod = 0;
				p.regproxy = 0;
				p.setcontract = 0;
				p.namebids = 0;
				p.rex = 0;
				p.delegate = 0;
				p.undelegate = 0;
				p.sellram = 0;
				p.buyram = 0;
				for (auto it=perms.begin(); it!=perms.end(); ++it){
					if(it->first == "createacc") { p.createacc = it->second; }
					if(it->first == "vote") { p.vote = it->second; }
					if(it->first == "regprod") { p.regprod = it->second; }
					if(it->first == "regproxy") { p.regproxy = it->second; }
					if(it->first == "setcontract") { p.setcontract = it->second; }
					if(it->first == "namebids") { p.namebids = it->second; }
					if(it->first == "rex") { p.rex = it->second; }
					if(it->first == "delegate") { p.delegate = it->second; }
					if(it->first == "undelegate") { p.undelegate = it->second; }
					if(it->first == "sellram") { p.sellram = it->second; }
					if(it->first == "buyram") { p.buyram = it->second; }
				}
			});
		}
	}


	void eosioproton::setuserava(name acc, std::string ava ){

		if (!has_auth(acc)) {
			require_auth(permission_level("wlcm.proton"_n, "update"_n));
		}
		require_recipient( acc );
		check( is_account( acc ), "Account does not exist.");

		usersinfo usrinf( get_self(), get_self().value );
		auto existing = usrinf.find( acc.value );

		if ( existing != usrinf.end() ) {
			usrinf.modify( existing, get_self(), [&]( auto& p ){
				p.avatar = ava;
				p.date = eosio::current_time_point().sec_since_epoch();;
			});
		} else {
			usrinf.emplace( get_self(), [&]( auto& p ){
				p.acc = acc;
				p.name = "";
				p.avatar = ava;
				p.verified = false;
				p.verifiedon = 0;
				p.verifier = ""_n;
				p.date = eosio::current_time_point().sec_since_epoch();
			});
		}
	}

	void eosioproton::setusername(name acc, std::string name ){
		
		if (!has_auth(acc)) {
			require_auth(permission_level("wlcm.proton"_n, "update"_n));
		}
		require_recipient( acc );
		check( is_account( acc ), "Account does not exist.");

		usersinfo usrinf( get_self(), get_self().value );
		auto existing = usrinf.find( acc.value );

		if ( existing != usrinf.end() ) {
			usrinf.modify( existing, get_self(), [&]( auto& p ){
				p.name = name;
				p.date = eosio::current_time_point().sec_since_epoch();
				p.verified = false;
			});
		} else {
			usrinf.emplace( get_self(), [&]( auto& p ){
				p.acc = acc;
				p.name = name;
				p.avatar = "";
				p.verified = false;
				p.verifiedon = 0;
				p.verifier = ""_n;
				p.date = eosio::current_time_point().sec_since_epoch();
			});
		}
	}


	void eosioproton::userverify(name acc, name verifier, bool  verified ){

		require_auth(permission_level("admin.proton"_n, "verifiers"_n));
		require_auth(verifier);
		check( is_account( acc ), "Account does not exist.");	
		require_recipient( acc );

		usersinfo usrinf( get_self(), get_self().value );
		auto existing = usrinf.find( acc.value );

		if ( existing != usrinf.end() ) {
			check (existing->verified != verified, "This status alredy set");
			usrinf.modify( existing, get_self(), [&]( auto& p ){
				p.verified = verified;
				if ( verified ) {
					p.verifiedon = eosio::current_time_point().sec_since_epoch();
					p.verifier = verifier;
				} else  {
					p.verifiedon = 0;
					p.verifier = ""_n;
				}
				p.date = eosio::current_time_point().sec_since_epoch();
			});
		} else {
			usrinf.emplace( get_self(), [&]( auto& p ){
				p.acc = acc;
				p.name = "";
				p.avatar = "";
				p.verified = verified;

				if ( verified ) {
					p.verifiedon = eosio::current_time_point().sec_since_epoch();
					p.verifier = verifier;
				} else  {
					p.verifiedon = 0;
					p.verifier = ""_n;
				}
				p.date = eosio::current_time_point().sec_since_epoch();
			});
		}

	}

	void eosioproton::updateraccs(name acc, vector<name> raccs){

		require_auth(acc);

		require_recipient( acc );
		check( is_account( acc ), "Account does not exist.");

		usersinfo usrinf( get_self(), get_self().value );
		auto existing = usrinf.find( acc.value );
		
		for (auto i = 0; i < raccs.size(); i++) {
			check( is_account( raccs[i] ), "raccs account '" + raccs[i].to_string() + "' does not exist.");
		}

		if ( existing != usrinf.end() ) {
			usrinf.modify( existing, get_self(), [&]( auto& p ){
				p.raccs = raccs;
				p.date = eosio::current_time_point().sec_since_epoch();
			});
		} else {
			usrinf.emplace( get_self(), [&]( auto& p ){
				p.acc = acc;
				p.name = "";
				p.avatar = "";
				p.verified = false;	
				p.verifiedon = 0;
				p.verifier = ""_n;
				p.raccs = raccs;
				p.date = eosio::current_time_point().sec_since_epoch();
			});
		}	
	}

	void eosioproton::updateaacts(name acc, vector<tuple<name, name>> aacts){

		require_auth(acc);

		require_recipient( acc );
		check( is_account( acc ), "Account does not exist.");

		usersinfo usrinf( get_self(), get_self().value );
		auto existing = usrinf.find( acc.value );

		for (auto i = 0; i < aacts.size(); i++) {
			const auto &[n1, n2] = aacts[i];
			//print(n1);
			check( is_account( n1 ), "aacts account '" + n1.to_string() + "' does not exist.");
		}

		if ( existing != usrinf.end() ) {
			usrinf.modify( existing, get_self(), [&]( auto& p ){
				p.aacts = aacts;
				p.date = eosio::current_time_point().sec_since_epoch();
			});
		} else {
			usrinf.emplace( get_self(), [&]( auto& p ){
				p.acc = acc;
				p.name = "";
				p.avatar = "";
				p.verified = false;	
				p.verifiedon = 0;
				p.verifier = ""_n;
				p.aacts = aacts;
				p.date = eosio::current_time_point().sec_since_epoch();
			});
		}
	}

	void eosioproton::updateac(name acc, vector<tuple<name, string>> ac){

		require_auth(acc);
		require_recipient( acc );
		check( is_account( acc ), "Account does not exist.");

		usersinfo usrinf( get_self(), get_self().value );
		auto existing = usrinf.find( acc.value );

		for (auto i = 0; i < ac.size(); i++) {
			const auto &[n1, s1] = ac[i];
			check( is_account( n1 ), "ac account '" + n1.to_string() + "' does not exist.");	
			symbol sym = symbol(s1, 4);
			check( sym.is_valid(), "invalid symbol name " + s1 );
		}

		if ( existing != usrinf.end() ) {
			usrinf.modify( existing, get_self(), [&]( auto& p ){
				p.ac = ac;
				p.date = eosio::current_time_point().sec_since_epoch();
			});
		} else {
			usrinf.emplace( get_self(), [&]( auto& p ){
				p.acc = acc;
				p.name = "";
				p.avatar = "";
				p.verified = false;	
				p.verifiedon = 0;
				p.verifier = ""_n;
				p.ac = ac;
				p.date = eosio::current_time_point().sec_since_epoch();
			});
		}
	}

	void eosioproton::reqperm(name acc, std::string permission ){
		require_auth( acc );
		permissions perm( get_self(), get_self().value );
		auto existing = perm.find( acc.value );

		if ( existing != perm.end() ) {
			perm.modify( existing, get_self(), [&]( auto& p ){
				if(permission == "createacc" && existing->createacc != 4 && existing->createacc != 1 ) { p.createacc = 2; }
				if(permission == "vote" && existing->vote != 4 && existing->vote != 1 ) { p.vote = 2; }
				if(permission == "regprod" && existing->regprod != 4 && existing->regprod != 1 ) { p.regprod = 2; }
				if(permission == "regproxy" && existing->regproxy != 4 && existing->regproxy != 1 ) { p.regproxy = 2; }
				if(permission == "setcontract" && existing->setcontract != 4 && existing->setcontract != 1 ) { p.setcontract = 2; }
				if(permission == "namebids" && existing->namebids != 4 && existing->namebids != 1 ) { p.namebids = 2; }
				if(permission == "rex" && existing->rex != 4 && existing->rex != 1 ) { p.rex = 2; }
				if(permission == "delegate" && existing->delegate != 4 && existing->delegate != 1 ) { p.delegate = 2; }
				if(permission == "undelegate" && existing->undelegate != 4 && existing->undelegate != 1 ) { p.undelegate = 2; }
				if(permission == "sellram" && existing->sellram != 4 && existing->sellram != 1 ) { p.sellram = 2; }
				if(permission == "buyram" && existing->buyram != 4 && existing->buyram != 1 ) { p.buyram = 2; }
			});
		} else {
			perm.emplace( get_self(), [&]( auto& p ){
				p.acc = acc;
				p.createacc = 0;
				p.vote = 0;
				p.regprod = 0;
				p.regproxy = 0;
				p.setcontract = 0;
				p.namebids = 0;
				p.rex = 0;
				p.delegate = 0;
				p.undelegate = 0;
				p.sellram = 0;
				p.buyram = 0;

				if(permission == "createacc") { p.createacc = 2; }
				if(permission == "vote") { p.vote = 2; }
				if(permission == "regprod") { p.regprod = 2; }
				if(permission == "regproxy") { p.regproxy = 2; }
				if(permission == "setcontract") { p.setcontract = 2; }
				if(permission == "namebids") { p.namebids = 2; }
				if(permission == "rex") { p.rex = 2; }

				if(permission == "delegate") { p.delegate = 2; }
				if(permission == "undelegate") { p.undelegate = 2; }
				if(permission == "sellram") { p.sellram = 2; }
				if(permission == "buyram") { p.buyram = 2; }
			});
		}
	}

	void eosioproton::remove(name acc){
		require_auth( get_self() );
		require_recipient( acc );
		permissions perm( get_self(), get_self().value );
		auto existing = perm.find( acc.value );

		check ( existing != perm.end(), "Account not found." );

		perm.erase( existing );	

	}

	void eosioproton::newaccres(name account){
		eosiosystem::del_bandwidth_table del_tbl( "eosio"_n, "wlcm.proton"_n.value );
		auto itr = del_tbl.find( account.value );
		check (itr == del_tbl.end(), "Account has already received default resources");

		auto act = action(
			permission_level{ "wlcm.proton"_n, "newacc"_n },
			"eosio"_n,
			"delegatebw"_n,
			std::make_tuple(
				"wlcm.proton"_n,
				account,
				asset(10000, SYSsym), // NET
				asset(100000, SYSsym), // CPU
				0
			)
		);
		act.send();

		// Add in userinfo if not present
		usersinfo usrinf( get_self(), get_self().value );
		auto existing = usrinf.find( account.value );

		if ( existing == usrinf.end() ) {
			usrinf.emplace( get_self(), [&]( auto& p ){
				p.acc = account;
				p.name = "";
				p.avatar = "";
				p.verified = false;	
				p.verifiedon = 0;
				p.verifier = ""_n;
				p.date = eosio::current_time_point().sec_since_epoch();
			});
		}
	}
   
	void eosioproton::kickbp( name producer ){
		require_auth(permission_level("eosio"_n, "active"_n));
		check( is_account( producer ), "Account does not exist.");

		permissions perm( get_self(), get_self().value );
		auto itr = perm.find( producer.value );
		if ( itr != perm.end() ) {
			perm.modify( itr, get_self(), [&]( auto& p ){
				p.regprod = 3; 
			});
		} else {
			perm.emplace( get_self(), [&]( auto& p ){
				p.acc = producer;
				p.createacc = 0;
				p.vote = 0;
				p.regprod = 3;
				p.regproxy = 0;
				p.setcontract = 0;
				p.namebids = 0;
				p.rex = 0;
				p.delegate = 0;
				p.undelegate = 0;
				p.sellram = 0;
				p.buyram = 0;
			});	
		}
	}

	void eosioproton::addkyc( name acc, kyc_prov kyc ){
		require_auth(kyc.kyc_provider);
		
		kycproviders kps( get_self(), get_self().value );		
		auto itr_kycprov = kps.require_find(kyc.kyc_provider.value, string("KYC provider " + kyc.kyc_provider.to_string() + "  not found").c_str());		
		
		check (!itr_kycprov->blisted, "Account is blacklisted.");
				
		usersinfo usrinf( get_self(), get_self().value );
		auto itr_usrs = usrinf.require_find( acc.value, string("User " + acc.to_string() + " not found").c_str() );

		for (auto i = 0; i < itr_usrs->kyc.size(); i++) {		
			if ( itr_usrs->kyc[i].kyc_provider == kyc.kyc_provider) {		
				check (false, string("There is already approval from " + kyc.kyc_provider.to_string()).c_str());
				break;
			}						
		}
		
		usrinf.modify( itr_usrs, get_self(), [&]( auto& p ){
			p.kyc.push_back(kyc);			
		});
	}
	
	void eosioproton::updatekyc( name acc, kyc_prov kyc ){
		require_auth(kyc.kyc_provider);
		
		kycproviders kps( get_self(), get_self().value );
				
		auto itr_kycprov = kps.require_find(kyc.kyc_provider.value, string("KYC provider " + kyc.kyc_provider.to_string() + "  not found").c_str());		
		
		check (!itr_kycprov->blisted, "Account is blacklisted.");
				
		usersinfo usrinf( get_self(), get_self().value );
		auto itr_usrs = usrinf.require_find( acc.value, string("User " + acc.to_string() + " not found").c_str() );


		vector<kyc_prov> new_kyc = itr_usrs->kyc;
		bool modified = false;
		for (auto i = 0; i < new_kyc.size(); i++) {
		
			if ( new_kyc[i].kyc_provider == kyc.kyc_provider) {
				new_kyc[i].kyc_level = kyc.kyc_level;
				new_kyc[i].kyc_date = kyc.kyc_date;
				modified = true;
				break;
			}						

		}
		check (modified, string("KYC from " + kyc.kyc_provider.to_string() + " not found").c_str());
		
		usrinf.modify( itr_usrs, get_self(), [&]( auto& p ){
			p.kyc = new_kyc;			
		});
	}	

	void eosioproton::removekyc( name acc, name kyc_provider ){
		require_auth(kyc_provider);		
		kycproviders kps( get_self(), get_self().value );
		
		auto itr_kycprov = kps.require_find(kyc_provider.value, string("KYC provider not found").c_str());		
		
		check (!itr_kycprov->blisted, "Account is blacklisted.");

		usersinfo usrinf( get_self(), get_self().value );
		auto itr_usrs = usrinf.require_find( acc.value, string("User " + acc.to_string() + " not found").c_str() );
		
		vector<kyc_prov> new_kyc = itr_usrs->kyc;
		bool removed = false;
		
		for (auto i = 0; i < new_kyc.size(); i++) {	
			if ( new_kyc[i].kyc_provider == kyc_provider) {
				new_kyc.erase(new_kyc.begin() + i);
				removed = true;
				break;
			}
		}
		
		check (removed, string("KYC from " + kyc_provider.to_string() + " not found").c_str());
		
		usrinf.modify( itr_usrs, get_self(), [&]( auto& p ){
			p.kyc = new_kyc;			
		});
	}

	void eosioproton::addkycprov( name kyc_provider, std::string desc, std::string url, std::string iconurl, std::string name ){		
		require_auth(permission_level("admin.proton"_n, "committee"_n));
		//require_auth(permission_level("admin.proton"_n, "light"_n));
		
		require_recipient( kyc_provider );
		check( is_account( kyc_provider ), "Account does not exist.");

		kycproviders kps( get_self(), get_self().value );
		auto itr_kycprov = kps.find( kyc_provider.value );

		if ( itr_kycprov != kps.end() ) {
			kps.modify( itr_kycprov, get_self(), [&]( auto& p ){			
				p.desc = desc;
				p.url = url;
				p.iconurl = iconurl;
				p.name = name;				
			});
		} else {
			kps.emplace( get_self(), [&]( auto& p ){				
				p.kyc_provider = kyc_provider;
				p.desc = desc;
				p.url = url;
				p.iconurl = iconurl;
				p.name = name;
				p.blisted = false;
			});
		}	
	}

	void eosioproton::blkycprov( name kyc_provider, bool state ){
		require_auth(permission_level("admin.proton"_n, "committee"_n));			
		//require_auth(permission_level("admin.proton"_n, "light"_n));
		
		kycproviders kps( get_self(), get_self().value );
		auto itr_kycprov = kps.require_find(kyc_provider.value, string("KYC provider not found").c_str());
		
		kps.modify( itr_kycprov, get_self(), [&]( auto& p ){			
			p.blisted = state;
		});
	}

	void eosioproton::rmvkycprov( name kyc_provider ){
		require_auth(permission_level("admin.proton"_n, "committee"_n));	
		//require_auth(permission_level("admin.proton"_n, "light"_n));
		
		kycproviders kps( get_self(), get_self().value );
		auto itr_kycprov = kps.require_find(kyc_provider.value, string("KYC provider not found").c_str());		
		kps.erase( itr_kycprov );
	}


}

EOSIO_DISPATCH( eosio::eosioproton, (setperm)(setperm2)(remove)(reqperm)(setusername)(setuserava)(userverify)(newaccres)(updateraccs)(updateaacts)(updateac)(kickbp)(addkyc)(updatekyc)(removekyc)(addkycprov)(rmvkycprov)(blkycprov))
