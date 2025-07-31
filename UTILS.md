# MEMOIRE EN C: 


##  a) La pile (Stack)
Utilisation :
La pile est utilisée pour stocker les variables locales, les adresses de retour des fonctions et les registres sauvegardés lors des appels de fonctions.

### Caractéristiques :

Structure en pile (LIFO) : La dernière donnée poussée est la première à être récupérée.
Taille limitée : Chaque thread a une taille de pile définie qui peut être dépassée par une allocation trop importante ou une récursion excessive.
Gestion automatique : La mémoire sur la pile est allouée et libérée automatiquement à l’entrée et à la sortie d’une fonction.

## b) Le tas (Heap)

Utilisation :

Le tas est une zone de mémoire destinée aux allocations dynamiques (via malloc, calloc, realloc, etc.).

### Caractéristiques :

Allocation manuelle : Le programmeur doit gérer l’allocation et la libération (avec free).
Taille plus grande : Le tas dispose généralement de beaucoup plus d’espace que la pile, mais reste sujet à la fragmentation.
Ordre libre : L’accès et la libération ne suivent pas une organisation LIFO.
Note : Dans le code fourni, seule la pile est utilisée (pour la variable locale), le tas n’est pas mobilisé.




![alt text](image.png)