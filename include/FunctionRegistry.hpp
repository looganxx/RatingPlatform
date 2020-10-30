#pragma once

#include <eosio/eosio.hpp>
using namespace std;

namespace eosio
{
  class [[eosio::contract("FunctionRegistry")]] FunctionRegistry : public eosio::contract
  {
  public:
    using contract::contract;
    FunctionRegistry(name receiver, name code, datastream<const char *> ds) : contract(receiver, code, ds) {}

    [[eosio::action]] void getFunction(id);
    [[eosio::action]] void pushFunction(function, id);

    using getFunction_action = action_wrapper<"get Function"_n, &FunctionRegistry::getFunction>;
    using pushFunction_action = action_wrapper<"push Function"_n, &FunctionRegistry::pushFunction>;

  private:
    struct [[eosio::table]] availableSkills
    {
      std name;
      //TODO: altri parametri relativi
      uint64_t primary_key() const { return name.value; }
    };

    typedef eosio::multi_index<"availableSkills"_n, availableSkills> availableSkillsTable;
  };
} 