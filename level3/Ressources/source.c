
#include <stdio.h>
#include <stdlib.h>

extern int m;

/*
 * Reads a line from stdin and prints it back using printf directly,
 * creating a classic format string vulnerability.  If the global
 * variable `m` equals 0x40, a shell is spawned.
 */
static void v(void)
{
    char buffer[520];

    fgets(buffer, sizeof(buffer), stdin);
    printf(buffer);                 /* intentional vulnerability */
    if (m == 0x40) {
        fwrite("Wait what?!\n", 1, 12, stdout);
        system("/bin/sh");
    }
}

int main(void)
{
    v();
    return 0;
}
