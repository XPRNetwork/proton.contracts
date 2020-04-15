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

		require_auth( _self );
		
		require_recipient( acc );
		check( is_account( acc ), "Account does not exist.");
		

		permissions perm( _self, _self.value );
		auto existing = perm.find( acc.value );

		if ( existing != perm.end() ) {

			perm.modify( existing, _self, [&]( auto& p ){
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
			perm.emplace( _self, [&]( auto& p ){
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
		require_auth( _self );

		require_recipient( acc );
		check( is_account( acc ), "Account does not exist.");

		permissions perm( _self, _self.value );
		auto existing = perm.find( acc.value );

		if ( existing != perm.end() ) {
			perm.modify( existing, _self, [&]( auto& p ){
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
			perm.emplace( _self, [&]( auto& p ){
				p.acc = acc;
				p.createacc = 0;
				p.vote = 0;
				p.regprod = 0;
				p.regproxy = 0;
				p.setcontract = 0;
				p.namebids = 0;
				p.rex = 0;
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
				//p.createacc = perms.find("createacc")->second ? perms.find("createacc")->second : 0;
				//p.vote = perms.find("vote")->second ? perms.find("vote")->second : 0;
				//p.regprod = perms.find("regprod")->second ? perms.find("regprod")->second : 0;
				//p.regproxy = perms.find("regproxy")->second ? perms.find("regproxy")->second : 0;
				//p.setcontract = perms.find("setcontract")->second ? perms.find("setcontract")->second : 0;
				//p.namebids = perms.find("namebids")->second ? perms.find("namebids")->second : 0;
				//p.rex = perms.find("rex")->second ? perms.find("rex")->second : 0;
			});
		}
	}


	void eosioproton::setuserava(name acc, std::string ava ){

		require_auth(permission_level("wlcm.proton"_n, "update"_n));
		require_recipient( acc );
		check( is_account( acc ), "Account does not exist.");

		usersinfo usrinf( _self, _self.value );
		auto existing = usrinf.find( acc.value );

		if ( existing != usrinf.end() ) {
			usrinf.modify( existing, _self, [&]( auto& p ){
				p.avatar = ava;
				p.date = eosio::current_time_point().sec_since_epoch();;
			});
		} else {
			usrinf.emplace( _self, [&]( auto& p ){
				p.acc = acc;
				p.name = "";
				p.avatar = ava;
				p.verified = 0;
				p.verifiedon = 0;
				p.verifier = ""_n;
				p.date = eosio::current_time_point().sec_since_epoch();
			});
		}
	}

	void eosioproton::setusername(name acc, std::string name ){
		require_auth(permission_level("wlcm.proton"_n, "update"_n));
		require_recipient( acc );
		check( is_account( acc ), "Account does not exist.");

		usersinfo usrinf( _self, _self.value );
		auto existing = usrinf.find( acc.value );

		if ( existing != usrinf.end() ) {
			usrinf.modify( existing, _self, [&]( auto& p ){
				p.name = name;
				p.date = eosio::current_time_point().sec_since_epoch();;
			});
		} else {
			usrinf.emplace( _self, [&]( auto& p ){
				p.acc = acc;
				p.name = name;
				p.avatar = "";
				p.verified = 0;
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

		usersinfo usrinf( _self, _self.value );
		auto existing = usrinf.find( acc.value );

		if ( existing != usrinf.end() ) {
			check (existing->verified != verified, "This status alredy set");
			usrinf.modify( existing, _self, [&]( auto& p ){
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
			usrinf.emplace( _self, [&]( auto& p ){
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

		usersinfo usrinf( _self, _self.value );
		auto existing = usrinf.find( acc.value );
		
		for (auto i = 0; i < raccs.size(); i++) {
			check( is_account( raccs[i] ), "raccs account '" + raccs[i].to_string() + "' does not exist.");
		}

		if ( existing != usrinf.end() ) {
			usrinf.modify( existing, _self, [&]( auto& p ){
				p.raccs = raccs;
				p.date = eosio::current_time_point().sec_since_epoch();
			});
		} else {
			usrinf.emplace( _self, [&]( auto& p ){
				p.acc = acc;
				p.name = "";
				p.avatar = "";
				p.verified = 0;	
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

		usersinfo usrinf( _self, _self.value );
		auto existing = usrinf.find( acc.value );

		for (auto i = 0; i < aacts.size(); i++) {
			const auto &[n1, n2] = aacts[i];
			//print(n1);
			check( is_account( n1 ), "aacts account '" + n1.to_string() + "' does not exist.");
		}

		if ( existing != usrinf.end() ) {
			usrinf.modify( existing, _self, [&]( auto& p ){
				p.aacts = aacts;
				p.date = eosio::current_time_point().sec_since_epoch();
			});
		} else {
			usrinf.emplace( _self, [&]( auto& p ){
				p.acc = acc;
				p.name = "";
				p.avatar = "";
				p.verified = 0;	
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

		usersinfo usrinf( _self, _self.value );
		auto existing = usrinf.find( acc.value );

		for (auto i = 0; i < ac.size(); i++) {
			const auto &[n1, s1] = ac[i];
			//print(n1);
			check( is_account( n1 ), "ac account '" + n1.to_string() + "' does not exist.");
			
			symbol sym = symbol(s1, 4);
			check( sym.is_valid(), "invalid symbol name " + s1 );
		}

		if ( existing != usrinf.end() ) {
			usrinf.modify( existing, _self, [&]( auto& p ){
				p.ac = ac;
				p.date = eosio::current_time_point().sec_since_epoch();
			});
		} else {
			usrinf.emplace( _self, [&]( auto& p ){
				p.acc = acc;
				p.name = "";
				p.avatar = "";
				p.verified = 0;	
				p.verifiedon = 0;
				p.verifier = ""_n;
				p.ac = ac;
				p.date = eosio::current_time_point().sec_since_epoch();
			});
		}
	}

	void eosioproton::reqperm(name acc, std::string permission ){
		require_auth( acc );
		permissions perm( _self, _self.value );
		auto existing = perm.find( acc.value );

		if ( existing != perm.end() ) {
			perm.modify( existing, _self, [&]( auto& p ){
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
			perm.emplace( _self, [&]( auto& p ){
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
		require_auth( _self );
		require_recipient( acc );
		permissions perm( _self, _self.value );
		auto existing = perm.find( acc.value );

		check ( existing != perm.end(), "Account not found." );

		perm.erase( existing );	

	}


	void eosioproton::setdappconf(uint64_t ram, uint64_t cpu, uint64_t net){
		require_auth( _self );
		dappconfig dconfig(_self, _self.value);
		_dcstate = dconfig.exists() ? dconfig.get() : dappconf{};

		check (ram > 0 || cpu > 0 || net > 0, "Action values should be positive numbers");
		check (ram < uint64_t(16*1024*1024)*1014 , "To much RAM");

		_dcstate.dappram = ram;
		_dcstate.dappcpu = cpu;
		_dcstate.dappnet = net;

		dconfig.set( _dcstate, _self );
	}

	void eosioproton::dappreg(name account){
		//Set setcode permission
		//Buys up to 2MB RAM
		//CPU up to 20
		//Net up to 20

		require_auth( account );

		permissions perm( _self, _self.value );
		auto uperm = perm.find( account.value );

		if ( uperm != perm.end() ) {
			check ( uperm->setcontract != 4 , "Sorry, account banned." );
			//check ( uperm->setcontract != 1, "Already registered" );

			perm.modify( uperm, _self, [&]( auto& p ){
				 p.setcontract = 1;
			});
		} else {
			perm.emplace( _self, [&]( auto& p ){
				p.acc = account;
				p.createacc = 0;
				p.vote = 0;
				p.regprod = 0;
				p.regproxy = 0;
				p.setcontract = 1;
				p.namebids = 0;
				p.rex = 0;
				p.delegate = 0;
				p.undelegate = 0;
				p.sellram = 0;
				p.buyram = 0;
			});
		}

		user_resources_table  userres( "eosio"_n, account.value );
		auto ures = userres.find( account.value );
		check ( ures != userres.end(), "Account not found." );

		dappconfig dconfig(_self, _self.value);
		if (dconfig.exists()){
			dconfig.get();
		} else {
			dappconf{};
			dconfig.set( _dcstate, _self );
		}

		asset cpu = ures->cpu_weight;
		asset net = ures->net_weight;
		uint64_t ram = ures->ram_bytes;

		//print("cpu: ", cpu, " net: ", net, " ram: ", ram);
		uint64_t addCpu = 0;
		uint64_t addNet = 0;

		auto addRam = _dcstate.dappram - ram;
		if (_dcstate.dappcpu > cpu.amount)
			addCpu = _dcstate.dappcpu - cpu.amount;
		if (_dcstate.dappnet > net.amount)
			addNet = _dcstate.dappnet - net.amount;

		if (addRam > 10){
			//SEND_INLINE_ACTION( eosio::eosio, buyrambytes, { {_self, "active"_n} }, { _self, account, 2 * 1024 * 1024 } );
			auto act = action(
				permission_level{ "wlcm.proton"_n, "newacc"_n },
				"eosio"_n,
				"buyrambytes"_n,
				std::make_tuple( "wlcm.proton"_n, account, addRam )
			);
			act.send();
		}

		if (addCpu + addNet > 0) {
			auto act = action(
				permission_level{ "wlcm.proton"_n, "newacc"_n },
				"eosio"_n,
				"delegatebw"_n,
				std::make_tuple( "wlcm.proton"_n,
					account,
					asset(addNet, symbol("SYS", 4)),
					asset(addCpu, symbol("SYS", 4)),
					0 )
			);
			act.send();
		}
	}
}

EOSIO_DISPATCH( eosio::eosioproton, (setperm)(setperm2)(remove)(reqperm)(setusername)(setuserava)(userverify)(dappreg)(setdappconf)(updateraccs)(updateaacts)(updateac))