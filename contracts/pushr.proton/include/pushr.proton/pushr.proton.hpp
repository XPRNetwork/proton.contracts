#pragma once

#include <eosio/asset.hpp>
#include <eosio/crypto.hpp>
#include <eosio/eosio.hpp>
#include <eosio/fixed_bytes.hpp>
#include <eosio/singleton.hpp>
#include <eosio/system.hpp>
#include <eosio/time.hpp>
#include <eosio/transaction.hpp>

#include <string>

namespace eosiosystem {
    class system_contract;
}

namespace eosio {

using std::string;
using std::tuple;
using std::vector;

class [[eosio::contract("pushr.proton")]] pushrproton : public contract {

public:
  using contract::contract;

  [[eosio::action]] void pushreq(eosio::name requestor, eosio::name signer,
                                 const std::vector<char> &esr);

  typedef eosio::name account_name;
  typedef eosio::name action_name;
  typedef eosio::name permission_name;
  typedef uint8_t chain_alias;
  typedef uint8_t request_flags;
  typedef eosio::checksum256 chain_id;
  typedef std::variant<chain_alias, chain_id> variant_id;

  struct info_pair_st {
    string key;
    vector<char> value;

    EOSLIB_SERIALIZE(info_pair_st, (key)(value))
  };

  struct permission_level_st {
    account_name actor;
    permission_name permission;

    EOSLIB_SERIALIZE(permission_level_st, (actor)(permission))
  };

  struct action_st {
    account_name account;
    action_name name;
    vector<permission_level> authorization;
    vector<char> data;

    EOSLIB_SERIALIZE(action_st, (account)(name)(authorization)(data))
  };

  struct identity_st {
    permission_level permission;

    EOSLIB_SERIALIZE(identity_st, (permission))
  };

  typedef std::variant<action_st, vector<action_st>, eosio::transaction, identity_st>
      variant_req;

  struct signing_request_st {
    variant_id chain_id;
    variant_req req;
    request_flags flags;
    string callback;
    vector<info_pair_st> info;

    EOSLIB_SERIALIZE(signing_request_st, (chain_id)(req)(flags)(callback)(info))
  };

private:

  // External tables

  struct [[eosio::table]] userinfo {
    eosio::name acc;
    string name;
    string avatar;
    bool verified;
    uint64_t date;
    uint64_t verifiedon;
    eosio::name verifier;

    vector<eosio::name> raccs;
    vector<tuple<eosio::name, eosio::name>> aacts;
    vector<tuple<eosio::name, string>> ac;

    uint64_t primary_key() const { return acc.value; }
  };

  typedef eosio::multi_index<"usersinfo"_n, userinfo> usersinfo;

  void check_action(eosio::pushrproton::usersinfo::const_iterator requestor_it,
                    eosio::name requestor, eosio::name signer,
                    action_st * action);
};

} // namespace eosio
