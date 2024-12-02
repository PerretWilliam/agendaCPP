Application de gest d'agenda en C++

Cette agenda a été développé en C++ qui fonctionne dans le terminal.
Il est composé de deux couples CPP/HPP :
  - date.cpp et date.hpp
  - agenda.cpp et agenda.hpp
et d'un main : main.cpp
Pour le compiler, un Makefile est à votre disposition, en bash, exécutez la commande "make".

Pour les tests avec expect, un dossier log sera crée avec trois tests bien distinct :
  - Deux tests qui s'assuront du bon fonctionnement de l'agenda avec des réponses attendues.
  - Un dernier test qui permettra de voir si l'agenda réagit correctement aux réponses inattandues.
Pour l'executer : expect -f agenda.exp.

Pour l'utiliser, il suffit de prendre les couples CPP et HPP, de les mettres dans le dossier de votre projet et d'écrire : #include "agenda.hpp" et #include "date.hpp".

Voici un exemple d'utilisation :

#include "date.hpp"
#include "agenda.hpp"

int main()
{
  agenda::Agenda agenda;

  agenda::demander_charger_agenda(agenda);

  return 0;
}

Développeur : Perret William.

