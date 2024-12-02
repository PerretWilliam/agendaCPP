#include <iostream>
#include <cstring>
#include <random>
#include <map>
#include <forward_list>
#include <sstream>
#include <algorithm>
#include <limits>
#include <fstream>

#include "date.hpp"
#include "agenda.hpp"

#define RESET "\033[0m"
#define BLACK "\033[30m"
#define RED "\033[31m"
#define GREEN "\033[32m"
#define YELLOW "\033[33m"
#define BLUE "\033[34m"
#define MAGENTA "\033[35m"
#define CYAN "\033[36m"
#define WHITE "\033[37m"

// Fonction n'étant pas spécifique à l'agenda :
std::string getUID(std::size_t len)
{
  // Génère un ID aléatoire pour les évènements.
  std::vector<char> x = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f'};
  std::string uid(len, '\0');
  static std::mt19937 gen(std::random_device{}());
  std::uniform_int_distribution<> dis(0, x.size() - 1);
  for (char &c : uid)
  {
    c = x[dis(gen)];
  }
  return uid;
}

namespace agenda
{

  void afficher_erreur(const std::string &msg)
  {
    std::cerr << RED << "Erreur : " << msg << RESET << std::endl;
  }

  std::string demander_titre(const std::string &msg)
  {
    // L'utilisateur est obligé de saisir un titre non vide.
    std::string titre;
    do
    {
      std::cout << BLUE << msg << RESET << " > ";
      std::getline(std::cin, titre);
      if (titre.empty())
      {
        afficher_erreur("Erreur : saisissez un titre non vide !");
      }
      else if (titre.find('/') != -1)
      {
        afficher_erreur("Erreur : saisissez un titre sans '/' !");
      }
    } while (titre.empty() || titre.find('/') != -1);
    return titre;
  }

  std::string demander_description(const std::string &msg)
  {
    // La description peut contenir plusieurs et se termine par un '.' qui ne fait parti de cette description.
    std::string chaine = "";
    std::string description = "";

    std::cout << BLUE << msg << RESET << " > ";
    while (std::getline(std::cin, chaine))
    {
      if (chaine == ".")
      {
        // Evite le \n en trop à la fin.
        if (!description.empty())
        {
          description.pop_back();
        }
        return description;
      }
      description += chaine + '\n';
    }
    return description;
  }

  void afficher_choix(const std::string &msg)
  {
    // Cette fonction affiche un message suivit d'un "oui" ou "non".
    std::cout << BLUE << msg << std::endl;
    std::cout << GREEN << "1. Oui" << std::endl;
    std::cout << RED << "2. Non" << RESET << std::endl;
  }

  bool demander_charger_agenda(Agenda &agenda)
  {
    // Cette fonction charge un agenda si elle le trouve sinon elle le redemande, si /exit est tapé, on quitte la fonction.
    {
      std::string nom_agenda;
      unsigned int selection;
      do
      {
        /* Ici, on ne peut pas utilisé demande_titre(...) car on veut pouvoir utiliser le '/' car aucun agenda ne peut
        posséder de '/' ce qui nous permet d'être sûr que /exit n'est pas un agenda existant. */
        std::cout << BLUE << "Donnez le nom de l'agenda à charger (ou /exit pour retourner au menu principale)." << RESET << " > ";
        std::getline(std::cin, nom_agenda);

        // Si oui, alors on vérifie si le fichier agenda avec le titre donné existe sinon on en crée un nouveau.
        if (nom_agenda == "/exit")
        {
          return false;
        }
        if (!agenda_existe(nom_agenda))
        {
          // Si l'agenda n'est pas trouvé on affiche une erreur.
          afficher_erreur("l'agenda demandé n'existe pas.");
        }
        else
        {
          // Si l'agenda est trouvé on le charge.
          agenda = charger_agenda(nom_agenda);
          return true;
        }
      } while (true);
    }
  }

  Agenda creer_agenda()
  {
    // On crée son titre, sa description et on précise qu'il a été modifié.
    Agenda agenda;
    agenda.titre = demander_titre("Saisissez le titre de l'agenda");
    agenda.description = demander_description("Saisissez sa description (laissez un point pour quitter)");
    agenda.modifie = true;
    return agenda;
  }

