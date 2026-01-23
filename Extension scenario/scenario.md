# Scénario d'extraction de malware

## Etape 0 : Préparation de l'environnement

Tout d'abord, utiliser `make clean`  
puis `make` pour compiler les outils nécessaires.

## Étape 1 : Lancement du faux malware

Lancer le faux malware en arrière-plan :

```bash
python3 malware.py &
```

**Contenu du malware :**
- Une information sensible que l'on va essayer d'extraire
- Une socket d'écoute réseau

---

## Étape 2 : Lancement de myenv

```bash
sudo ./myenv
```

---

## Étape 3 : Vérification de la charge moyenne

```bash
myinfo
```

> **Note :** Un vrai malware pourrait affecter la charge moyenne, mais pour la présentation, on ne fera pas ça.

---

## Étape 4 : Identification du port suspect avec mynestat

```bash
mynetstat
```

**Observations :**
- Le port `0.0.0.0:4444` est écouté par un script `python3`
- On note le **PID du programme suspect** (ex : `9455`)

---

## Étape 5 : Recherche du processus avec mypstree

```bash
mypstree
```

**Résultat attendu :**
```
│               ├─gnome-terminal-(9414)─┬─bash(9423)─┬─python3(9455)
│               │                       │            └─sudo(9468)───+
│               │                       ├─{gnome-terminal-}(9415)
│               │                       ├─{gnome-terminal-}(9416)
│               │                       ├─{gnome-terminal-}(9417)
│               │                       ├─{gnome-terminal-}(9418)
│               │                       └─{gnome-terminal-}(9421)
```

**Analyse :**
- Le processus est lié au terminal actif
- Fermer le terminal suffirait à l'arrêter
- Un vrai script aurait pu faire un `fork` pour mieux se déguiser

---

## Étape 6 : Vérification de l'environnement du système

```bash
myenv -p [PID]
```

**Informations obtenues :**
- ✓ Confirmation que le script a été lancé par une session `bash`
- ✓ Identification de l'utilisateur (pas d'administrateur)
- ✓ Variables d'environnement pour les couleurs utilisés par `ls`
- ✓ Dossier relativement "normal"

> **Observation :** Le malware ne s'est pas caché excessivement, contenant beaucoup d'informations révélatrices

---

## Étape 7 : Cartographie mémoire du processus

```bash
mymaps -p [PID]
```

**Objectif :** Identifier les zones mémoire appropriées pour utiliser `mydump`

**Zones identifiées :**

### Zone 1 : Heap
```
00400000-00420000 r--p 00000000 08:02 27919054                           /usr/bin/python3.12
00420000-00703000 r-xp 00020000 08:02 27919054                           /usr/bin/python3.12
00703000-00a27000 r--p 00303000 08:02 27919054                           /usr/bin/python3.12
00a27000-00a28000 r--p 00627000 08:02 27919054                           /usr/bin/python3.12
00a28000-00ba6000 rw-p 00628000 08:02 27919054                           /usr/bin/python3.12
00ba6000-00baa000 rw-p 00000000 00:00 0 

      ZONE INTÉRESSANTE :
0e49a000-0e5b7000 rw-p 00000000 00:00 0                                  [heap]
```

### Zone 2 : Données
```
7a89d4b00000-7a89d4c00000 rw-p 00000000 00:00 0 
7a89d4c00000-7a89d5176000 r--p 00000000 08:02 27918544                   /usr/lib/locale/locale-archive
7a89d51df000-7a89d5200000 rw-p 00000000 00:00 0 
7a89d5200000-7a89d5228000 r--p 00000000 08:02 27929658                   /usr/lib/x86_64-linux-gnu/libc.so.6
7a89d5228000-7a89d53b0000 r-xp 00028000 08:02 27929658                   /usr/lib/x86_64-linux-gnu/libc.so.6
7a89d53b0000-7a89d53ff000 r--p 001b0000 08:02 27929658                   /usr/lib/x86_64-linux-gnu/libc.so.6
7a89d53ff000-7a89d5403000 r--p 001fe000 08:02 27929658                   /usr/lib/x86_64-linux-gnu/libc.so.6
7a89d5403000-7a89d5405000 rw-p 00202000 08:02 27929658                   /usr/lib/x86_64-linux-gnu/libc.so.6
7a89d5405000-7a89d5412000 rw-p 00000000 00:00 0 

      ZONE INTÉRESSANTE :
7a89d5423000-7a89d5566000 rw-p 00000000 00:00 0
```

