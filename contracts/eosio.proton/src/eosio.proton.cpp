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
		
		internal_use_do_not_use::require_auth2("wlcm.proton"_n.value, "update"_n.value );
		//require_auth(acc);

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
				p.date = eosio::current_time_point().sec_since_epoch();;
				p.data = "{}";
				p.primary = 0;
			});
		}

	}
	
		
	void eosioproton::setuserdata(name acc, std::string data ){

		internal_use_do_not_use::require_auth2("wlcm.proton"_n.value, "update"_n.value );
		//require_auth(acc);

		require_recipient( acc );
		check( is_account( acc ), "Account does not exist.");

		usersinfo usrinf( _self, _self.value );
		auto existing = usrinf.find( acc.value );
		
		if ( existing != usrinf.end() ) {
			usrinf.modify( existing, _self, [&]( auto& p ){
				p.data = data;
				p.date = eosio::current_time_point().sec_since_epoch();;
			});
		} else {
			usrinf.emplace( _self, [&]( auto& p ){				
				p.acc = acc;
				p.name = "";
				p.avatar = "";
				p.verified = 0;
				p.date = eosio::current_time_point().sec_since_epoch();;			
				p.data = data;			
				p.primary = 0;
			});
		}
	}
	

	void eosioproton::setusername(name acc, std::string name ){

		internal_use_do_not_use::require_auth2("wlcm.proton"_n.value, "update"_n.value );
		//require_auth(acc);

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
				p.date = eosio::current_time_point().sec_since_epoch();;			
				p.data = "{}";
				p.primary = 0;
			});
		}

	}	
	
	
	
	
	void eosioproton::userverify(name acc, bool  verified ){

		internal_use_do_not_use::require_auth2("wlcm.proton"_n.value, "newacc"_n.value );
		//require_auth( _self );
		//require_auth(acc);

		require_recipient( acc );
		check( is_account( acc ), "Account does not exist.");
		

		usersinfo usrinf( _self, _self.value );
		auto existing = usrinf.find( acc.value );
		
		if ( existing != usrinf.end() ) {
			usrinf.modify( existing, _self, [&]( auto& p ){
				p.verified = verified;
				p.date = eosio::current_time_point().sec_since_epoch();;
			});
		} else {
			usrinf.emplace( _self, [&]( auto& p ){				
				p.acc = acc;
				p.name = "";				
				p.avatar = "";
				p.verified = verified;
				p.date = eosio::current_time_point().sec_since_epoch();;			
				p.data = "{}";
				p.primary = 0;
			});
		}

	}	


	void eosioproton::userprimary(name acc, bool  primary ){

		internal_use_do_not_use::require_auth2("wlcm.proton"_n.value, "newacc"_n.value );
		//require_auth( _self );
		//require_auth(acc);

		require_recipient( acc );
		check( is_account( acc ), "Account does not exist.");
		

		usersinfo usrinf( _self, _self.value );
		auto existing = usrinf.find( acc.value );
		
		if ( existing != usrinf.end() ) {
			usrinf.modify( existing, _self, [&]( auto& p ){
				p.primary = primary;
				p.date = eosio::current_time_point().sec_since_epoch();;
			});
		} else {
			usrinf.emplace( _self, [&]( auto& p ){				
				p.acc = acc;
				p.name = "";				
				p.avatar = "";
				p.verified = 0;
				p.date = eosio::current_time_point().sec_since_epoch();;			
				p.data = "{}";
				p.primary = primary;
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

    void eosioproton::migration(name acc, std::string name, std::string avatar, bool verified, uint64_t date, std::string data, bool primary) {
        require_auth( _self );
		
		usersinfo usersinfo(_self, _self.value);

        usersinfo.emplace(_self, [&](auto& s) {
            s.acc      = acc;
            s.name     = name;
            s.avatar   = avatar;
            s.verified = verified;
            s.date     = date;
            s.data     = data;
			s.primary  = primary;
        });		
	}
	
	void eosioproton::setdappconf(uint64_t ram, uint64_t cpu, uint64_t net){
		
		require_auth( _self );
		
		dappconfig dconfig(_self, _self.value);
		_dcstate = dconfig.exists() ? dconfig.get() : dappconf{};
		
		check (ram > 0 || cpu > 0 || net > 0, "Action values should be positive numbers");
		check (ram < uint64_t(16*1024*1024)*1014 , "To much RAM");
		
		_dcstate.dappramm = ram;
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

		auto addRam = _dcstate.dappramm - ram;
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
                std::make_tuple( "wlcm.proton"_n,
                        account,
                        addRam )
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






EOSIO_DISPATCH( eosio::eosioproton, (setperm)(setperm2)(remove)(reqperm)(setusername)(setuserdata)(setuserava)(userverify)(userprimary)(migration)(dappreg)(setdappconf))