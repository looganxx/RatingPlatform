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

    //da item
    [[eosio::action]] void additem(const name &item, const name& user); //va aggiunta la skill
    [[eosio::action]] void delitem(const name &item, const name& user);

    [[eosio::action]] void addrate(const name &item, const name &user, const uint64_t &score);
    [[eosio::action]] void delrate(const name &item, const name &user);

    //[[eosio::action]] void proviamo(const name &user);

    //da DatabaseSkills
    [[eosio::action]] void addskill(const string &skill);
    [[eosio::action]] void getskills(); //?se tornassi un iterator

    //da FunctionRegistry
    //[[eosio::action]] void getFunction(id);
    //[[eosio::action]] void pushFunction(function, id);

    //using prova_action = action_wrapper<"prova"_n, &RatingSystem::proviamo>;

    using newuser_action = action_wrapper<"newuser"_n, &RatingSystem::newuser>;
    using deluser_action = action_wrapper<"deluser"_n, &RatingSystem::deluser>;

    using additem_action = action_wrapper<"additem"_n, &RatingSystem::additem>;
    using delitem_action = action_wrapper<"delitem"_n, &RatingSystem::delitem>;

    using addrate_action = action_wrapper<"addrate"_n, &RatingSystem::addrate>;
    using delrate_action = action_wrapper<"delrate"_n, &RatingSystem::delrate>;

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
      name iname; //*PK

      name owner; //*FK
      name skill; //*FK

      uint64_t primary_key() const { return iname.value; }
      uint64_t by_secondary() const { return owner.value; }
      uint64_t by_tertiary() const { return skill.value; }
    };


    struct [[eosio::table]] ratings
    {
      //uint64_t idrating; //*PK

      name item; //*FK
      name user; //*PK
      uint64_t score;

      uint64_t primary_key() const { return (item.value | user.value); }
      uint64_t by_secondary() const { return item.value; }
      uint64_t by_tertiary() const { return user.value; }
    };

    struct [[eosio::table]] availableSkills
    {
      name sname; //*PK 
      
      uint64_t primary_key() const { return sname.value; }
    };

    struct [[eosio::table]] userSkills
    {
      //uint64_t iduskill; //*PK
      //!chiave a doppio campo?
      name uname; //*FK
      name skill; //*FK
      uint64_t value; 

      uint64_t primary_key() const { return (uname.value | skill.value); }
    };


    struct [[eosio::table]] function
    {
      name fname;

      uint64_t primary_key() const { return fname.value; }
    };



    typedef eosio::multi_index<"users"_n, users> usersTable;

    typedef eosio::multi_index<"userskills"_n, userSkills> userSkillsTable;

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