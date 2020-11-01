#pragma once

#include <eosio/eosio.hpp>

using namespace std;

namespace eosio{
  class [[eosio::contract("RatingSystem")]] RatingSystem : public eosio::contract
  {

  public:
    using contract::contract;

    RatingSystem(name receiver, name code, datastream<const char *> ds) : contract(receiver, code, ds){}

    //basic
    [[eosio::action]] void newuser(const name& user);
    [[eosio::action]] void deluser(const name& user);
    //da DatabaseSkills
    [[eosio::action]] void addskill(const string &skill);
    [[eosio::action]] void getskills(); //?se tornassi un iterator

    //da FunctionRegistry
    //[[eosio::action]] void getFunction(id);
    //[[eosio::action]] void pushFunction(function, id);

    using createUser_action = action_wrapper<"newuser"_n, &RatingSystem::newuser>;
    using deleteUser_action = action_wrapper<"deluser"_n, &RatingSystem::deluser>;

    using addSkill_action = action_wrapper<"addskill"_n, &RatingSystem::addskill>;
    using getSkill_action = action_wrapper<"getskills"_n, &RatingSystem::getskills>;

    //using getFunction_action = action_wrapper<"get_Function"_n, &RatingSystem::getFunction>;
    //using pushFunction_action = action_wrapper<"push_Function"_n, &RatingSystem::pushFunction>;

  private:
    struct [[eosio::table]] users
    {
      name username;
      vector<string> skill;
      //vector<string> items;

      uint64_t primary_key() const { return username.value; }
    };


    struct [[eosio::table]] availableSkills
    {
      name name;
      uint64_t primary_key() const { return name.value; }
    };


    struct [[eosio::table]] function
    {
      name name;
      //TODO: altri parametri relativi
      uint64_t primary_key() const { return name.value; }
    };



    typedef eosio::multi_index<"users"_n, users> usersTable;
    typedef eosio::multi_index<"skills"_n, availableSkills> skillsTable;
    typedef eosio::multi_index<"function"_n, function> functionTable;

  };
}