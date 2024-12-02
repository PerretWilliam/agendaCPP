#ifndef AGENDA_HPP
#define AGENDA_HPP

#include <iostream>
#include <string>
#include <forward_list>

std::string getUID(std::size_t len);

namespace agenda
{
  struct Evenement
  {
    std::string id;
    std::string titre;
    std::string description;
    date::Date date_debut;
    date::Date date_fin;
  };

  struct Agenda
  {
    std::string titre;
    std::string description;
    std::forward_list<Evenement> liste_evenement;
    bool modifie = false;
  };

  std::string demander_titre(const std::string &msg = "");
  std::string demander_description(const std::string &msg = "");

  void afficher_choix(const std::string &msg = "");

  bool demander_charger_agenda(Agenda &agenda);

  Agenda creer_agenda();

  unsigned int selection_menu(const std::string &msg = "");

  void afficher_menu_princpale();
  void menu_principale(Agenda &agenda);

  void afficher_menu();
  void menu(Agenda &agenda);

  void afficher_agenda(const Agenda &agenda);

  void afficher_evenement(const Agenda &agenda);
  void creer_evenement(Agenda &agenda);
  void ajouter_evenement(Agenda &agenda, const Evenement &evenement);
  void supprimer_evenement(Agenda &agenda);
  void confirmer_suppression(Agenda &agenda, std::string id, std::forward_list<Evenement>::iterator avant);

  void exporter_agenda_html(const Agenda &agenda);

  void enregistrer_agenda(const Agenda &agenda);
  void confirmer_enregistrement(const Agenda &agenda);

  std::string charger_description(const std::ifstream &agenda_charge);
  Agenda charger_agenda(const std::string &nom_agenda);

  bool agenda_existe(const std::string &nom_agenda);
}

#endif