#include <RatingSystem.hpp>

using namespace std;

namespace eosio{

  [[eosio::action]] void RatingSystem::newuser(const name &user)
  {
    //!lo possono fare tutti gli utenti (user)
    require_auth(user);
    check(is_account(user), "to account does not exist");
    //!rivedere i parametri
    usersTable users(get_first_receiver(), get_first_receiver().value);
    auto iterator = users.find(user.value);

    //se esiste l'utente => exception
    check(iterator == users.end(), "user already exists");

    //*primo parametro: chi paga per lo storage del nuovo oggetto
    users.emplace(user, [&](auto &row) {
      row.uname = user;
      row.active = true;
    });
  }

  [[eosio::action]] void RatingSystem::deluser(const name &user)
  {
    require_auth( user );

    usersTable users(user, user.value);
    auto iterator = users.find(user.value);
    //se non esiste l'utente => exception
    check(iterator != users.end(), "user does not exists");
    //se l'utente non è più attivo => exception
    check(iterator->active == true, "this user is no longer active");

    users.modify(iterator, same_payer, [&](auto &row) {
      row.active = false;
    });
    //?devo ritirare il balance quando viene disabilitato un utente?
  }

  [[eosio::action]] void RatingSystem::additem(
      const name &item,
      const name &user,
      const name &skill,
      const asset &max_supply,
      const double &tokenval)
  {
    require_auth(user);

    check_user(user, get_first_receiver());
    const symbol& sym = max_supply.symbol;
    check(sym.is_valid(), "symbol not valid");
    check(tokenval>0 && tokenval<=1, "invalid token value");

    itemsTable items(get_first_receiver(), get_first_receiver().value);
    auto iter = items.find(item.value);
    check(iter == items.end(), "item already exists");

    skillsTable skills(get_first_receiver(), get_first_receiver().value);
    auto it_skills = skills.find(skill.value);
    check(it_skills != skills.end(), "skill does not exists");

    //*primo parametro: chi paga per lo storage del nuovo oggetto
    items.emplace(user, [&](auto &row) {
      row.iname = item;
      row.owner = user;
      row.skill = skill;
      row.sym = sym;
      row.tokenval = tokenval;
      row.active = true;
    });
    
    rsftoken::create_action create_val("rsf.token"_n, {get_self(), "active"_n});
    create_val.send(user, max_supply);

    rsftoken::issue_action issue("rsf.token"_n, {user, "active"_n});
    issue.send(user, max_supply, "issue token");
  }

  [[eosio::action]] void RatingSystem::delitem(const name &item, const name& owner)
  {
    require_auth(owner);
  
    itemsTable items(get_first_receiver(), get_first_receiver().value);
    auto iter = items.find(item.value);
    check(iter != items.end(), "item does not exists");
    check(iter->owner == owner, owner.to_string() + " is not the owner");

    check_user(owner, get_first_receiver());

    //items.erase(iter);
    
    items.modify(iter, same_payer, [&](auto &row) {
      row.active = false;
    });
  }

  [[eosio::action]] void RatingSystem::addskill(const name &skill_name)
  {
    //*solo chi fa il deploy del contratto può aggiungere una skill
    require_auth(get_self());

    skillsTable skills(get_first_receiver(), get_first_receiver().value);
    auto iterator = skills.find(skill_name.value);

    //se esiste skill non viene aggiunta
    check(iterator == skills.end(), "skill already exists");

    skills.emplace(get_self(), [&](auto &row) {
      row.sname = skill_name;
    });
  }

