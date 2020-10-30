#pragma once

#include <eosio/eosio.hpp>
using namespace std;

namespace eosio
{
  class [[eosio::contract("DatabaseSkills")]] DatabaseSkills : public eosio::contract
  {
  public:
    using contract::contract;
    DatabaseSkills(name receiver, name code, datastream<const char *> ds) : contract(receiver, code, ds) {}

    [[eosio::action]] void addSkill(string skill);
    [[eosio::action]] Vector<> getSkills(); //?se tornassi un iterator

    using addSkill_action = action_wrapper<"add Skill"_n, &DatabaseSkills::addSkill>;
    using getSkill_action = action_wrapper<"get Skills"_n, &DatabaseSkills::getSkills>;

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