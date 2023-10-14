#define _GNU_SOURCE
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>

// handler may contain non-reentrant functions
void
signal_handler(int signum, siginfo_t* info, void* )
{
    sigval_t sigval = info->si_value;

    time_t mytime = time(NULL);
    char* time = ctime(&mytime);
    time[strlen(time)-1] = '\0';

    if( info->si_code == SI_QUEUE ){ // if signal was sigqueue'd
        write(1, "SI_QUEUE installed\n", 20);
        printf("[%s] tid :[%d], sigval.sival_int - %d; sigval.sival_ptr - %d\n",
                time, gettid(), sigval.sival_int, *(int*)sigval.sival_ptr);
    } else {
        write(1, "SI_QUEUE NOT installed\n", 24);
    }

    sleep(2);
}

int
main(int argc, char** argv)
{
    int val = 42;
    //char *string = "hello";
    int *val_ptr = &val;

    struct sigaction action;
    action.sa_flags = SA_SIGINFO;
    action.sa_sigaction = signal_handler;

    sigval_t sigval = {.sival_int = val, .sival_ptr = val_ptr};
    if( sigaction(SIGINT, &action, NULL) != 0 ){
        perror("sigaction() error");
        return -1;
    }

    printf("main:  pid [%d], ppid[%d], tid [%d]\nwatch -d -n1 \"cat /proc/%d/status | grep Sig\" \n",
        getpid(), getppid(), gettid(), getpid());
    sleep(10);
    while(1){
        if( sigqueue(getpid(), SIGINT, sigval) != 0 ){
            perror("sigqueue() error");
        }
        //sleep(1);
    }

   sleep(100);
}