  [[eosio::action]] void RatingSystem::addrate(
    const uint64_t &idpayment, 
    const name &user, 
    const uint64_t &score)
  {
    require_auth(user);
    check_user(user, get_first_receiver());

    check(score >0 && score<=10, "invalide score value");

    paymentsTable payments(get_first_receiver(), get_first_receiver().value);
    auto id = payments.find(idpayment);
    check (id->paid ==true, "bill not paid");
    check(id->client == user, "you can't rate this item, this is not youre bill");
    name item = id->iname;

    itemsTable items(get_first_receiver(), get_first_receiver().value);
    auto iter = items.find(item.value);
    check(iter != items.end(), "item does not exists");
    check(iter->active == 1, "item no longer active");
    name i_skill = iter->skill;
    name owner = iter->owner;
    symbol sym = iter->sym;

    //controllo user non voti i suoi item
    check(user != iter->owner, "you can't vote your item!");

    ratingsTable rates(get_first_receiver(), get_first_receiver().value);
    auto it = rates.find(idpayment);
    check(it == rates.end(), "you already rate it");
    rates.emplace(user, [&](auto &row) {
      row.idrating = idpayment;
      row.item = item;
      row.user = user;
      row.score = score;
    }); 

    
    //controllo se non esiste già lo stesso campo
    userSkillsTable userskills(get_first_receiver(), get_first_receiver().value);
    auto it_s = userskills.get_index<"byuser"_n>();
    auto i_s = it_s.find(user.value);

    
    while (i_s != it_s.end() && i_s->uname == user && i_s->skill != i_skill)
    {
      i_s++;
    }

    uint64_t transf_value = 0;
    if (i_s == it_s.end() || i_s->uname != user)
    { //creare una nuova row
      userskills.emplace(user, [&](auto &row) {
        row.iduskill = userskills.available_primary_key();
        row.uname = user;
        row.skill = i_skill;
        row.value = 1;
      });
    }
    else
    { //aggiornare row già esistente
      uint64_t index_s = i_s->iduskill;
      auto mod_s = userskills.find(index_s);

      userskills.modify(mod_s, same_payer, [&](auto &row) {
        transf_value = row.value;
        if (row.value != 10)
          row.value++;
      });
    }


    //*trasferimento token dall'owner al client
    if(transf_value != 0 ){
      rsftoken::transfer_action transfer("rsf.token"_n, {owner, "active"_n});
      string memo = "tokens gained for rating";
      //prendere il simbolo dalla tabella e usarlo per creare la quantità
      uint64_t precision = pow(10, sym.precision());
      asset quantity = asset(transf_value * precision, sym); 
      transfer.send(owner, user, quantity, memo);
    }
  }

  [[eosio::action]] void RatingSystem::payperm(const name &item, const name &owner, const name &client, const asset &bill)
  {
    require_auth(owner);

    check_user(owner, get_first_receiver());
    check(owner != client, "owner and client must be different");

    itemsTable items(get_first_receiver(), get_first_receiver().value);
    auto iter = items.find(item.value);
    check(iter != items.end(), "item does not exists");
    check(iter->active == 1, "item not active");
    check(iter->owner == owner, owner.to_string() + " is not the owner");

    check_user(client, get_first_receiver());

    auto sym = bill.symbol;
    check(sym.is_valid(), "invalid symbol name");
    check( bill.is_valid(), "invalid quantity" );
    check( bill.amount > 0, "bill must be a positive quantity" );
    check( bill.symbol == symbol("RSF", 4), "symbol precision mismatch");

    paymentsTable payments(get_first_receiver(), get_first_receiver().value);
    uint64_t code = payments.available_primary_key();
    payments.emplace(owner, [&](auto &row) {
      row.idpay = code;
      row.iname = item;
      row.client = client;
      row.bill = bill;
      row.paid = 0;
    });

    send_notify(client, "code: " + to_string(code) + ", bill: " + bill.to_string());
  }

  [[eosio::action]] void RatingSystem::payitem(
      const uint64_t &idpay,
      const name &user,
      const asset &quantity,
      const bool &pay_with_token)
  {
    require_auth(user);

    check_user(user, get_first_receiver());

    paymentsTable payments(get_first_receiver(), get_first_receiver().value);
    auto it = payments.find(idpay);
    check(it != payments.end(), "bill does not exists");
    check (it->paid == false, "bill already paid");
    check(it->bill == quantity, "please insert the correct value to pay");
    check(it->client == user, "you have not to pay");

    itemsTable items(get_first_receiver(), get_first_receiver().value);
    auto iter = items.find((it->iname).value);
    name owner = iter->owner;
    symbol sym;
    double tokenval; 
    if(pay_with_token){
      sym = iter->sym;
      tokenval = iter->tokenval;
    }

    check_user(owner, get_first_receiver());

    asset final_quantity;
    if(pay_with_token){
      //convertire il coupon in RSF
      asset sym_balance = rsftoken::balance("rsf.token"_n, user, sym.code());
      double coup_conv = sym_balance.amount * tokenval;
      uint64_t coup_conv_i = (uint64_t) (coup_conv + 0.5);
      if(quantity.amount > coup_conv_i){
        //preparo doppia transazione
        final_quantity.amount = quantity.amount - coup_conv_i;
        final_quantity.symbol = quantity.symbol;

        rsftoken::transfer_action transfer("rsf.token"_n, {user, "active"_n});
        string memo = "bill: " + to_string(idpay) +
                      ", item: " + name{it->iname}.to_string() +
                      ", paid in token: " + sym_balance.to_string();
        transfer.send(user, owner, sym_balance, memo);
      }else{
        //sinogla transazione di coupon/token
        //conversione bill in coupon
        double bill_conv = quantity.amount / tokenval;
        uint64_t bill_conv_i = (uint64_t)(bill_conv + 0.5);
        final_quantity.amount = bill_conv_i;
        final_quantity.symbol = sym_balance.symbol;
      }
    }else{
      final_quantity = quantity;
    }

    rsftoken::transfer_action transfer("rsf.token"_n, {user, "active"_n});
    //trasferisco il denaro da user->owner
    string memo = "bill: " + to_string(idpay) + ", item: " + name{it->iname}.to_string();
    transfer.send(user, owner, final_quantity, memo);
    payments.modify(it, user, [&](auto &row) {
      row.paid = true;
    });

    send_notify(owner, name{user}.to_string() + " has paid the bill with code " + to_string(idpay));
  }