  unsigned int selection_menu(const std::string &msg)
  {
    unsigned int valeur;
    while (true)
    {
      std::cout << msg;
      std::cin >> valeur;

      // Cela évite la répétition du msg en vidant le buffer de std::cin.
      std::cin.clear();
      std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
      std::cout << '\n';

      // Cela évite un bug où l'utilisateur est coincé dans une boucle infinie sans pouvoir retaper une valeur.
      if (std::cin.fail())
      {
        afficher_erreur("ce n'est pas un entier !");
      }
      else
        return valeur;
    }
  }

  void afficher_menu_princpale()
  {
    // Affiche le menu principale.
    std::cout << BLUE << "Bienvenue, que souhaitez-vous faire ?" << RESET << std::endl;
    std::cout << "1. Crée un nouvel agenda." << std::endl;
    std::cout << "2. Charger un agenda." << std::endl;
    std::cout << "3. Quitter." << std::endl;
  }

  void menu_principale(Agenda &agenda)
  {
    unsigned int indice_menu;
    bool agenda_charge = false;
    do
    {
      afficher_menu_princpale();
      std::string nom_agenda;
      indice_menu = selection_menu("> ");
      switch (indice_menu)
      {
      // Crée l'agenda.
      case 1:
        agenda = creer_agenda();
        break;

      // On tente de charger un agenda, si ce n'est pas le cas, la fonction renvoie faux et donc retourne dans la boucle au lieu d'en sortir.
      case 2:
        agenda_charge = demander_charger_agenda(agenda);
        break;

      // On quitte le menu principale.
      case 3:
        return;

      // Dans les autres cas, le numéro est incorrect (ou le texte).
      default:
        afficher_erreur("aucun menu ne correspond au numéro saisi.");
        break;
      }
    } while (indice_menu != 1 && !agenda_charge);
    menu(agenda);
  }

  void afficher_menu(const Agenda &agenda)
  {
    // On affiche le menu de l'agenda dans lequel on est.
    std::cout << BLUE << "Bienvenue dans l'agenda " << MAGENTA << agenda.titre << BLUE << '.' << RESET << std::endl;
    std::cout << "1. Modifier l'agenda" << std::endl;
    std::cout << "2. Afficher l'agenda" << std::endl;
    std::cout << "3. Créer un évènement." << std::endl;
    std::cout << "4. Supprimer un évènement." << std::endl;
    std::cout << "5. Exporter au format HTML" << std::endl;
    std::cout << "6. Enregistrer l'agenda" << std::endl;
    std::cout << "7. Quitter" << std::endl;
  }

  void menu(Agenda &agenda)
  {
    // Sélection du menu.
    do
    {
      std::cout << "\n";
      afficher_menu(agenda);
      unsigned int indice_menu = selection_menu("> ");
      switch (indice_menu)
      {
      // Modifier l'agenda.
      case 1:
        agenda.titre = demander_titre("Saisissez le titre de l'agenda > ");
        agenda.description = demander_description("Saisissez sa description (laissez un point pour quitter) > ");
        agenda.modifie = true;
        break;

      // Afficher l'agenda.
      case 2:
        afficher_agenda(agenda);
        break;

      // Crée un évènement.
      case 3:
        creer_evenement(agenda);
        agenda.modifie = true;
        break;

      // Supprimer un évènement.
      case 4:
        supprimer_evenement(agenda);
        break;

      // Exporter l'agenda au HTML.
      case 5:
        exporter_agenda_html(agenda);
        break;

      // Enregistrer l'agenda.
      case 6:
        enregistrer_agenda(agenda);
        agenda.modifie = false;
        break;

      // Quitter l'agenda et demander l'enregistrement s'il a été modifié.
      case 7:
        if (agenda.modifie)
        {
          confirmer_enregistrement(agenda);
        }
        return;

      // Dans les autres cas, on affiche une erreur.
      default:
        afficher_erreur("aucun menu ne correspond au numéro saisi.");
        break;
      }
      // On fait une boucle infini pour toujours rester dans le menu jusqu'à ce que l'utilisateur tape '7' pour quitter.
    } while (true);
  }

  void afficher_agenda(const Agenda &agenda)
  {
    // Affichage de l'agenda.
    std::cout << agenda.titre << std::endl;
    std::cout << agenda.description << std::endl;
    afficher_evenement(agenda);
  }

