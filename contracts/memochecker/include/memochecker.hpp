#include <eosio/eosio.hpp>
#include <eosio/crypto.hpp>
#include <eosio/transaction.hpp>
#include <eosio/asset.hpp>
#include <eosio/symbol.hpp>

using namespace eosio;
using namespace std;

const vector<string> default_memo = { "Stake", "Unstake", "stake bandwidth", "unstake XPR" };

CONTRACT memochecker : public contract{
   public:
	  using contract::contract;

	  ACTION addmemo(const string& memo);
	  using addmemo_action = action_wrapper<"addmemo"_n, &memochecker::addmemo>;

	  ACTION delmemo(const string& memo);
	  using delmemo_action = action_wrapper<"delmemo"_n, &memochecker::delmemo>;

	  ACTION delid(uint64_t id);
	  using delid_action = action_wrapper<"delid"_n, &memochecker::delid>;

	  [[eosio::on_notify("*::transfer")]] void ontransfer(name from, name to, asset quantity, string memo);

	  checksum256 make_hash(const string& memo);
	  optional<uint64_t> get_memo_id(const string& memo);

	  void add_default_memos(); 


public:
	  TABLE memoholder{
		uint64_t		id;
		string			memo;
		checksum256		hash;

		auto primary_key() const {
			return id;
		}

		checksum256 by_hash() const {
			return hash;
		}

	  void set_memo(const string& str_memo)
	  {
		  hash = sha256(str_memo.c_str(), str_memo.size());
		  memo = str_memo;
	  }

	  };
	  typedef eosio::multi_index< "memoholders"_n, memoholder,
		  eosio::indexed_by< "hash"_n, eosio::const_mem_fun<memoholder, checksum256, &memoholder::by_hash>>
	  > memoholders;

	  memoholders memoholders_ = { _self, _self.value };
};