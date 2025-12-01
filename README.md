<h1> Projet Systèmes 2025/26 </h1>


- [\[1ère partie\] Implémentation](#1ère-partie-implémentation)
  - [A. Le Cœur : Le Mini-Shell](#a-le-cœur--le-mini-shell)
    - [Fonctionnalité de base](#fonctionnalité-de-base)
    - [Fonctionnalités clés](#fonctionnalités-clés)
  - [B. Les "Alias" : Commandes Emballées](#b-les-alias--commandes-emballées)
  - [C. Les "Outils Maison" : Implémentation](#c-les-outils-maison--implémentation)
  - [D. Les Bonus](#d-les-bonus)
- [\[2ème Partie\] Scénario de tests](#2ème-partie-scénario-de-tests)

---

Le projet demande de construire deux choses principales :
1.  Un **interpréteur de commandes** (un mini-shell) qui gère les bases de la redirection et de l'enchaînement de commandes.
2.  Une **série d'outils** (les commandes `my...`) spécialisés dans l'analyse "forensics" (analyse système et mémoire), qui s'appuient majoritairement sur le système de fichiers `/proc`.

---
---

# [1ère partie] Implémentation

> [!IMPORTANT] Tests de Robustesse du shell et Revue de l’exécution des commandes simples:
> - Validation des appels dup2, close, waitpid et close() systématique
> - Tests des redirections combinées (ex. cat < f1 > f2) - on se limitera à 2 redirections max sinon afficher un message d'erreur
> - Validation du comportement du shell dans les cas d’erreur (fichier manquant, permission, etc.)  et gestion des erreurs (errno, perror)


## A. Le Cœur : Le Mini-Shell

Coder le "cerveau" du programme. Ce n'est pas une commande, c'est le programme principal qui lit ce que l'utilisateur tape et décide quoi faire.

### Fonctionnalité de base 
Il doit pouvoir lancer n'importe quelle commande externe (comme `ls -l`, `grep toto`, `cat fichier.txt`, etc.). Pour cela, il utilise la mécanique standard `fork()` (pour créer un nouveau processus) puis `exec()` (pour remplacer ce nouveau processus par la commande demandée).

### Fonctionnalités clés

* **Pipes (`|`) :** Permettre d'enchaîner des commandes (ex: `ls -l | grep .txt`). Cela se fait en créant un "tube" (avec `pipe()`) et en "branchant" la sortie (stdout) de la première commande sur l'entrée (stdin) de la seconde (avec `dup2()`).  
  :warning: On se limite à 1 pipe et 2 commandes sinon ERROR
* **Redirections (`>`, `<`) :** Permettre de rediriger la sortie vers un fichier (ex: `ls > out.txt`) ou de lire l'entrée depuis un fichier (ex: `sort < in.txt`). Cela se fait aussi avec `dup2()` pour remplacer stdin ou stdout par un descripteur de fichier.  
  :warning: 2 redirections max sinon ERROR
* **Bonus :** Gérer les tâches de fond (`&`) et attendre (`wait`/`waitpid`) correctement la fin des commandes lancées en avant-plan.

---

## B. Les "Alias" : Commandes Emballées

Ces commandes ne sont **pas à réimplémenter**. Le mini-shell doit simplement les *reconnaître* par leur nom (`myps`, `mypstree`, etc.) et exécuter la *vraie* commande Linux correspondante avec les bons arguments.

Ce sont essentiellement des raccourcis ou des "alias" codés en dur dans ton shell.

* **`myps`** doit lancer `ps` avec les arguments spécifiques pour afficher ce qui est demandé. (`PID PPID UID VSZ(<unité>) RSS(<unité>) CMD `)
    * *Note :* La "dernière ligne somme" est un peu plus complexe. Tu devras peut-être exécuter `ps ... | awk '...'` pour faire le calcul, ou créer un petit script shell séparé que ton mini-shell exécutera.
* **`mypstree`** doit lancer `pstree -p`.
* **`mynetstat`** doit lancer `netstat -tunap`.
* **`myarp`** doit lancer `arp -n`.
* **`myexe`** doit lancer une commande (que tu dois définir, par exemple `find / -type f -exec file {} + | grep ELF`) pour trouver des exécutables.

---

## C. Les "Outils Maison" : Implémentation

Ici, c'est le coeur de la partie "forensics". Ces commandes sont **à coder soi-même**. Elles seront probablement des fonctions *internes* au shell ou des petits programmes C séparés que le shell exécutera (comme les autres).

Elles se basent toutes sur la lecture de fichiers virtuels dans le répertoire `/proc/<PID>/`.

* **`myinfo` :** <Détails à venir>, mais affichera sûrement des infos de `/proc/cpuinfo`, `/proc/meminfo`, `/proc/version`, etc.
* **`myenv -p <PID>` :**
    * **Action :** Tu dois lire le fichier `/proc/<PID>/environ`.
    * **Difficulté :** Ce n'est pas un fichier texte classique. Les variables sont séparées par des caractères nuls (`\0`) au lieu de sauts de ligne. Ton programme C devra lire ce fichier et remplacer les `\0` par des `\n` pour les afficher "proprement".
* **`mymaps -p <PID>` :** <Détails à venir>, mais lira certainement `/proc/<PID>/maps` pour afficher les segments de mémoire alloués au processus (code, données, pile, tas, bibliothèques).
* **`mydump -p <PID> ...` :** <Détails à venir>, mais lira très probablement `/proc/<PID>/mem` pour extraire ("dumper") une plage de mémoire brute du processus vers un fichier.

---

## D. Les Bonus

Ce sont des commandes "maison" supplémentaires, dans la même logique que la section C (analyse de `/proc`).

* **`mylof -p <PID>` :** Le nom ressemble à "List Open Files" (comme `lsof`). Cela impliquerait de lister le contenu du répertoire `/proc/<PID>/fd/` (qui contient des liens symboliques vers les fichiers ouverts par le processus).
* **`mydelexe` :** <À décrire>, peut-être une commande pour supprimer un exécutable en cours d'exécution de manière "furtive" (en utilisant `unlink` pendant qu'il tourne).

--- 
---

# [2ème Partie] Scénario de tests

Cette étape doit être menée en parallèle des différentes étapes précédentes. Il ne faut pas attendre la fin de l’étape 3 pour y réfléchir, afin de faire mûrir le scénario au fil de la progression technique du projet.

L'objectif est de concevoir un scénario de mise en situation du mini-shell et des commandes implémentées, dans un contexte de forensics mémoire, afin d’enrichir la soutenance finale par une démonstration concrète.

Le scénario peut être très simple mais crédible. Il doit être productible et doit permettre de montrer comment les commandes réalisées permettent de mieux observer, diagnostiquer ou comprendre l’activité d’un système Linux.

Le scénario peut s’appuyer sur :

- un environnement isolé : machines virtuelles sous Linux soit avec vos machines ou avec le CyberRange  DIATEAM - une topologie isolée de 2 machines "cible" et "analyste" est suffisante ;
- un jeu de processus simulant une activité suspecte ou anormale (ex. un script qui lance plusieurs processus, écoute sur un port, consomme la mémoire, etc.) ;
- ....

Ne pas hésiter à en parler avec les profs