  void afficher_evenement(const Agenda &agenda)
  {
    // Affichage des informations des évènements s'il en existe.
    if (!agenda.liste_evenement.empty())
      for (const Evenement &evenement : agenda.liste_evenement)
      {
        std::cout << '\n';
        std::cout << afficher_date(evenement.date_debut);
        std::cout << evenement.titre << std::endl;
        std::cout << evenement.description << std::endl;
        std::cout << afficher_date(evenement.date_fin);
      }
    else
    {
      std::cout << '\n';
      std::cout << YELLOW << "Aucun évènement." << RESET << std::endl;
    }
  }

  bool meme_date()
  {
    // On demande à l'utilisateur s'il veut que la date de début = la date de fin.
    afficher_choix("Voulez-vous que la date de début soit également la date de fin ?");
    std::string message = "> ";
    unsigned int selection;

    do
    {
      selection = selection_menu(message);
      switch (selection)
      {
      case 1:
        return true;

      case 2:
        return false;
        break;

      default:
        afficher_erreur("ce n'est pas une réponse valide !");
        break;
      }
    } while (true);
  }

  void creer_evenement(Agenda &agenda)
  {
    Evenement evenement;
    evenement.id = getUID(10);
    evenement.titre = demander_titre("Saisissez le titre de l'évènement");
    evenement.description = demander_description("Saisissez sa description (laissez un point pour quitter)");

    // L'utilisateur saisir les dates si elles sont inversées, on lui redemande de les ressaisir.
    bool condition = false;
    do
    {
      evenement.date_debut = date::creer_date("Saisissez la date de début (jj/mm/aaaa hh:mm)");
      std::cout << '\n';
      bool verif_meme_date = meme_date();
      if (!verif_meme_date)
      {
        evenement.date_fin = date::creer_date("Saisissez la date de fin (jj/mm/aaaa hh:mm)");
      }
      else
      {
        evenement.date_fin = evenement.date_debut;
      }

      if (date::est_inverse(evenement.date_debut, evenement.date_fin))
      {
        afficher_erreur("les dates sont inversées !");
      }
      else
      {
        condition = true;
      }

    } while (!condition);

    ajouter_evenement(agenda, evenement);
  }

  void ajouter_evenement(Agenda &agenda, const Evenement &evenement)
  {
    agenda.liste_evenement.push_front(evenement);
  }

  void supprimer_evenement(Agenda &agenda)
  {
    std::string titre = demander_titre("Donnez le titre de l'évènement à supprimer");

    // Initialisation des itérateurs.
    std::forward_list<Evenement>::iterator avant = agenda.liste_evenement.before_begin();
    std::forward_list<Evenement>::iterator it = agenda.liste_evenement.begin();

    bool est_trouve = false;

    // Tant que la fin de la liste des évènements n'est pas atteinte, on boucle.
    while (it != agenda.liste_evenement.end())
    {
      // Si le titre est trouvé alors on affiche l'ID et la description de l'agenda.
      if (it->titre == titre)
      {
        if (est_trouve)
        {
          std::cout << "ID : " << YELLOW << it->id << RESET << " - Description : " << it->description << std::endl;
        }
        else
        {
          // Si on en trouve au moins un, on met un message disant qu'un évènement a été trouvé.
          est_trouve = true;
          std::cout << "Il existe plusieurs évènements : " << YELLOW << it->titre << RESET << ", voici leur ID : " << std::endl;
          std::cout << "ID : " << YELLOW << it->id << RESET << " - Description : " << it->description << std::endl;
        }
      }
      // Incrémentation des itérateurs.
      ++avant;
      ++it;
    }

    // Si aucun évèbement est trouvé, on le signale.
    if (!est_trouve)
    {
      afficher_erreur("aucun évènement ne comporte ce nom !");
      return;
    }

    // Si un ou des évènements ont été trouvés, on demande l'ID de celui que l'on veut supprimer.
    std::string id = demander_titre("Donnez l'ID de l'évènement à supprimer");
    est_trouve = false;

    // Rénitialisation des itérateurs pour aller chercher à nouveau l'évènement dans la liste.
    avant = agenda.liste_evenement.before_begin();
    it = agenda.liste_evenement.begin();

    while (it != agenda.liste_evenement.end())
    {
      // On regarde quel évènement correspond à l'ID saisit et on demande confirmation de sa suppression.
      if (it->id == id)
      {
        confirmer_suppression(agenda, id, avant);
        est_trouve = true;
        break;
      }
      ++avant;
      ++it;
    }

    if (!est_trouve)
    {
      afficher_erreur("l'ID " + id + " n'existe pas !");
    }
  }

