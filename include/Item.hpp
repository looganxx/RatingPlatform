#pragma once

#include <eosio/eosio.hpp>

namespace eosio
{
  class [[eosio::contract("Item")]] Item : public eosio::contract
  {
  public:
    using contract::contract;
    Item(name receiver, name code, datastream<const char *> ds) : contract(receiver, code, ds) {}

    [[eosio::action]] void grantPermission(name to);
    [[eosio::action]] void grantPermission(name to, const asset& amount);
    [[eosio::action]] void revokePermission(name to);
    [[eosio::action]] void addRate(uint64_t score);
    [[eosio::action]] void computeScore(); //!input: contratto rating function
    [[eosio::action]] void successfullPayement(const asset& amount ); //! altro parametro: discount

    using createItem_action = action_wrapper<"create item"_n, &Item::createItem>;
    using addRate_action = action_wrapper<"add rate"_n, &Item::addRate>;
    using payItem_action = action_wrapper<"pay item"_n, &Item::payItem>;
    using withdraw_action = action_wrapper<"with draw"_n, &Item::withdraw>;

  private:
    
    /*
     * struct premissionMap 
     * paymentMAp
     * ratings
     * property
     * tokenContract
     */
  };
} 