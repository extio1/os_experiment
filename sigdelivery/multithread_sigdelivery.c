#define _GNU_SOURCE

#include <pthread.h>
#include <stdio.h>
#include <signal.h>
#include <string.h>

/* 
 * Changing SIGINT to SIGRTMIN (which is real-time) will force
 * signal queue to fill up and throw the error, instead of SIGINT.
 * 
 * Examine $ watch -d -n1 "cat /proc/pid/status | grep Sig"
 */
#define SIGNUM SIGINT 


// handler may contain non-reentrant functions
void
signal_handler(int signum, siginfo_t* info, void* )
{
    sigval_t sigval = info->si_value;

    time_t mytime = time(NULL);
    char* time = ctime(&mytime);
    time[strlen(time)-1] = '\0';

    if( info->si_code == SI_QUEUE ){
        write(1, "SI_QUEUE installed\n", 20);
        printf("[%s] tid :[%d], sigval.sival_int - %d; sigval.sival_ptr - %s\n",
                time, gettid(), sigval.sival_int, (char*)sigval.sival_ptr);
    } else {
        write(1, "SI_QUEUE NOT installed\n", 24);
    }


}


void*
thread_routine(void*)
{
    sigset_t set;
    sigval_t sigval = {.sival_ptr = "hello"}; // data'll be transferred to sighandler

    printf("thread created: pid [%d], ppid[%d], tid [%d]\n", getpid(), getppid(), gettid());

    if( sigemptyset(&set) != 0){
        perror("sigemptyset() error");
        return NULL;
    }

    if(sigaddset(&set, SIGNUM) != 0){
        perror("sigaddset() error");
        return NULL;
    }

    if(pthread_sigmask(SIG_BLOCK, &set, NULL) != 0){
        perror("pthread_sigmask() error");
        return NULL;
    }

    while(1){
        if( sigqueue(getpid(), SIGNUM, sigval) != 0 ){
            perror("sigqueue() error");
        }
        //sleep(1);
    }

    return NULL;
}


int
main(int argc, char** argv)
{
    pthread_t tid;

    struct sigaction action;
    action.sa_flags = SA_SIGINFO;
    action.sa_sigaction = signal_handler;

    printf("main:  pid [%d], ppid[%d], tid [%d]\nwatch -d -n1 \"cat /proc/%d/status | grep Sig\" \n",
            getpid(), getppid(), gettid(), getpid());
    sleep(10);

    if( sigaction(SIGNUM, &action, NULL) != 0 ){
        perror("sigaction() error");
        return -1;
    }

    if( pthread_create(&tid, NULL, thread_routine, NULL) != 0){
        perror("pthread_create() error");
        return -1;
    }

    // just living
    while(1){
        printf("ALIVE\n");
        sleep(1);
    }

}
