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

    using newuser_action = action_wrapper<"newuser"_n, &RatingSystem::newuser>;
    using deluser_action = action_wrapper<"deluser"_n, &RatingSystem::deluser>;

    using additem_action = action_wrapper<"additem"_n, &RatingSystem::additem>;
    using delitem_action = action_wrapper<"delitem"_n, &RatingSystem::delitem>;

    using addSkill_action = action_wrapper<"addskill"_n, &RatingSystem::addskill>;
    using getSkill_action = action_wrapper<"getskills"_n, &RatingSystem::getskills>;

    //using getFunction_action = action_wrapper<"get_Function"_n, &RatingSystem::getFunction>;
    //using pushFunction_action = action_wrapper<"push_Function"_n, &RatingSystem::pushFunction>;

    /*static const struct item_s
    {
      name item;
      uint64_t rating;
    } item;*/

  private:
    struct [[eosio::table]] users
    {
      name uname; //*PK

      uint64_t primary_key() const { return uname.value; }
    };

    struct [[eosio::table]] items
    {
      name name; //*PK

      name owner; //*FK
      uint64_t idskill; //*FK

      uint64_t primary_key() const { return name.value; }
      uint64_t by_secondary() const { return owner.value; }
      uint64_t by_tertiary() const { return idskill; }
    };


    struct [[eosio::table]] ratings
    {
      uint64_t idrating; //*PK

      name item; //*FK
      name user; //*PK

      uint64_t primary_key() const { return idrating; }
      uint64_t by_secondary() const { return item.value; }
      uint64_t by_tertiary() const { return user.value; }
    };

    struct [[eosio::table]] availableSkills
    {
      uint64_t idskill; //*PK

      name name;     
      
      uint64_t primary_key() const { return idskill; }
    };


    struct [[eosio::table]] function
    {
      name name;

      uint64_t primary_key() const { return name.value; }
    };



    typedef eosio::multi_index<"users"_n, users> usersTable;

    typedef eosio::multi_index<
        "item"_n, items,
        indexed_by<"byowner"_n, const_mem_fun<items, uint64_t, &items::by_secondary>>,
        indexed_by<"byskill"_n, const_mem_fun<items, uint64_t, &items::by_tertiary>>>
        itemsTable;

    typedef eosio::multi_index<
        "rating"_n, ratings,
        indexed_by<"byitem"_n, const_mem_fun<ratings, uint64_t, &ratings::by_secondary>>,
        indexed_by<"byuser"_n, const_mem_fun<ratings, uint64_t, &ratings::by_tertiary>>>
        ratingsTable;
        
    typedef eosio::multi_index<"skills"_n, availableSkills> skillsTable;
    typedef eosio::multi_index<"function"_n, function> functionTable;

  };
}