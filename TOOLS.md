# OUTILS POUR LE PROJET RAINFALL

## CYCLIC

Pour utiliser CYCLIC (python)

alias cyclic='python3 -c "from pwn import *; import sys; print(cyclic(int(sys.argv[1])).decode())"'
alias cyclic_find='python3 -c "from pwn import *; import sys; print(cyclic_find(int(sys.argv[1], 0)))"'

---

## COMMANDE FIND

Pour trouver les .pass

find / -name ".pass" -type f 2>/dev/null

## ./scp.sh

Script automatisé pour copier les fichiers home d'un niveau via SCP.

**Usage :**
```bash
./scp.sh [NIVEAU]
```

**Fonctionnement :**
- Utilise le flag du niveau précédent comme mot de passe
- Copie le répertoire home du niveau spécifié
- Gère automatiquement les niveaux reguliers (0-9) et bonus (10-13)
- Crée un dossier `levelX_home/` ou `bonusX_home/`

**Exemples :**
```bash
./scp.sh 1    # Copie level1 avec le mot de passe de level0
./scp.sh 5    # Copie level5 avec le flag de level4
./scp.sh 10   # Copie bonus0 avec le flag de level9
```

**Cas spéciaux :**
- Level 0 : Utilise "level0" comme mot de passe
- Level 10-13 : Correspond aux bonus0-bonus3

## ./ssh.sh

Script automatisé pour se connecter à un niveau via SSH.

**Usage :**
```bash
./ssh.sh [NIVEAU]
```

**Fonctionnement :**
- Utilise le flag du niveau précédent comme mot de passe
- Se connecte automatiquement au niveau spécifié
- Gère automatiquement les niveaux reguliers (0-9) et bonus (10-13)
- Port SSH : 2222

**Exemples :**
```bash
./ssh.sh 1    # Se connecte à level1 avec le mot de passe de level0
./ssh.sh 5    # Se connecte à level5 avec le flag de level4
./ssh.sh 10   # Se connecte à bonus0 avec le flag de level9
```

**Cas spéciaux :**
- Level 0 : Utilise "level0" comme mot de passe
- Level 10-13 : Correspond aux bonus0-bonus3

**Prérequis :**
- `sshpass` installé sur le système
- Flags des niveaux précédents stockés dans `./levelX/flag`

## ./start_iso.sh

Script automatisé pour démarrer la VM Rainfall avec QEMU.

**Usage :**
```bash
./start_iso.sh
```

**Fonctionnement :**
- Démarre la VM Rainfall avec QEMU
- Configure la mémoire (4GB) et les CPU (4 cores)
- Active KVM pour les performances
- Configure le port forwarding (2222 → 4242)
- Démarre en mode daemon (arrière-plan)

**Configuration :**
- **Mémoire** : 4GB
- **CPU** : 4 cores avec KVM
- **Port** : 2222 (hôte) → 4242 (VM)
- **ISO** : ~/Downloads/RainFall.iso

**Vérification :**
```bash
# Démarrer la VM
./start_iso.sh

# Vérifier que la VM est accessible
ping -c 1 localhost

# Vérifier le port SSH
nc -zv localhost 2222
```

**Prérequis :**
- QEMU installé (`qemu-system-x86_64`)
- KVM activé sur le système
- ISO Rainfall dans `~/Downloads/RainFall.iso`