#pragma once

#include <eosio/eosio.hpp>
using namespace std;

namespace eosio
{
  class [[eosio::contract("UserSkills")]] UserSkills : public eosio::contract
  {
  public:
    using contract::contract;
    UserSkills(name receiver, name code, datastream<const char *> ds) : contract(receiver, code, ds) {}
    //?nel costruttore bisogna mettere la table

    [[eosio::action]] void updateSkill(string skill, uint64_t value);
    [[eosio::action]] uint64_t getSkillValue(string skill); //?se tornassi un iterator

    using updateSkill_action = action_wrapper<"update Skill"_n, &UserSkills::updateSkill>;
    using getSkillValue_action = action_wrapper<"get Skill Value"_n, &UserSkills::getSkillValue>;

  private:
    struct [[eosio::table]] userSkills
    {
      string name;
      uint64_t value;
      uint64_t primary_key() const { return name.value; }
    };

    typedef eosio::multi_index<"userSkills"_n, userSkills> userSkillsTable;
  };
} 