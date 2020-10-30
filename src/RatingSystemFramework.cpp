#include <RatingSystemFramework.hpp>

namespace eosio{
  [[eosio::action]] void RatingSystemFramework::createUser(name user)
  {

    //?solo chi fa il deploy del contratto può creare l'utente
    //!lo possono fare tutti gli utenti (user)
    require_auth(user);

    //!rivedere i parametri
    userTable users(get_first_receiver(), get_first_receiver().value);
    auto iterator = users.find(user.value);

    //se esiste l'utente => exception
    check(iterator == users.end(), "user already exists ");

    //TODO: creazione oggetto user

    //*primo parametro: chi paga per lo storage del nuovo oggetto 
    users.emplace(""_n, [&](auto &row) {
      row.key = user;
      //*altri valori
    });
  }

  [[eosio::action]] void RatingSystemFramework::deleteUser(name user)
  {
    //solo chi fa il deploy del contratto può eliminare l'utente
    require_auth(get_self());

    //!rivedere i parametri
    userTable users(get_first_receiver(), get_first_receiver().value);
    auto iterator = users.find(user.value);

    //se esiste l'utente => exception
    check(iterator != users.end(), "user does not exists ");

    users.erase(iterator);
  }
}