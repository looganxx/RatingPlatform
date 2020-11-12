#pragma once

#include <eosio/eosio.hpp>
#include <eosio/asset.hpp>
#include <eosio/symbol.hpp>

#include <rsf.token/rsf.token.hpp>

using namespace std;

namespace eosio{
  class [[eosio::contract("RatingSystem")]] RatingSystem : public eosio::contract
  {

  public:
    using contract::contract;

    //constructor
    RatingSystem(name receiver, name code, datastream<const char *> ds) : contract(receiver, code, ds){}

    /**
     * @brief   Create a new user. If succesfully created, then a new entry in the user table is created.
     * 
     * @param user: Eosio's name of the user who want to create an account in RS.
     * 
     * @pre   User has to have an eosio account.
     */
    [[eosio::action]] void newuser(const name& user);

    /**
     * @brief   Deactivate the user. The "active" bool variable will set to false. 
     * 
     * @param user: Eosio's name of the user who want to deactivate their own account in RS.
     * 
     * @pre  User has to have an RS account.
     */
    [[eosio::action]] void deluser(const name& user);

    /**
     * @brief   Allows an user to create his own item.
     * 
     * @param item: New item's name.
     * @param user: Name of the account who wants to create the item.
     * @param skill: Name of the skill that owns the item. 
     * 
     * @pre  User has to have an RS account.
     */
    [[eosio::action]] void additem(
        const name &item,
        const name &user,
        const name &skill,
        const symbol &sym,
        const double &tokenval);

    /**
     * @brief Deactivate the item. The "active" bool variable will set to false.
     * 
     * @param item: Name of the item to deactivate.
     * @param owner:  Name of the item's owner. 
     * 
     * @pre Owner has to be the item's holder.
     */
    [[eosio::action]] void delitem(const name &item, const name& owner);

    /**
     * @brief Allows RS account to add a new skill in the skills DB.
     * 
     * @param skill:  Name of the skill to add.
     * 
     * @pre Only the account that deploy the contract can call this action.
     */
    [[eosio::action]] void addskill(const name &skill_name);

    /**
     * @brief 
     * 
     */
    [[eosio::action]] void getskills(); //?se tornassi un iterator

    /**
     * @brief Allows an item's owner give to an user a permission to pay the service. The token will notified to the user who have to pay.
     *        The boolean value "paid" will set to false.
     * 
     * @param item: Name of the item.
     * @param owner:  Name of the item's owner who want grant the permission to pay to the user "client".
     * @param client: Name of the user who will pay for the service.
     * @param bill: Amount that the "client" will pay to "owner".
     * 
     * @pre The account "owner" has to own an item.
     * @pre "owner" has to be different to "client".
     */
    [[eosio::action]] void payperm(
        const name &item,
        const name &owner,
        const name &client,
        const asset &bill);

    /**
     * @brief An user that have to pay a bill call this action passing the value of the token and the amount to pay.
     *        The boolean value "paid" will set to true.
     * 
     * @param idpay: The token emitted by the "payperm" action.
     * @param user: The name of the account that have to pay the bill.
     * @param quantity: The amount of the bill to pay.
     * 
     * @pre "user" has to own the token ("idpay")
     * @pre Only the user that have the permission granted can pay the bill.
     */
    [[eosio::action]] void payitem(
        const uint64_t &idpay,
        const name &user,
        const asset &quantity,
        const bool &pay_with_token);

    /**
     * @brief Allows "user" to rate an item precedently paid using the same token used tu pay the item.
     * 
     * @param idpayment: The token emitted by the "payperm" action used also to pay the item.
     * @param user: The name of the user who want to rate the item.
     * @param score: The value to assing to the item rated. The "score" value has to be between 1 and 10.  
     * 
     * @pre The "user" had paid the owner of the item using the token.
     */
    [[eosio::action]] void addrate(const uint64_t &idpayment, const name& user, const uint64_t &score);

    /**
     * @brief Allows "user" to delete a rate.
     * 
     * @param idpayment: Token used to pay and to rate de item.
     * @param user: Name of the user who made the rate.
     */
    [[eosio::action]] void delrate(const uint64_t &idpayment, const name &user);

    [[eosio::action]] void deathangel();

    /**
     * @brief Action used to notify to an user some informations.
     * 
     * @param user: Name of the user to whom you want to notify the action.
     * @param message: Message to send to "user".
     * 
     * @pre Only the account that deploy the contract can call this action.
     */
    [[eosio::action]] void notify(const name& user, const string& message)
    {
      require_auth(get_self());
      require_recipient(user);
    }

    
    //da FunctionRegistry
    //[[eosio::action]] void getFunction(id);
    //[[eosio::action]] void pushFunction(function, id);