  [[eosio::action]] void RatingSystem::avg(const name& user, const name &item)
  {
    require_auth(user);
    check_item(item, get_first_receiver());
    ratingsTable ratings(get_first_receiver(), get_first_receiver().value);
    auto it_r = ratings.get_index<"byitem"_n>();
    auto i_r = it_r.find(item.value);

    check(i_r != it_r.end(), "item never rated");

    uint64_t count = 0;
    double avg = 0;


    while (i_r != it_r.end())
    {
      count++;
      avg += i_r->score;
      i_r++;
    }

    avg = avg/count;
    send_notify(user, "the average of the " 
      + name{item}.to_string() 
      + " reviews is " 
      + std::to_string(avg));
  }

  [[eosio::action]] void RatingSystem::weightedavg(const name& user, const name &item)
  {
    require_auth(user);
    check_item(item, get_first_receiver());
    ratingsTable ratings(get_first_receiver(), get_first_receiver().value);
    auto it_r = ratings.get_index<"byitem"_n>();
    auto i_r = it_r.find(item.value);
    check(i_r != it_r.end(), "item never rated");

    itemsTable items(get_first_receiver(), get_first_receiver().value);
    auto iter = items.find(item.value);
    name skill = iter->skill;

    std::vector<pair<eosio::name, uint64_t>> users;

    uint64_t weigth = 0;
    uint64_t w_sum = 0;
    double w_avg;


    while (i_r != it_r.end())
    {
      name w_user = i_r->user;

      auto it = std::find_if(users.begin(), users.end(), 
        [&](const std::pair<eosio::name, uint64_t> &element) 
        { return element.first == w_user; });

      //user in
      if (it != users.end()){
        weigth += it->second;
        w_sum += i_r->score * it->second;
      }
      else{ //add user
        userSkillsTable userskills(get_first_receiver(), get_first_receiver().value);
        auto it_s = userskills.get_index<"byuser"_n>();
        auto i_s = it_s.find(w_user.value);

        while (i_s != it_s.end() && i_s->uname == w_user && i_s->skill != skill)
        {
          i_s++;
        }

        if (i_s != it_s.end()){
          pair<eosio::name, uint64_t> user_skill;
          user_skill.first = w_user;
          user_skill.second = i_s->value;

          weigth += user_skill.second;
          w_sum += i_r->score * user_skill.second;

          users.push_back(user_skill);
        }
      }

      i_r++;
    }

    w_avg = w_sum/weigth;


    send_notify(user, "the weighted average of the " 
      + name{item}.to_string() 
      + " reviews is " 
      + std::to_string(w_avg));
  }

  [[eosio::action]] void RatingSystem::deathangel()
  {
    usersTable us(get_first_receiver(), get_first_receiver().value);

    for (auto it = us.begin(); it != us.end();)
    {
      us.erase(it++);
    }

    userSkillsTable ust(get_first_receiver(), get_first_receiver().value);

    for (auto it = ust.begin(); it != ust.end();)
    {
      ust.erase(it++);
    }

    itemsTable ite(get_first_receiver(), get_first_receiver().value);

    for (auto it = ite.begin(); it != ite.end();)
    {
      ite.erase(it++);
    }

    ratingsTable rtab(get_first_receiver(), get_first_receiver().value);

    for (auto it = rtab.begin(); it != rtab.end();)
    {
      rtab.erase(it++);
    }

    paymentsTable ptab(get_first_receiver(), get_first_receiver().value);

    for (auto it = ptab.begin(); it != ptab.end();)
    {
      ptab.erase(it++);
    }

    /*
    require_auth(user);
    //!addresses.available_primary_key();
    //print("Hello ", name{user});
    
    itemsTable items(get_first_receiver(), get_first_receiver().value);
    auto iter = items.get_index<"byowner"_n>();
    //check(iter != items.end(), "mannaia");
    auto next = iter.find(user.value);
    //for (; next != iter.end(); next++){
    print(next->iname.to_string() + " " + next->owner.to_string() + "\n");
    next++;
    print(next->iname.to_string() + " " + next->owner.to_string() + "\n");
    next++;
    print(next->iname.to_string() + " " + next->owner.to_string() + "\n");
    next++;
    print(next->iname.to_string() + " " + next->owner.to_string() + "\n");
    //}*/
  }
}