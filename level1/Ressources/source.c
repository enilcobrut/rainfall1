
#include <stdio.h>
#include <stdlib.h>

/*
 * Helper executed from the exploitation process.  It simply prints
 * a message and spawns a shell.
 */
void run(void)
{
    fwrite("Good... Wait what?\n", 1, 19, stdout);
    system("/bin/sh");
}

int main(void)
{
    /* Vulnerable buffer used by the original challenge */
    char buffer[76];

    gets(buffer);
    return 0;
}

