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
    users.emplace( user , [&](auto &row) {
      row.uname = user;
      row.active = true;
    });

    //creo un account per depositare la moneta con balance=0
    token::open_action open_user("eosio.token"_n, {get_self(), "active"_n});
    auto symbol_t = symbol("EOS", 0);
    open_user.send(user, symbol_t, get_self());
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

    users.modify(iterator, user, [&](auto &row) {
      row.active = false;
    });
    //?devo ritirare il balance quando viene disabilitato un utente?
  }

  [[eosio::action]] void RatingSystem::additem(const name &item, const name& user, const name &skill)
  {
    require_auth(user);

    check_user(user, get_first_receiver());

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
      row.active = true;
    });
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
    
    items.modify(iter, owner, [&](auto &row) {
      row.active = false;
    });
  }

  [[eosio::action]] void RatingSystem::addskill(const name &skill)
  {
    //*solo chi fa il deploy del contratto può aggiungere una skill
    require_auth(get_self());

    skillsTable skills(get_first_receiver(), get_first_receiver().value);
    auto iterator = skills.find(skill.value);

    //se esiste skill non viene aggiunta
    check(iterator == skills.end(), "skill already exists");

    skills.emplace( get_self(), [&](auto &row) {
        row.sname = skill;
    });
  }

  [[eosio::action]] void RatingSystem::getskills()
  {
    //?chiunque può chiedere il nome di una skill
    require_auth(get_self());

    skillsTable skills(get_first_receiver(), get_first_receiver().value);

    //!se funziona così TOP
    //return (skills)
  }

  [[eosio::action]] void RatingSystem::addrate(const uint64_t &idpayment, const name &user, const uint64_t &score)
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

    if (i_s == it_s.end() || i_s->uname != user)
    { //creare una nuova row
      userskills.emplace(get_self(), [&](auto &row) {
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

      userskills.modify(mod_s, user, [&](auto &row) {
        if (row.value != 10)
          row.value++;
      });
    }
  }

  [[eosio::action]] void RatingSystem::delrate(const uint64_t &idpayment, const name &user)
  {
    require_auth(user);

    check_user(user, get_first_receiver());

    ratingsTable rates(get_first_receiver(), get_first_receiver().value);
    auto it = rates.find(idpayment);
    check(it != rates.end(), "rate does not exists");
  
    rates.erase(it);
  }

  [[eosio::action]] void RatingSystem::payperm(const name &item, const name &owner, const name &client, const asset &bill)
  {
    /**
     * il proprietario dell'item può usare questa azione
     * controllo che esista il cliente e che non sia disattivato
     * controllo sulla valuta di bill
     */
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
    //check( bill.symbol == st.supply.symbol, "symbol precision mismatch" );
    //?altri controlli su quantity

    paymentsTable payments(get_first_receiver(), get_first_receiver().value);
    uint64_t code = payments.available_primary_key();
    payments.emplace(get_self(), [&](auto &row) {
      row.idpay = code;
      row.iname = item;
      row.client = client;
      row.bill = bill;
      row.paid = 0;
    });

    //TODO mandare id all'utente
    send_notify(client, "code: " + to_string(code) + ", bill: " + bill.to_string());
  }

  [[eosio::action]] void RatingSystem::payitem(const uint64_t& idpay , const name &user, const asset &quantity){
    require_auth(user);
    /**
     * cerco se l'utente esiste ecc
     * controllo quantity se rispetta i valori
     * cerco nella tabella payment e controllo che paid non sia true
     * effettua pagamento
     * setta a true paid
     */

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

    check_user(owner, get_first_receiver());

    send_notify(owner, name{user}.to_string() + " has paid " + quantity.to_string() +
                           " to the bill with code " + to_string(idpay));

    token::transfer_action transfer("eosio.token"_n, {user, "active"_n});
    //trasferisco il denaro da user->owner
    string memo = "bill: " + to_string(idpay) + ", item: " + name{it->iname}.to_string();
    transfer.send(user, owner , quantity, memo);
    payments.modify(it, get_self(), [&](auto &row) {
      row.paid = true;
    });
    
  }
 
  [[eosio::action]] void RatingSystem::prova(const name &i)
  {
    userSkillsTable us(get_first_receiver(), get_first_receiver().value);

    for (auto it = us.begin(); it != us.end();)
    {
      us.erase(it++);
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