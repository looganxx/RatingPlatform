#pragma once

#include <eosio/eosio.hpp>

namespace eosio
{
  class [[eosio::contract("User")]] User : public eosio::contract
  {
  public:
      using contract::contract;
      //*lista di item
      //*lista di skills
      User(name receiver, name code, datastream<const char *> ds) : contract(receiver, code, ds) {}

      [[eosio::action]] void createItem(name name);
      [[eosio::action]] void addRate( //tipo item
        uint64_t score);
      [[eosio::action]] void payItem( //tipo item
          const asset& amount);
      [[eosio::action]] void withdraw();

      using createItem_action = action_wrapper<"create item"_n, &User::createItem>;
      using addRate_action = action_wrapper<"add rate"_n, &User::addRate>;
      using payItem_action = action_wrapper<"pay item"_n, &User::payItem>;
      using withdraw_action = action_wrapper<"with draw"_n, &User::withdraw>;

    private:
      struct [[eosio::table]] skills
      {
          name skill;
          //TODO: altri parametri relativi
          uint64_t primary_key() const { return skill.value; }
      };

      //? Inglobare UserSkills??
      struct [[eosio::table]] items
      {
        name item;
        //TODO: altri parametri relativi 
        uint64_t primary_key() const { return item.value; }
      };

      typedef eosio::multi_index<"skills"_n, skills> skillsTable;
      typedef eosio::multi_index<"items"_n, items> itemsTable;
  };
}