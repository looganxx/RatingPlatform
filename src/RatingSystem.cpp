#include <RatingSystem.hpp>

using namespace std;

namespace eosio{

  [[eosio::action]] void RatingSystem::newuser(const name &user)
  {
    //!lo possono fare tutti gli utenti (user)
    require_auth(user);

    //!rivedere i parametri
    usersTable users(get_first_receiver(), get_first_receiver().value);
    auto iterator = users.find(user.value);

    //se esiste l'utente => exception
    check(iterator == users.end(), "user already exists");

    //*primo parametro: chi paga per lo storage del nuovo oggetto 
    users.emplace( user , [&](auto &row) {
      row.uname = user;
    });
  }

  [[eosio::action]] void RatingSystem::deluser(const name &user)
  {
    require_auth( user );

    usersTable users(get_first_receiver(), get_first_receiver().value);
    auto iterator = users.find(user.value);

    //se non esiste l'utente => exception
    check(iterator != users.end(), "user does not exists");

    users.erase(iterator);
  }

  [[eosio::action]] void RatingSystem::additem(const name &item, const name &user)
  {
    require_auth(user);

    usersTable users(get_first_receiver(), get_first_receiver().value);
    auto iterator = users.find(user.value);
    //se non esiste l'utente => exception
    check(iterator != users.end(), "user does not exists");

    itemsTable items(get_first_receiver(), get_first_receiver().value);
    auto iter = items.find(item.value);
    check(iter == items.end(), "item already exists");

    //*primo parametro: chi paga per lo storage del nuovo oggetto
    items.emplace(user, [&](auto &row) {
      row.iname = item;
      row.owner = user;
    });
  }

  [[eosio::action]] void RatingSystem::delitem(const name &item, const name &user)
  {
    require_auth(user);

    itemsTable items(get_first_receiver(), get_first_receiver().value);
    auto iter = items.find(item.value);
    check(iter != items.end(), "item does not exists");
    check(iter->owner == user, user.to_string() + " is not the owner");

    items.erase(iter);
  }

  [[eosio::action]] void RatingSystem::addskill(const string &skill)
  {
    //*solo chi fa il deploy del contratto può aggiungere una skill
    require_auth(get_self());

    skillsTable skills(get_first_receiver(), get_first_receiver().value);
    auto iterator = skills.find(name{skill}.value);

    //se esiste skill non viene aggiunta
    check(iterator == skills.end(), "skill yet exists");
    skills.emplace( get_self(), [&](auto &row) {
        row.sname = name{skill};
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

  [[eosio::action]] void RatingSystem::addrate(const name &item, const name &user, const uint64_t &score)
  {
    require_auth(user);

    //giusto per ricordare che valore ha lo score
    check(score >=0 && score<=5, "invalide score value");
    
    itemsTable items(get_first_receiver(), get_first_receiver().value);
    auto iter = items.find(item.value);
    check(iter != items.end(), "item does not exists");

    usersTable users(get_first_receiver(), get_first_receiver().value);
    auto iterator = users.find(user.value);
    //se non esiste l'utente => exception
    check(iterator != users.end(), "user does not exists");

    //controllo user non voti i suoi item
    check(user != iter->owner, "you can't vote your item!");


    //!serve il token per votare
    //tabella rating - adding
    ratingsTable rates(get_first_receiver(), get_first_receiver().value);
    auto it = rates.find((item.value | user.value));
    check(it == rates.end(), "rating already exists");

    rates.emplace(user, [&](auto &row) {
      //row.idrating = rates.available_primary_key();
      row.item = item;
      row.user = user;
      row.score = score;
    });

    //TODO: aggiornare il punteggio utente
    //?faccio solo +1
    //TODO: va creata prima la tabella contente le skill di user
  }

  [[eosio::action]] void RatingSystem::delrate(const name &item, const name &user){
    require_auth(user);

    ratingsTable rates(get_first_receiver(), get_first_receiver().value);
    auto it = rates.begin();
    while (it != rates.end())
    {
      it = rates.erase(it);
    }
  }

  /*
  [[eosio::action]] void RatingSystem::proviamo(const name &user)
  {
    require_auth(user);
    //!addresses.available_primary_key();
    //print("Hello ", name{user});
    /*
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
    //}
  }*/
}