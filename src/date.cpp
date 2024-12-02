#include <iostream>
#include <string>
#include <iomanip>
#include <sstream>
#include <limits>

#include "date.hpp"

#define RESET "\033[0m"
#define BLACK "\033[30m"
#define RED "\033[31m"
#define GREEN "\033[32m"
#define YELLOW "\033[33m"
#define BLUE "\033[34m"
#define MAGENTA "\033[35m"
#define CYAN "\033[36m"
#define WHITE "\033[37m"

namespace date
{
  bool est_bissextile(unsigned int annee)
  {
    // Vérifie si l'année est bissextile.
    return (annee % 400 == 0) || (annee % 4 == 0 && annee % 100 != 0);
  }

  unsigned int jour_valide(const Date &date)
  {
    // On cherche quel est le jour maximum grâce au mois saisit précédemment.
    unsigned int jour_max;
    if (date.mois == 1 || date.mois == 3 || date.mois == 5 || date.mois == 7 || date.mois == 8 || date.mois == 10 || date.mois == 12)
      jour_max = 31;
    else if (date.mois == 4 || date.mois == 6 || date.mois == 9 || date.mois == 11)
      jour_max = 30;
    else if (date.mois == 2)
      // Pour le mois de février, si l'année est bissextile, on prend 29 sinon 28.
      jour_max = est_bissextile(date.annee) ? 29 : 28;

    return jour_max;
  }

  std::string afficher_date(const Date &date)
  {
    // On affiche la date et on ajoute des 0 si nécessaire pour avoir ce format là : 01/01/2000 00:00
    std::ostringstream chaine;
    chaine << std::setw(2) << std::setfill('0') << date.jour
           << '/' << std::setw(2) << std::setfill('0') << date.mois
           << '/' << date.annee
           << ' ' << std::setw(2) << std::setfill('0') << date.heure
           << ':' << std::setw(2) << std::setfill('0') << date.minute << '\n';
    return chaine.str();
  }

  bool est_valide(const Date &date)
  {
    // On vérifie si la date saisie est valide en regardant l'année, le mois, le jour, les heures et les minutes
    return !(date.annee == 0 || date.mois < 1 || date.mois > 12 || date.jour < 1 || date.jour > jour_valide(date) || date.heure < 0 || date.heure > 23 || date.minute < 0 || date.minute > 59);
  }

  Date creer_date(const std::string &msg)
  {
    Date date;
    std::string entree;
    bool valide = false;

    do
    {
      std::cout << BLUE << msg << RESET << " > ";
      getline(std::cin, entree);

      // Vérifier la présence de séparateurs pour ajuster la lecture
      if (entree.find('/') != std::string::npos || entree.find(':') != std::string::npos)
      {
        // Cas avec séparateurs
        std::istringstream ss(entree);
        char sep_date, sep_heure;
        ss >> date.jour >> sep_date >> date.mois >> sep_date >> date.annee >> date.heure >> sep_heure >> date.minute;

        // Vérifier que les séparateurs sont corrects
        if (sep_date != '/' || sep_heure != ':')
        {
          std::cerr << RED << "Erreur : format de la date incorrect !" << RESET << std::endl;
          continue;
        }
      }
      else
      {
        try
        {
          date.jour = std::stoi(entree.substr(0, 2));
          date.mois = std::stoi(entree.substr(3, 2));
          date.annee = std::stoi(entree.substr(6, 4));
          date.heure = std::stoi(entree.substr(11, 2));
          date.minute = std::stoi(entree.substr(14, 2));
        }
        catch (const std::invalid_argument &e)
        {
          std::cerr << RED << "Erreur : format de la date incorrect !" << RESET << std::endl;
          continue;
        }
        catch (const std::out_of_range &e)
        {
          std::cerr << RED << "Erreur : format de la date incorrect !" << RESET << std::endl;
          continue;
        }
      }

      // Vérifie la validité de la date
      valide = est_valide(date);
      if (!valide)
      {
        std::cerr << RED << "Erreur : date non valide !" << RESET << std::endl;
        continue;
      }
    } while (!valide);

    return date;
  }

  bool est_inverse(const Date &date_debut, const Date &date_fin)
  {
    // Permet de vérifier si la date de fin de l'évènement est plus petite que celle de début.
    if (date_fin.annee != date_debut.annee)
      return date_fin.annee < date_debut.annee;
    if (date_fin.mois != date_debut.mois)
      return date_fin.mois < date_debut.mois;
    if (date_fin.jour != date_debut.jour)
      return date_fin.jour < date_debut.jour;
    if (date_fin.heure != date_debut.heure)
      return date_fin.heure < date_debut.heure;
    return date_fin.minute < date_debut.minute;
  }

  Date lire_date(std::istream &current_date)
  {
    Date date;

    // On stocke la date dans une chaîne de charactère pour la séparer ensuite.
    std::string date_str;
    std::getline(current_date, date_str);

    std::stringstream new_date(date_str);
    char sep_date, sep_time;

    // Lecture de la date.
    new_date >> date.jour >> sep_date >> date.mois >> sep_date >> date.annee;

    // Permet de sauter l'espace entre la date et l'heure.
    new_date.get();

    // Lecture du temps.
    new_date >> date.heure >> sep_time >> date.minute;
    return date;
  }
}