
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static char *auth;
static char *service;

/*
 * Simplified version of the original authentication logic.  The
 * program accepts simple commands on stdin and allows a shell when
 * a specific condition is met.
 */
int main(void)
{
    char buffer[128];

    while (1) {
        printf("%p, %p \n", auth, service);
        if (!fgets(buffer, sizeof(buffer), stdin))
            return 0;

        if (!strncmp(buffer, "auth ", 5)) {
            auth = malloc(4);
            *auth = '\0';
            if (strlen(buffer + 5) < 31)
                strcpy(auth, buffer + 5);
        }
        else if (!strncmp(buffer, "reset", 5)) {
            free(auth);
            auth = NULL;
        }
        else if (!strncmp(buffer, "service", 7)) {
            free(service);
            service = strdup(buffer + 7);
        }
        else if (!strncmp(buffer, "login", 5)) {
            if (auth && auth[8] == '\0') {
                fwrite("Password:\n", 1, 10, stdout);
            } else {
                system("/bin/sh");
            }
        }
    }
}