    /*
    using prova_action = action_wrapper<"prova"_n, &RatingSystem::proviamo>;

    using newuser_action = action_wrapper<"newuser"_n, &RatingSystem::newuser>;
    using deluser_action = action_wrapper<"deluser"_n, &RatingSystem::deluser>;

    using additem_action = action_wrapper<"additem"_n, &RatingSystem::additem>;
    using delitem_action = action_wrapper<"delitem"_n, &RatingSystem::delitem>;

    using addrate_action = action_wrapper<"addrate"_n, &RatingSystem::addrate>;
    using delrate_action = action_wrapper<"delrate"_n, &RatingSystem::delrate>;

    using addSkill_action = action_wrapper<"addskill"_n, &RatingSystem::addskill>;
    using getSkill_action = action_wrapper<"getskills"_n, &RatingSystem::getskills>;

    using payperm_action = action_wrapper<"payperm"_n, &RatingSystem::payperm>;
    using payitem_action = action_wrapper<"payitem"_n, &RatingSystem::payitem>;
    */
    //using getFunction_action = action_wrapper<"get_Function"_n, &RatingSystem::getFunction>;
    //using pushFunction_action = action_wrapper<"push_Function"_n, &RatingSystem::pushFunction>;

    /*static const struct item_s
    {
      name item;
      uint64_t rating;
    } item;*/

  private:
    static void check_user(const name &user, const name& gfr)
    {
      usersTable users(gfr, gfr.value);
      auto iterator = users.find(user.value);
      //se non esiste l'utente => exception
      check(iterator != users.end(), "user does not exists");
      //se l'utente non è più attivo => exception
      check(iterator->active == true, "this user is no longer active");
    }

    static void check_item(const name &item, const name &gfr)
    {
      itemsTable items(gfr, gfr.value);
      auto iter = items.find(item.value);
      //se item non esiste => exception
      check(iter != items.end(), "item does not exists");
      //se item non è più attivo => exception
      check(iter->active == 1, "item not active");
    }

    void send_notify(const name& user, const string& message)
    {
      action(
        //permission_level,
        permission_level{get_self(), "active"_n},
        //code,
        get_self(),
        //action,
        "notify"_n,
        //data
        std::make_tuple(user, message))
        .send();
    }

    struct [[eosio::table]] users
    {
      name uname; //*PKit
      bool active;

      uint64_t primary_key() const { return uname.value; }
    };

    struct [[eosio::table]] items
    {
      name iname; //*PK

      name owner; //*FK
      name skill; //*FK
      symbol sym;
      double tokenval;
      bool active; 


      uint64_t primary_key() const { return iname.value; }
      uint64_t by_secondary() const { return owner.value; }
      uint64_t by_tertiary() const { return skill.value; }
    };

    struct [[eosio::table]] availableSkills
    {
      name sname; //*PK 
      
      uint64_t primary_key() const { return sname.value; }
    };

    struct [[eosio::table]] userSkills
    {
      uint64_t iduskill; //*PK

      name uname; //*FK
      name skill; //*FK
      uint64_t value;

      uint64_t primary_key() const { return iduskill; }
      uint64_t by_secondary() const { return uname.value; }
      uint64_t by_tertiary() const { return skill.value; }
    };

    struct [[eosio::table]] payments
    {
      uint64_t idpay; //*PK

      name iname; //*FK
      name client; //*FK
      asset bill;
      bool paid;

      uint64_t primary_key() const { return idpay; }
      uint64_t by_secondary() const { return iname.value; }
      uint64_t by_tertiary() const { return client.value; }
    };

    struct [[eosio::table]] ratings
    {
      uint64_t idrating; //*PK

      name item;      //*FK
      name user;      //*FK
      uint64_t score; //il più giovane

      uint64_t primary_key() const { return idrating; }
      uint64_t by_secondary() const { return item.value; }
      uint64_t by_tertiary() const { return user.value; }
    };

    struct [[eosio::table]] function
    {
      name fname;

      uint64_t primary_key() const { return fname.value; }
    };



    typedef eosio::multi_index<"users"_n, users> usersTable;

    typedef eosio::multi_index<
      "userskills"_n, userSkills,
      indexed_by<"byuser"_n, const_mem_fun<userSkills, uint64_t, &userSkills::by_secondary>>,
      indexed_by<"byskill"_n, const_mem_fun<userSkills, uint64_t, &userSkills::by_tertiary>>>
      userSkillsTable;

    typedef eosio::multi_index<
      "items"_n, items,
      indexed_by<"byowner"_n, const_mem_fun<items, uint64_t, &items::by_secondary>>,
      indexed_by<"byskill"_n, const_mem_fun<items, uint64_t, &items::by_tertiary>>>
      itemsTable;

    typedef eosio::multi_index<
      "ratings"_n, ratings,
      indexed_by<"byitem"_n, const_mem_fun<ratings, uint64_t, &ratings::by_secondary>>,
      indexed_by<"byuser"_n, const_mem_fun<ratings, uint64_t, &ratings::by_tertiary>>>
      ratingsTable;

    typedef eosio::multi_index<
      "payments"_n, payments,
      indexed_by<"byitem"_n, const_mem_fun<payments, uint64_t, &payments::by_secondary>>,
      indexed_by<"byuser"_n, const_mem_fun<payments, uint64_t, &payments::by_tertiary>>>
      paymentsTable;

    typedef eosio::multi_index<"skills"_n, availableSkills> skillsTable;
    typedef eosio::multi_index<"function"_n, function> functionTable;

  };

}