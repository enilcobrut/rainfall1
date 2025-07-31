#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

static char c[68];

/* Print the stored string with a timestamp */
static void m(void)
{
    time_t now = time(NULL);
    printf("%s - %ld\n", c, now);
}

int main(int argc, char **av)
{
    /* allocate and prepare two structures */
    void **a = malloc(8);
    a[0] = (void *)1;
    a[1] = malloc(8);

    void **b = malloc(8);
    b[0] = (void *)2;
    b[1] = malloc(8);

    strcpy((char *)a[1], av[1]);
    strcpy((char *)b[1], av[2]);

    FILE *f = fopen("/home/user/level8/.pass", "r");
    fgets(c, sizeof(c), f);
    puts("~~");
    return 0;
}
