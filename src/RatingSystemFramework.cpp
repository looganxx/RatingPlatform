#include <RatingSystemFramework.hpp>

namespace eosio{
  [[eosio::action]] void RatingSystemFramework::createUser(const name &user)
  {

    //?solo chi fa il deploy del contratto può creare l'utente
    //!lo possono fare tutti gli utenti (user)
    require_auth(user);

    //!rivedere i parametri
    userTable users(get_first_receiver(), get_first_receiver().value);
    auto iterator = users.find(user.value);

    //se esiste l'utente => exception
    check(iterator == users.end(), "user already exists ");

    //*primo parametro: chi paga per lo storage del nuovo oggetto 
    users.emplace( user , [&](auto &row) {
      row.key = user;
    });
  }

  [[eosio::action]] void RatingSystemFramework::deleteUser(const name &user)
  {
    require_auth( user );

    userTable users(get_first_receiver(), get_first_receiver().value);
    auto iterator = users.find(user.value);

    //se non esiste l'utente => exception
    check(iterator != users.end(), "user does not exists ");

    users.erase(iterator);
  }

  [[eosio::action]] void RatingSystemFramework::addSkill(const string &skill)
  {
    //*solo chi fa il deploy del contratto può aggiungere una skill
    require_auth(get_self());

    availableSkillsTable skills(get_first_receiver(), get_first_receiver().value);
    auto iterator = skills.find(skill.value);

    //se esiste skill non viene aggiunta
    check(iterator == skills.end(), "skill yet exists ");
    skills.emplace( get_self(), [&](auto &row) {
        row.name = skill;
      });
  }
  

  [[eosio::action]] void RatingSystemFramework::getSkills()
  {
    //?chiunque può chiedere il nome di una skill
    require_auth(get_self());

    availableSkillsTable skills(get_first_receiver(), get_first_receiver().value);

    //!se funziona così TOP
    //return (skills)
  }
}