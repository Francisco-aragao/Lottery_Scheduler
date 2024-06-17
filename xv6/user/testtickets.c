#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/pstat.h"
#include "kernel/fcntl.h"

void forkAndExec(char* numTickets, int pids[], int idx) {
    int pid = fork();
    if (pid < 0) {
        fprintf(2, "fork fail\n");
        exit(1);
    }
    
    if (pid == 0) { //child
        //execute ticket spam
        char *args[3] = {"", numTickets, 0};
        exec("ticketspam", args);
    } else { 
        //register child pid
        fprintf(2, "pid %d\n", pid);
        pids[idx] = pid;
    }
}

int main(int argc, char *argv[]) {
    if (argc < 4){
        fprintf(2, "testtickets usage: [number of csv lines] [number of assigned tickets to this program] [number of assigned tickets per instance..]\n");
        exit(1);
    }

    int fd;
    int pids[argc - 3];
    int idxs[argc - 3];
    struct pstat pstat;

    settickets(atoi(argv[2]));

    if((fd = open("./testtickets.csv", O_CREATE|O_WRONLY)) < 0){
        printf("testtickets: cannot open testtickets.csv\n");
        exit(1);
    }

    for (int i=3; i < argc; i++) {
        //Run a ticketspam
        forkAndExec(argv[i], pids, i - 3);

        if (i == argc - 1) fprintf(fd, "pid_%d_tickets, pid_%d_ticks\n", pids[i - 3], pids[i - 3]);
        else fprintf(fd, "pid_%d_tickets, pid_%d_ticks, ", pids[i - 3], pids[i - 3]);

        //Find pstat index
        getpinfo(&pstat);
        int idx = 0;
        while (pstat.pid[idx] != pids[i-3]) idx++;
        idxs[i-3] = idx;
    }

    int prev = -1, runs = atoi(argv[1]);
    for (int i=0; i < runs; i++) {
        for (;;) {
            getpinfo(&pstat);
            if (pstat.ticks[idxs[0]] != prev) {
                printf("%d %d \n", pstat.ticks[idxs[0]], prev);
                break;
            }
        } //Wait for update

        //Record to csv
        for (int i=0; i < argc - 3; i++) {
            int idx = idxs[i];
            printf("Estou escrevendo!\n");
            if (i == argc - 4) fprintf(fd, "%d, %d\n", pstat.tickets[idx], pstat.ticks[idx]);
            else fprintf(fd, "%d, %d, ", pstat.tickets[idx], pstat.ticks[idx]);
        }
        prev = pstat.ticks[idxs[0]];
    }
    
    close(fd); //close file
    for (int i = 0; i < argc - 3; i++) {
        kill(pids[i]);
    }
    exit(0);
}