  void confirmer_suppression(Agenda &agenda, std::string id, std::forward_list<Evenement>::iterator avant)
  {
    // On demande à l'utilisateur s'il est sûr de vouloir supprimer l'évènement.
    afficher_choix("L'évènement avec l'ID\033[33m " + id + " \033[0msera supprimé.");
    std::string message = "> ";
    unsigned int selection;

    // Choix, 1 = Suppression de l'évènement et 2 = Annuler cette suppression, dans les autres cas : Erreur.
    do
    {
      selection = selection_menu(message);
      switch (selection)
      {
      case 1:
        // Supression de l'évènement.
        agenda.liste_evenement.erase_after(avant);
        std::cout << "L'évènement avec l'ID " << YELLOW << id << RESET << " a bien été supprimé." << std::endl;
        agenda.modifie = true;
        break;

      case 2:
        // Annule la suppression de l'évènement.
        std::cout << "L'évènement avec l'ID " << YELLOW << id << RESET << " ne sera pas supprimé." << std::endl;
        break;

      default:
        afficher_erreur("ce n'est pas une réponse valide !");
        break;
      }
    } while (selection != 1 && selection != 2);
  }

  void exporter_agenda_html(const Agenda &agenda)
  {
    std::string nom = agenda.titre + ".html";
    std::ofstream fichier(nom);

    // En-tête du fichier HTML.
    fichier << "<!DOCTYPE html>\n<html lang=\"fr\">\n<head>\n";
    fichier << "<meta charset=\"UTF-8\">\n<title>" << agenda.titre << "</title>\n";
    fichier << "</head>\n<body>\n";

    // Titre et description de l'agenda.
    fichier << "<h1>" << agenda.titre << "</h1>\n";
    fichier << "<p>";

    // On boucle jusqu'à atteindre la fin, à chaquye \n on met un <br> pour sauter une ligne, à la fin on ferme le paragraphe.
    for (char c : agenda.description)
    {
      if (c == '\n')
      {
        fichier << "<br>\n";
      }
      else
      {
        fichier << c;
      }
    }
    fichier << "\n</p>\n";

    // Affichage des évènements de l'agenda.
    fichier << "<h2>Événements</h2>\n";
    if (agenda.liste_evenement.empty())
    {
      fichier << "<p>Aucun événement dans l'agenda.</p>\n";
    }
    // Dans le cas où des évènements existent, on crée une table pour classer leurs informations.
    else
    {
      // Création des en-tête.
      fichier << "<table>\n";
      fichier << "<tr>\n";
      fichier << "<th>ID</th>\n";
      fichier << "<th>Titre</th>\n";
      fichier << "<th>Description</th>\n";
      fichier << "<th>Date de début</th>\n";
      fichier << "<th>Date de fin</th>\n";
      fichier << "</tr>\n";
      for (const Evenement &evenement : agenda.liste_evenement)
      {
        // Création des évènements.
        fichier << "<tr>";
        fichier << "<td>" << evenement.id << "</td>\n";
        fichier << "<td>" << evenement.titre << "</td>\n";
        fichier << "<td><p>";

        // Comme pour la description de l'agenda.
        for (char c : evenement.description)
        {
          if (c == '\n')
          {
            fichier << "<br>\n";
          }
          else
          {
            fichier << c;
          }
        }
        fichier << "</p></td>\n";
        fichier << "<td>" << afficher_date(evenement.date_debut) << "</td>\n";
        fichier << "<td>" << afficher_date(evenement.date_fin) << "</td>\n";
        fichier << "</tr>";
      }
      fichier << "</table>\n";
    }

    // On termine le fichier HTML.
    fichier << "</body>\n</html>\n";
    fichier.close();

    // On informe l'utilisateur que l'agenda à bien était exporté.
    std::cout << GREEN << "L'agenda a été exporté dans le fichier : " << CYAN << nom << GREEN << '.' << RESET << std::endl;
  }

