#ifndef DATE_HPP
#define DATE_HPP

#include <string>

namespace date
{
  struct Date
  {
    int annee;
    unsigned int jour, mois, heure, minute;
  };

  bool est_bissextile(unsigned int annee);

  unsigned int jour_valide(const Date &date);

  std::string afficher_date(const Date &date);

  Date creer_date(const std::string &msg = "");

  bool est_inverse(const Date &date_debut, const Date &date_fin);

  Date lire_date(std::istream &current_date);
}
#endif
