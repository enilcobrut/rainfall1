#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

static void p(char *buf, const char *prompt)
{
    char tmp[0x1000];

    puts(prompt);
    read(0, tmp, sizeof(tmp));
    char *nl = strchr(tmp, '\n');
    if (nl)
        *nl = '\0';
    strncpy(buf, tmp, 0x14);
}

static void pp(char *out)
{
    char buf1[20];
    char buf2[20];

    p(buf1, "-- ");
    p(buf2, "-- ");

    strcpy(out, buf1);
    size_t len = strlen(out);
    out[len] = ' ';
    out[len + 1] = '\0';
    strcat(out, buf2);
}

int main(void)
{
    char msg[54];
    pp(msg);
    puts(msg);
    return 0;
}