  void enregistrer_agenda(const Agenda &agenda)
  {
    // On crée un fichier agenda.
    std::string agenda_txt = agenda.titre + ".txt";
    std::ofstream agenda_enregistre(agenda_txt);

    // Si on arrive pas à y accéder on affiche une erreur.
    if (!agenda_enregistre.is_open())
    {
      afficher_erreur("impossible d'ouvrir l'agenda !" + agenda_txt);
      return;
    }

    // Sauvegarde du titre et de la description de l'agenda
    agenda_enregistre << agenda.titre << "\n";
    agenda_enregistre << agenda.description << "\n";
    agenda_enregistre << ".\n";

    // Sauvegarde des événements
    for (const Evenement &evenement : agenda.liste_evenement)
    {
      agenda_enregistre << evenement.id << "\n";
      agenda_enregistre << evenement.titre << "\n";
      agenda_enregistre << evenement.description << "\n";
      agenda_enregistre << ".\n";

      agenda_enregistre << afficher_date(evenement.date_debut);
      agenda_enregistre << afficher_date(evenement.date_fin);
    }

    // On ferme le fichier.
    agenda_enregistre.close();

    std::cout << GREEN << "L'agenda a été enregistré dans le fichier " << CYAN << agenda_txt << GREEN << '.' << RESET << std::endl;
  }

  std::string charger_description(std::ifstream &agenda_charge)
  {
    std::string ligne;
    std::string description;
    do
    {
      std::getline(agenda_charge, ligne);
      if (ligne != ".")
      {
        description += ligne + '\n';
      }
      else
      {
        // Evite le \n en trop.
        description.pop_back();
      }
      // Tant que on a pas juste un '.' sur une ligne, alors on boucle.
    } while (ligne != ".");
    return description;
  }

  Agenda charger_agenda(const std::string &nom_agenda)
  {
    Agenda agenda;
    std::ifstream agenda_charge(nom_agenda + ".txt");

    // Si le fichier ne peut pas être ouvert on affiche une erreur.
    if (!agenda_charge.is_open())
    {
      afficher_erreur("Erreur : impossible d'ouvrir le fichier " + nom_agenda + '.');
      return agenda;
    }

    // On initialize les lignes et descriptions.
    std::string ligne;
    std::string description;

    // On charge le titre avec la première ligne et la description avec la fonction prévue pour.
    std::getline(agenda_charge, agenda.titre);
    agenda.description = charger_description(agenda_charge);

    // Chargement des événements.
    Evenement evenement;

    // Tant que on arrive à lire un ID sur la ligne et que donc la chaine n'est pas vide alors on charge l'évènement.
    while (std::getline(agenda_charge, evenement.id) && !evenement.id.empty())
    {
      std::getline(agenda_charge, evenement.titre);
      description = "";

      evenement.description = charger_description(agenda_charge);

      // Chargement des dates.
      std::getline(agenda_charge, ligne);
      std::istringstream date_debut(ligne);
      evenement.date_debut = date::lire_date(date_debut);

      std::getline(agenda_charge, ligne);
      std::istringstream date_fin(ligne);
      evenement.date_fin = date::lire_date(date_fin);

      // Ajoute l'événement à la liste
      agenda.liste_evenement.push_front(evenement);
    }

    // On notifie l'utilisateur que l'agenda a été chargé.
    std::cout << GREEN << "L'agenda a été chargé." << RESET << std::endl;
    agenda_charge.close();
    return agenda;
  }

  void confirmer_enregistrement(const Agenda &agenda)
  {
    unsigned int selection;

    // Même schéma que pour la suppression de l'évènement, c'est un choix classique.
    do
    {
      afficher_choix("L'agenda a été modifié ! L'enregistrer ?");
      selection = selection_menu("> ");
      switch (selection)
      {
      case 1:
        // Enregistrement de l'agenda.
        enregistrer_agenda(agenda);
        break;

      case 2:
        // On enregistre pas l'agenda.
        break;

      default:
        afficher_erreur("réponse invalide !");
        break;
      }
    } while (selection != 1 && selection != 2);
  }

  bool agenda_existe(const std::string &nom_agenda)
  {
    // Vérifie si le nom donné pour charger l'agenda existe.
    std::ifstream fichier(nom_agenda + ".txt");
    return fichier.is_open();
  }
}
