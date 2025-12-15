#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include <fcntl.h>
#include <sys/utsname.h>
#include <time.h>

bool alias_commands(char** cmd) {

    if(cmd[0] == NULL) {
        fprintf(stderr, "No command provided\n");
        return false;
    }
    // myps alias for ps
    // FIXME: Ask if it's what was asked
    else if(strcmp(cmd[0], "myps") == 0) {
        execvp("ps", (char* const[]){"ps", cmd[1], NULL});
        perror("execvp failed");
        exit(1);
        return true;
    }
    // mypstree alias for pstree -p
    else if(strcmp(cmd[0], "mypstree") == 0) {
        execvp("pstree", (char* const[]){"pstree", "-p", NULL});
        perror("execvp failed");
        exit(1);
        return true;
    }
    // mynetstat alias for netstat -tunap
    // FIXME: Ask professor if it's normal
    else if(strcmp(cmd[0], "mynetstat") == 0) {
        printf("---\nMYENV INFO > WARNING: this command may not work if net-tools is not installed\n---\n");
        execvp("netstat", (char* const[]){"netstat", "-tunap", NULL});
        perror("execvp failed");
        exit(1);
        return true;
    }
    // myarp alias for arp -n
    // FIXME: Ask professor if it's normal
    else if(strcmp(cmd[0], "myarp") == 0) {
        printf("---\nMYENV INFO > arp -n is deprecated, using ip neigh show instead.\n---\n");
        execvp("ip", (char* const[]){"ip", "neigh", "show", NULL});
        perror("execvp failed");
        exit(1);
        return true;
    }
    // myexe
    // XXX: implement myexe command when information about it provided
    else if(strcmp(cmd[0], "myexe") == 0) {
        printf("---\nMYENV INFO > myexe command not implemented yet.\n---\n");
        exit(1);
        return true;
    }
    return false;
}

void command_my(int PID, char* chem){
    char chemin[64];
    sprintf(chemin, "/proc/%d/%s", PID, chem);
    FILE* file = fopen(chemin, "r");
    if (file == NULL) {
         perror("fopen failed");
         return;
    }
    char line[256];
    while (fgets(line, sizeof(line), file) != NULL) {
        // Print the modified line
        printf("%s\n", line);
    }
    fclose(file);
}

void command_mydump(int PID, unsigned long address, unsigned long size, char *output_file){
    char chemin[64];
    sprintf(chemin, "/proc/%d/mem", PID);
    int file = open(chemin, O_RDONLY);
    if (file == -1) {
        perror("open failed");
        return;
    }
    if (lseek(file, address, SEEK_SET) == -1) {
        perror("lseek failed");
        close(file);
        return;
    }

    FILE *out = fopen(output_file, "wb");
    if (out == NULL) {
        perror("fopen output file failed");
        close(file);
        return;
    }

    unsigned char buffer[16];
    while (size > 0) {
        int chunk;
        if(size > 16) chunk = 16;
        else chunk = size;

        ssize_t n = read(file, buffer, chunk);
        if (n <= 0) {
            perror("read failed");
            close(file);
            fclose(out);
            return;
        }
        
        fprintf(out, "%016lx: ", address);
        for (int i = 0; i < n; i++) {
            fprintf(out, "%02x ", buffer[i]);
        }
        fprintf(out, "\n");
        address += n;
        size -= n;
    }
    close(file);
    fclose(out);
} 


void myinfo(void){
    char hostname[256];
    struct utsname uts;

    gethostname(hostname, sizeof(hostname));
    uname(&uts);

    printf("%s %s %s\n", hostname, uts.release, uts.machine);

    time_t now = time(NULL);
    struct tm *tm = localtime(&now);

    FILE *f_uptime = fopen("/proc/uptime", "r");
    if (!f_uptime) {
        perror("fopen /proc/uptime");
        return;
    }

    double uptime_seconds;
    fscanf(f_uptime, "%lf", &uptime_seconds);
    fclose(f_uptime);

    long up = (long)uptime_seconds;

    int days = up / 86400;
    int hours = (up % 86400) / 3600;
    int minutes = (up % 3600) / 60;

    printf("%02d:%02d:%02d up %d days, %02d:%02d\n",
           tm->tm_hour,
           tm->tm_min,
           tm->tm_sec,
           days,
           hours,
           minutes);

    FILE *f_load = fopen("/proc/loadavg", "r");
    if (!f_load) {
        perror("fopen /proc/loadavg");
        return;
    }

    double l1, l5, l15;
    fscanf(f_load, "%lf %lf %lf", &l1, &l5, &l15);
    fclose(f_load);

    printf("Load : %.2f - %.2f - %.2f\n", l1, l5, l15);

}


bool home_made_commands(char** cmd) {
    // myinfo implementation
    if(strcmp(cmd[0], "myinfo") == 0) {
        myinfo();
        return true;
    }
    // myenv implementation
    else if(strcmp(cmd[0], "myenv") == 0) {
        // if PID provided
        if(strcmp(cmd[1], "-p") == 0 && cmd[2] != NULL) {
            int pid = atoi(cmd[2]);
            command_my(pid,"environ");
        }
        // else current process
        // FIXME: Ask if it's possible, if not delete it
        else {
            command_my(getpid(),"environ");
        }
        return true;
    }
    // mymaps implementation
    else if(strcmp(cmd[0], "mymaps") == 0) {
        // if PID provided
        if(strcmp(cmd[1], "-p") == 0 && cmd[2] != NULL) {
            int pid = atoi(cmd[2]);
            command_my(pid,"maps");
        }
        // else current process
        // FIXME: Ask if it's possible, if not delete it
        else {
            command_my(getpid(),"maps");
        }
        return true;
    }
    // XXX: implement mydump command when information about it provided
    // mydump implementation
    // mydump -p 5842 --start 0x7ffc30f57000 --end 0x7ffc30f59000 -o dump.bin
    else if(strcmp(cmd[0], "mydump") == 0) {
        // if PID provided
        if(strcmp(cmd[1], "-p") == 0 && cmd[2] != NULL) {
            int pid = atoi(cmd[2]);
            // Check if --start and --end provided
            if(strcmp(cmd[3], "--start") == 0 && cmd[4] != NULL && strcmp(cmd[5], "--end") == 0 && cmd[6] != NULL) {
                unsigned long start = strtoul(cmd[4], NULL, 0);
                unsigned long end = strtoul(cmd[6], NULL, 0);
                unsigned long size = end - start;
                // Check if -o provided
                if(strcmp(cmd[7], "-o") == 0 && cmd[8] != NULL) {
                    command_mydump(pid,start,size, cmd[8]);
                } else {
                    command_mydump(pid,start,size, "mydump_output.bin");
                }
            } else {
            printf("---\nMYENV ERROR > On mydump: Invalid or missing --start/--end parameters\n---\n");
            }
        }
        return true;
    } 
    return false;
}

void commands(char** cmd) {
    if(cmd[0] == NULL) {
        fprintf(stderr, "No command provided\n");
        return;
    }
    // Try alias commands first
    if (!alias_commands(cmd)) {
        // Then try home made commands
        if(!home_made_commands(cmd)) {
            // Then do classical commands
            execvp(cmd[0], cmd);
            perror("execvp failed");
            exit(1);
        }
    }
}