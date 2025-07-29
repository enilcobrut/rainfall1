# 🌧️ RAINFALL - Projet de Sécurité

## 📋 Présentation

**Rainfall** est un projet de sécurité informatique qui propose une série de **10 niveaux d'exploitation** + **4 niveaux bonus**, conçus pour apprendre et pratiquer les techniques de **reverse engineering** et d'**exploitation de vulnérabilités**.

### Objectifs du projet
- **Apprendre** les vulnérabilités classiques (buffer overflow, format string, etc.)
- **Pratiquer** l'analyse de binaires avec des outils comme Ghidra, GDB
- **Maîtriser** les techniques d'exploitation en environnement contrôlé
- **Comprendre** la gestion mémoire en C (stack, heap, GOT, etc.)

### Niveaux disponibles
- **Niveaux 0-9** : Vulnérabilités progressives
- **Bonus 0-3** : Techniques avancées
- **End** : Niveau final (⚠️ Ne pas tenter de devenir root !)

## 🛠️ Fichiers du projet

### 📁 Structure
```
rainfall_intra/
├── level0/          # Niveau 0 - Argument simple
├── level1/          # Niveau 1 - Buffer overflow classique
├── level2/          # Niveau 2 - Heap exploitation
├── level3/          # Niveau 3 - Format string basique
├── level4/          # Niveau 4 - Format string optimisé
├── level5/          # Niveau 5 - GOT overwrite
├── level6/          # Niveau 6 - Function pointer overwrite
├── level7/          # Niveau 7 - Double heap overflow
├── level8/          # Niveau 8 - Use After Free
├── level9/          # Niveau 9 - À découvrir
├── bonus0-3/        # Niveaux bonus
├── TOOLS.md         # Outils et scripts d'automatisation
├── UTILS.md         # Mémoire en C et concepts techniques
├── start_iso.sh     # Script de démarrage de la VM
├── scp.sh           # Script de copie automatique
└── ssh.sh           # Script de connexion automatique
```

### 📄 Fichiers de documentation

#### **TOOLS.md**
Contient tous les outils et scripts pour faciliter l'exploitation :
- **Scripts automatisés** : `./ssh.sh` et `./scp.sh` pour la navigation
- **Outils d'analyse** : CYCLIC, commandes FIND
- **Scripts de démarrage** : `start_iso.sh` pour la VM

#### **UTILS.md**
Guide technique sur la mémoire en C :
- **Stack (Pile)** : Variables locales, adresses de retour
- **Heap (Tas)** : Allocations dynamiques, malloc/free
- **Concepts** : Gestion mémoire, vulnérabilités

## 🚀 Quick Start

### 1. Démarrage de la VM

```bash
# Démarrer la VM Rainfall
./start_iso.sh

# Vérifier que la VM est accessible
ping -c 1 localhost
```

### 2. Première connexion (Level 0)

```bash
# Se connecter au level 0
./ssh.sh 0

# Le script utilise automatiquement le mot de passe "level0"
# Vous êtes maintenant connecté à la VM
```

### 3. Exploitation du Level 0

```bash
# Dans la VM, exécuter le binaire
./level0 423

# Récupérer le flag
cat /home/user/level1/.pass
```

### 4. Copie des fichiers

```bash
# Copier le répertoire home du level 0
./scp.sh.sh 0

# Cela crée un dossier level0_home/ avec tous les fichiers
```

### 5. Navigation vers le niveau suivant

```bash
# Se connecter au level 1 avec le flag du level 0
./ssh.sh 1

# Le script utilise automatiquement le flag du level 0 comme mot de passe
```

## 📚 Exemple complet Level 0 → Level 1

```bash
# 1. Démarrage
./start_iso.sh

# 2. Connexion level 0
./ssh.sh 0

# 3. Exploitation
./level0 423
# → Obtient un shell avec privilèges level1

# 4. Récupération du flag
cat /home/user/level1/.pass
# → 1fe8a524fa4bec01ca4ea2a869af2a02260d4a7d5fe7e7c24d8617e6dca12d3a

# 5. Copie des fichiers
./scp.sh 0

# 6. Connexion au niveau suivant
./ssh.sh 1
# → Utilise automatiquement le flag comme mot de passe
```

