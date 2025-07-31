
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

/*
 * This program expects an integer argument.  If the value equals
 * 0x1a7 (423) a new shell is executed with the current privileges.
 */
int main(int argc, char **argv)
{
    if (argc < 2)
        return 0;

    int value = atoi(argv[1]);
    if (value == 423) {
        char *args[] = {"/bin/sh", NULL};

        gid_t gid = getegid();
        uid_t uid = geteuid();
        setresgid(gid, gid, gid);
        setresuid(uid, uid, uid);
        execv(args[0], args);
    } else {
        fwrite("No !\n", 1, 5, stderr);
    }

    return 0;
}