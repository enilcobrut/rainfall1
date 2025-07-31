
#include <stdio.h>
#include <stdlib.h>

/* Spawn a shell and terminate. */
static void o(void)
{
    system("/bin/sh");
    _exit(1);
}

static void n(void)
{
    char buffer[520];

    fgets(buffer, sizeof(buffer), stdin);
    printf(buffer);               /* format string vulnerability */
    exit(1);
}

int main(void)
{
    n();
    return 0;
}