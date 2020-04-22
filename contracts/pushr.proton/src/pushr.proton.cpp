#include <pushr.proton/pushr.proton.hpp>

namespace eosio {

void pushrproton::pushreq(eosio::name requestor, eosio::name signer,
                          const std::vector<char> &esr) {

  require_auth(requestor);
  check(is_account(signer), "Signer must be a valid account");

  usersinfo usersinfo_t("eosio.proton"_n, "eosio.proton"_n.value);
  auto requestor_it = usersinfo_t.find(requestor.value);

  check(requestor_it != usersinfo_t.end(),
        "Requestor's account not found in usersinfo table");
  check(requestor_it->verified == true, "Requestor must be verified");

  auto esr_st = eosio::unpack<signing_request_st>(esr);
  check(esr_st.info.size() > 0, "Expected info pair");

  bool info_account_found = false;
  for (const info_pair_st inf : esr_st.info) {
    if (inf.key == "account") {
      std::string acc(inf.value.begin(), inf.value.end());
      if (acc == requestor.to_string()) {
        info_account_found = true;
        break;
      }
    }
  }

  check(info_account_found, "Expected account: name in info options");

  auto *actions = std::get_if<vector<action_st>>(&esr_st.req);
  auto *action = std::get_if<action_st>(&esr_st.req);

  check(actions != nullptr || action != nullptr,
        "Expected action or action[] type esr");

  if (actions != nullptr) {
    for (action_st act : *actions) {
      check_action(requestor_it, requestor, signer, &act);
    }
  } else if (action != nullptr) {
    check_action(requestor_it, requestor, signer, action);
  }
}

void pushrproton::check_action(
    eosio::pushrproton::usersinfo::const_iterator requestor_it,
    eosio::name requestor, eosio::name signer, action_st *action) {

  auto acttuple = std::make_tuple(action->account, action->name);
  check(std::find(requestor_it->aacts.begin(), requestor_it->aacts.end(),
                  acttuple) != requestor_it->aacts.end(),
        "Action passed not in accounts aacts");

  if (action->account == "eosio"_n) {
    check(action->name != "updateauth"_n, "updateauth action not supported");
    check(action->name != "deleteauth"_n, "deleteauth action not supported");
    check(action->name != "linkauth"_n, "linkauth action not supported");
    check(action->name != "unlinkauth"_n, "unlinkauth action not supported");
  }
}

} // namespace eosio

EOSIO_DISPATCH(eosio::pushrproton, (pushreq))