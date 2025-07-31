
#include <stdio.h>
#include <stdlib.h>

/*
 * Reads a string from stdin and prints it back.  The original
 * challenge inspected the return address to detect exploitation,
 * which is approximated here using __builtin_return_address.
 */
static void p(void)
{
    char buffer[76];

    fflush(stdout);
    gets(buffer);

    unsigned int ret = (unsigned int)__builtin_return_address(0);
    if ((ret & 0xb0000000) == 0xb0000000) {
        printf("(%p)\n", (void *)ret);
        _exit(1);
    }

    puts(buffer);
    strdup(buffer);
}

int main(void)
{
    p();
    return 0;
}
