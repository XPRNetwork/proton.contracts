#include <memochecker.hpp>

optional<uint64_t> memochecker::get_memo_id(const string& memo)
{
	optional<uint64_t> result = nullopt;

	auto index_hash = memoholders_.get_index<"hash"_n>();
	auto it = index_hash.find(make_hash(memo));

	if (it != index_hash.end())
	{
		result = optional<uint64_t>(it->id);
	}

	return result;
}

ACTION memochecker::addmemo(const string& memo) {
	require_auth(get_self());

	auto id = get_memo_id(memo);
	check(!(id.has_value() == true), "memo already exist at record id: " + to_string((id.has_value() == true)?id.value():0));
	
	memoholders_.emplace(_self, [&](auto& s) {
		s.id = memoholders_.available_primary_key();
		s.set_memo(memo);
	});
}

ACTION memochecker::delid(uint64_t id) {
	require_auth(get_self());

	auto it = memoholders_.require_find(id, string("id " +  to_string(id) + " does not exist").c_str());
	memoholders_.erase(it);
}

checksum256 memochecker::make_hash(const string& memo)
{
	return sha256(memo.c_str(), memo.size());
}

ACTION memochecker::delmemo(const string& memo) {
	require_auth(get_self());

	auto id = get_memo_id(memo);
	check(!(id.has_value() == false), "memo does not exist at memoholder table");

	auto it = memoholders_.require_find(id.value(), string("id " + to_string(id.value()) + " does not exist").c_str());
	memoholders_.erase(it);
}

void memochecker::ontransfer(name from, name to, asset quantity, string memo)
{
	if (to != get_self() || from == get_self()) {
		return;
	}

	require_auth(from);


	add_default_memos();
	//const auto token_contract = get_first_receiver();

	auto id = get_memo_id(memo);
	check(!(id.has_value() == false), "memo does not exist at memoholder table");
}

void memochecker::add_default_memos()
{
	if (memoholders_.begin() == memoholders_.end())
	{ 
		for (const auto & memo : default_memo)
		{
			memoholders_.emplace(_self, [&](auto& s) {
				s.id = memoholders_.available_primary_key();
				s.set_memo(memo);
			});
		}
	}
}