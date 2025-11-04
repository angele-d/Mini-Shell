<h1> Projet Systèmes 2025/26 </h1>


- [A. Le Cœur : Le Mini-Shell](#a-le-cœur--le-mini-shell)
  - [Fonctionnalité de base](#fonctionnalité-de-base)
  - [Fonctionnalités clés](#fonctionnalités-clés)
- [B. Les "Alias" : Commandes Emballées](#b-les-alias--commandes-emballées)
- [C. Les "Outils Maison" : Implémentation](#c-les-outils-maison--implémentation)
- [D. Les Bonus](#d-les-bonus)
- [En résumé](#en-résumé)

---

Le projet te demande de construire deux choses principales :
1.  Un **interpréteur de commandes** (un mini-shell) qui gère les bases de la redirection et de l'enchaînement de commandes.
2.  Une **série d'outils** (les commandes `my...`) spécialisés dans l'analyse "forensics" (analyse système et mémoire), qui s'appuient majoritairement sur le système de fichiers `/proc`.

---

Voici une explication détaillée de chaque section :

## A. Le Cœur : Le Mini-Shell

Coder le "cerveau" de ton programme. Ce n'est pas une commande, c'est le programme principal qui lit ce que l'utilisateur tape et décide quoi faire.

### Fonctionnalité de base 
Il doit pouvoir lancer n'importe quelle commande externe (comme `ls -l`, `grep toto`, `cat fichier.txt`, etc.). Pour cela, il utilise la mécanique standard `fork()` (pour créer un nouveau processus) puis `exec()` (pour remplacer ce nouveau processus par la commande demandée).

### Fonctionnalités clés

* **Pipes (`|`) :** Permettre d'enchaîner des commandes (ex: `ls -l | grep .txt`). Cela se fait en créant un "tube" (avec `pipe()`) et en "branchant" la sortie (stdout) de la première commande sur l'entrée (stdin) de la seconde (avec `dup2()`).
* **Redirections (`>`, `<`) :** Permettre de rediriger la sortie vers un fichier (ex: `ls > out.txt`) ou de lire l'entrée depuis un fichier (ex: `sort < in.txt`). Cela se fait aussi avec `dup2()` pour remplacer stdin ou stdout par un descripteur de fichier.
* **Bonus :** Gérer les tâches de fond (`&`) et attendre (`wait`/`waitpid`) correctement la fin des commandes lancées en avant-plan.

---

## B. Les "Alias" : Commandes Emballées

Ces commandes ne sont **pas à réimplémenter**. Ton mini-shell doit simplement les *reconnaître* par leur nom (`myps`, `mypstree`, etc.) et exécuter la *vraie* commande Linux correspondante avec les bons arguments.

Ce sont essentiellement des raccourcis ou des "alias" codés en dur dans ton shell.

* **`myps`** doit lancer `ps` avec les arguments spécifiques pour afficher ce qui est demandé.
    * *Note :* La "dernière ligne somme" est un peu plus complexe. Tu devras peut-être exécuter `ps ... | awk '...'` pour faire le calcul, ou créer un petit script shell séparé que ton mini-shell exécutera.
* **`mypstree`** doit lancer `pstree -p`.
* **`mynetstat`** doit lancer `netstat -tunap`.
* **`myarp`** doit lancer `arp -n`.
* **`myexe`** doit lancer une commande (que tu dois définir, par exemple `find / -type f -exec file {} + | grep ELF`) pour trouver des exécutables.

---

## C. Les "Outils Maison" : Implémentation

Ici, c'est le cœur de la partie "forensics". Ces commandes sont **à coder toi-même**. Elles seront probablement des fonctions *internes* à ton shell ou des petits programmes C séparés que ton shell exécutera (comme les autres).

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

## En résumé 
Construis un mini-shell (Partie A) qui sert à la fois à lancer des commandes normales, des alias (Partie B) et ta propre suite d'outils forensics (Parties C et D) que tu dois coder en C en lisant `/proc`.

