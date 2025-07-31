
#include <stdio.h>
#include <stdlib.h>

extern int m;

static void p(char *s)
{
    printf(s);                         /* vulnerable printf */
}

static void n(void)
{
    char buffer[520];

    fgets(buffer, sizeof(buffer), stdin);
    p(buffer);
    if (m == 0x1025544) {
        system("/bin/cat /home/user/level5/.pass");
    }
}

int main(void)
{
    n();
    return 0;
}