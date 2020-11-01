#pragma once

#include <eosio/eosio.hpp>

namespace eosio{
  class [[eosio::contract("RatingSystemFramework")]] RatingSystemFramework : public eosio::contract
  {

  public:
    using contract::contract;

    RatingSystemFramework(name receiver, name code, datastream<const char *> ds, 
      DatabaseSkills databaseSkill, FunctionRegistry functionRegistry) : contract(receiver, code, ds),
       databaseSkill(new DatabaseSkill()), functionRegistry(new FunctionRegistry()) {}

    //basic
    [[eosio::action]] void createUser(const name& user);
    [[eosio::action]] void deleteUser(const name& user);

    //da DatabaseSkills
    [[eosio::action]] void addSkill(const string &skill);
    [[eosio::action]] auto getSkills(); //?se tornassi un iterator

    //da FunctionRegistry
    [[eosio::action]] void getFunction(id);
    [[eosio::action]] void pushFunction(function, id);

    using createUser_action = action_wrapper<"create user"_n, &RatingSystemFramework::createUser>;
    using deleteUser_action = action_wrapper<"delete user"_n, &RatingSystemFramework::deleteUser>;

    using addSkill_action = action_wrapper<"add Skill"_n, &RatingSystemFramework::addSkill>;
    using getSkill_action = action_wrapper<"get Skills"_n, &RatingSystemFramework::getSkills>;

    using getFunction_action = action_wrapper<"get Function"_n, &RatingSystemFramework::getFunction>;
    using pushFunction_action = action_wrapper<"push Function"_n, &RatingSystemFramework::pushFunction>;

  private:
    struct [[eosio::table]] users
    {
      name username;
      vector<string> skill;
      //* in C posso mettere una struct all'interno di una struct
      //* posso farlo qui? come la gestisco?
      //vector<string> items;

      uint64_t primary_key() const { return username.value; }
    };


    struct [[eosio::table]] availableSkills
    {
      std name;
      uint64_t primary_key() const { return name.value; }
    };


    struct [[eosio::table]] function
    {
      std name;
      //TODO: altri parametri relativi
      uint64_t primary_key() const { return name.value; }
    };



    typedef eosio::multi_index<"users"_n, users> usersTable;
    typedef eosio::multi_index<"availableSkills"_n, availableSkills> availableSkillsTable;
    typedef eosio::multi_index<"function"_n, function> functionTable;

  };
}