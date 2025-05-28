#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/time.h>

#define MAX_EVENTS 100

typedef struct {
    int pid;
    int signal_num;
    struct timeval timestamp;
} Event;

Event events[MAX_EVENTS];
int event_count = 0;

void handle_signal(int sig, siginfo_t *info, void *context) {
    if (event_count >= MAX_EVENTS) return;
    gettimeofday(&events[event_count].timestamp, NULL);
    events[event_count].pid = info->si_pid;
    events[event_count].signal_num = sig;
    event_count++;
}

int compare_events(const void *a, const void *b) {
    const Event *ea = a, *eb = b;
    if (ea->timestamp.tv_sec == eb->timestamp.tv_sec)
        return ea->timestamp.tv_usec - eb->timestamp.tv_usec;
    return ea->timestamp.tv_sec - eb->timestamp.tv_sec;
}

void print_timeline() {
    qsort(events, event_count, sizeof(Event), compare_events);
    printf("Timeline:\n");
    FILE *dot = fopen("timeline.dot", "w");
    fprintf(dot, "digraph Timeline {\n");
    for (int i = 0; i < event_count; ++i) {
        printf("PID: %d | Signal: %d | Time: %ld.%06ld\n",
               events[i].pid, events[i].signal_num,
               events[i].timestamp.tv_sec, events[i].timestamp.tv_usec);
        fprintf(dot, "  e%d [label=\"PID %d\\nSIG %d\\n%ld.%06ld\"];\n",
                i, events[i].pid, events[i].signal_num,
                events[i].timestamp.tv_sec, events[i].timestamp.tv_usec);
        if (i > 0) fprintf(dot, "  e%d -> e%d;\n", i - 1, i);
    }
    fprintf(dot, "}\n");
    fclose(dot);
}

int main() {
    struct sigaction sa = {
        .sa_sigaction = handle_signal,
        .sa_flags = SA_SIGINFO
    };
    sigemptyset(&sa.sa_mask);
    sigaction(SIGUSR1, &sa, NULL);
    sigaction(SIGUSR2, &sa, NULL);

    pid_t p1 = fork();
    if (p1 == 0) {
        sleep(1);
        kill(getppid(), SIGUSR1);
        exit(0);
    }

    pid_t p2 = fork();
    if (p2 == 0) {
        sleep(2);
        kill(getppid(), SIGUSR2);
        exit(0);
    }

    sleep(3);
    print_timeline();
    return 0;
}
