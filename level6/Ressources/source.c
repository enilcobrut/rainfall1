#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Print the content of the next level's password file */
static void n(void)
{
    system("/bin/cat /home/user/level7/.pass");
}

/* Default function used by a function pointer */
static void m(void)
{
    puts("Nope");
}

int main(int argc, char **argv)
{
    char *buffer = malloc(0x40);
    void (**fp)(void) = malloc(sizeof(*fp));
    *fp = m;
    strcpy(buffer, argv[1]);
    (*fp)();
    return 0;
}