### Zone 3 : Stack
```
7a89d5566000-7a89d556a000 r--p 00000000 08:02 27918446                   /usr/lib/x86_64-linux-gnu/libexpat.so.1.9.1
[... bibliothèques ...]
7a89d56b6000-7a89d56b8000 rw-p 00000000 00:00 0 
7a89d56b8000-7a89d56b9000 r--p 00000000 08:02 27929655                   /usr/lib/x86_64-linux-gnu/ld-linux-x86-64.so.2

       ZONE INTÉRESSANTE :
7ffd34de5000-7ffd34e06000 rw-p 00000000 00:00 0                          [stack]

7ffd34f6e000-7ffd34f72000 r--p 00000000 00:00 0                          [vvar]
7ffd34f72000-7ffd34f74000 r-xp 00000000 00:00 0                          [vdso]
ffffffffff600000-ffffffffff601000 --xp 00000000 00:00 0                  [vsyscall]
```

**Pourquoi ces zones ?**
- Python stocke ses données dans : le tas, la pile ou un `rw` anonyme
- Ces zones sont suspectes car :
  - Elles sont relativement haut dans la pile
  - Elles sont particulièrement grosses
  - Ce sont des zones `rw` anonymes

> **Note :** Il faudra faire plusieurs entraînements pour voir si on est capable de le retrouver

---

## Étape 8 : Extraction des données avec mydump

Effectuer des dumps sur les zones identifiées :

```bash
myenv > mydump -p 9133 --start 0x0e49a000 --end 0x0e5b7000 -o heap.bin
myenv > mydump -p 9133 --start 0x7a89d5423000 --end 0x7a89d5566000 -o data.bin
myenv > mydump -p 9133 --start 0x7ffd34de5000 --end 0x7ffd34e06000 -o stack.bin
```

| Zone | Adresse | Fichier |
|------|---------|---------|
| Heap | `0x0e49a000` → `0x0e5b7000` | `heap.bin` |
| Data | `0x7a89d5423000` → `0x7a89d5566000` | `data.bin` |
| Stack | `0x7ffd34de5000` → `0x7ffd34e06000` | `stack.bin` |

---

## Étape 9 : Analyse des dumps mémoire

:warning: Copier-coller les commandes suivantes dans le terminal.

**Recherche d'informations sensibles dans les dumps :**
```bash
strings heap.bin | less
strings data.bin | less
strings stack.bin | less
```

**Recherche de patterns spécifiques (adresses IP, ports, etc.) :**
```bash
strings heap.bin | grep "4444"
strings heap.bin | grep "0.0.0.0"
```

> **Note importante :** Dans ce scénario simplifié avec Python, les constantes de chaînes sont optimisées par l'interpréteur et stockées dans des sections read-only du code, pas dans la heap/stack modifiable. Un vrai malware compilé (C/C++) stockerait ses secrets dans des zones mémoire plus facilement extractibles.
>
> **En pratique réelle :** Un analyste utiliserait des outils spécialisés comme:
> - `volatility` pour l'analyse forensique mémoire
> - `gdb` pour l'inspection en temps réel
> - `strings` avec des filtres plus sophistiqués
> - Analyse de l'historique bash, des connexions réseau actives, etc.

**Résultat attendu :** Démonstration de la méthode d'extraction mémoire, même si le secret Python n'est pas directement visible. ✓

---

## Conclusion

Ce scénario démontre une approche systématique pour :
1. Identifier un processus suspect
2. Analyser son environnement
3. Cartographier sa mémoire
4. Extraire les données sensibles
5. Localiser les informations compromises
