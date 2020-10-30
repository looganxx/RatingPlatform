#pragma once

#include <eosio/eosio.hpp>

namespace eosio{
  class [[eosio::contract("RatingSystemFramework")]] RatingSystemFramework : public eosio::contract
  {
  private:
    DatabaseSkills databaseSkill;
    FunctionRegistry functionRegistry;

  public:
    using contract::contract;

    RatingSystemFramework(name receiver, name code, datastream<const char *> ds, 
      DatabaseSkills databaseSkill, FunctionRegistry functionRegistry) : contract(receiver, code, ds),
       databaseSkill(new DatabaseSkill()), functionRegistry(new FunctionRegistry()) {}

    [[eosio::action]] void createUser(name user);
    [[eosio::action]] void deleteUser(name user);

    using createUser_action = action_wrapper<"create user"_n, &RatingSystemFramework::createUser>;
    using deleteUser_action = action_wrapper<"delete user"_n, &RatingSystemFramework::deleteUser>;

    struct [[eosio::table]] users
    {
        name key;
        //?istanza utente
        uint64_t primary_key() const { return key.value; }
    };

    typedef eosio::multi_index<"users"_n, users> usersTable;

  };
}