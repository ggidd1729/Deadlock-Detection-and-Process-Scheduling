#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <unistd.h>
#include "detect.h"

int main(int argc, char *argv[]) { 
    char filepath[PATH_MAX];
    char mode = 'd'; 
    int challenge = 0;
    
    // read flags
    int opt;
    while((opt = getopt(argc, argv, "f:ec")) != -1){
        switch(opt){
            case 'f':
                realpath(optarg, filepath);
                break;
            case 'e':
                mode = 'e';
                break;
            case 'c':
                challenge = 1;
                break;
        }
    }

    /* Open file
       I adapted this code block from the fileIO.pdf on the LMS */
    FILE* fp = fopen(filepath, "r");
    if (fp == NULL) {
        perror("fopen");
        exit(EXIT_FAILURE);
    }

    /* Read into memory
       I adapted this code block from the fileIO.pdf on the LMS */
    size_t lines_len = 0, lines_size = 2;
    Line* lines = malloc(sizeof(Line) * lines_size);
    if (lines == NULL) {
        fprintf(stderr, "Malloc failure\n");
        exit(EXIT_FAILURE);
    }
    int process_id, locked, requested;
    while (fscanf(fp, "%d %d %d", &process_id, &locked, &requested) == 3) {
        if (lines_len == lines_size) {
            lines_size *= 2;
            lines = realloc(lines, sizeof(Line) * lines_size);
            if (lines == NULL) {
                fprintf(stderr, "Realloc failure\n");
                exit(EXIT_FAILURE);
            }
        }
        lines[lines_len].process_id = process_id;
        lines[lines_len].locked = locked;
        lines[lines_len++].requested = requested;
    }

    // section 4: challenge
    if (challenge){
        schedule(lines, lines_len);
        return 0;
    }

    /* create an array for holding process-id's and an array for holding file-id's
       I adapted this code block from the fileIO.pdf on the LMS */
    int processes_len = lines_len, files_len = 2*lines_len;
    int processes[processes_len], files[files_len];
    for (int i = 0; i < processes_len; i++) {
        Line line = lines[i];
        processes[i] = line.process_id;
        files[2*i] = line.locked;
        files[(2*i)+1] = line.requested;
    }

    // sort the arrays
    qsort(processes, processes_len, sizeof(int), cmpfunc);
    qsort(files, files_len, sizeof(int), cmpfunc);

    // task 2 (also contains task 1)
    if (mode == 'e'){
        int execution_time;
        if (!lines_len){
            execution_time = 0;
        } else {
            execution_time = processes_requesting_same_file(lines, lines_len) + 1;
        }
        print_task_1(processes, files, processes_len, files_len);
        printf("Execution time %d\n", execution_time);
    }

    // tasks 3, 4 and 5 (also contains task 1)
    else {
        int lowest_deadlocked_ids[processes_len];
        initialise(lowest_deadlocked_ids, processes_len, -1); 
        int deadlock = detect_deadlock(lines, lines_len, lowest_deadlocked_ids);
        qsort(lowest_deadlocked_ids, processes_len, sizeof(int), cmpfunc);
        print_task_1(processes, files, processes_len, files_len);
        print_task_3_4_5(lines, lines_len, deadlock, lowest_deadlocked_ids);
    }

    free(lines);
    fclose(fp);
    return 0; 
}

void schedule(Line lines[], int lines_len) {
    int completed[lines_len];
    initialise(completed, lines_len, 0);

    if (!lines_len){
        printf("Simulation time %d\n", lines_len);
        return;
    }

    // i is a time unit, with lines_len being the maximum simulation time (for a naive scheduler)
    for (int i = 0; i < lines_len; i++){
        int locked[lines_len];
        initialise(locked, lines_len, 0);

        for (int j = 0; j < lines_len; j++){
            if (completed[j]){
                continue;
            } else if (file_locked(lines, locked, lines_len, lines[j].locked, lines[j].requested)){
                continue;
            } else {
                locked[j] = 1;
                completed[j] = 1;
                printf("%d %d %d,%d\n", i, lines[j].process_id, lines[j].locked, lines[j].requested);
            }
        }

        if (all_completed(completed, lines_len)){
            printf("Simulation time %d\n", i + 1);
            break;
        }
    }
}

int cmpfunc (const void *a, const void *b) {
   return (*(int*)a - *(int*)b);
}

int count_distinct_ids(int id[], int id_len) {
    if (id_len == 0) {
        return id_len;
    }

    int count = 1;
    for (int i = 0; i < id_len - 1; i++){
        if (id[i] != id[i+1]){
            count++;
        }
    }

    return count;
}

int processes_requesting_same_file(Line lines[], int lines_len) {
    int max_clash = 1;

    for (int i = 0; i < lines_len; i++){
        int clash_count = 1;
        for (int j = i + 1; j < lines_len; j++){
            if (lines[i].requested == lines[j].requested){
                clash_count++;
            }
        }

        if (clash_count > max_clash) {
            max_clash = clash_count;
        }
    }

    return max_clash;
}

void print_task_1(int processes[], int files[], int processes_len, int files_len) {
    printf("Processes %d\n", count_distinct_ids(processes, processes_len));
    printf("Files %d\n", count_distinct_ids(files, files_len));
}

void print_task_3_4_5(Line lines[], int lines_len, int deadlock, int lowest_deadlocked_ids[]) {
    if (deadlock) {
        printf("Deadlock detected\n");
        printf("Terminate ");
        for (int i = 0; i < lines_len; i++){
            if (lowest_deadlocked_ids[i] != -1){
                printf("%d ", lowest_deadlocked_ids[i]);
            }
        }
        printf("\n");
    } else {
        printf("No deadlocks\n");
    }
}

int detect_deadlock(Line lines[], int lines_len, int lowest_deadlocked_ids[]) {
    int deadlock_detected = 0;
    int assessed[lines_len];
    initialise(assessed, lines_len, 0);

    for (int i = 0; i < lines_len; i++){
        if (assessed[i]){
            continue;
        }

        int visited[lines_len];
        initialise(visited, lines_len, 0);
        visited[i] = 2;

        if (search(lines, lines_len, visited, i)) {
            deadlock_detected = 1;
            cpy(visited, assessed, lines_len);
            int lowest_id = find_lowest_id(lines, visited, lines_len);
            lowest_deadlocked_ids[lowest_id] = lines[lowest_id].process_id;
        }

    }

    return deadlock_detected;
}

int search(Line lines[], int lines_len, int visited[], int current_line) {
    for (int j = 0; j < lines_len; j++){
        if (lines[current_line].requested == lines[j].locked){
            if (visited[j] == 2){
                return 1;
            } else if (visited[j] == 1){
                return 0;
            }
            visited[j] = 1;
            return search(lines, lines_len, visited, j);
        }
    }
    
    return 0;
}

void initialise(int array[], int length, int value) {
    for (int i = 0; i < length; i++){
        array[i] = value;
    }
}

int find_lowest_id(Line lines[], int visited[], int lines_len) {
    unsigned int lowest_id = UINT_MAX;

    for (int i = 0; i < lines_len; i++){
        if (visited[i] && lines[i].process_id < lowest_id){
            lowest_id = lines[i].process_id;
        }
    }   

    for (int i = 0; i < lines_len; i++){
        if (lines[i].process_id == lowest_id){
            return i;
        }
    }

    return lowest_id;
}

int file_locked(Line lines[], int locked[], int length, int file1, int file2) {
    for (int i = 0; i < length; i++){
        if (locked[i]){
            if (lines[i].locked == file1 || lines[i].locked == file2 
                || lines[i].requested == file1 || lines[i].requested == file2){
                    return 1;
                }
        }
    }

    return 0;
}

int all_completed(int completed[], int length) {
    for (int i = 0; i < length; i++){
        if (completed[i] == 0){
            return 0;
        }
    }

    return 1;
}

void cpy(int source[], int destination[], int length) {
    for (int i = 0; i < length; i++){
        if (source[i] == 1){
            destination[i] = 1;
        } else if (source[i] == 2){
            destination[i] = 2;
        } 
    }
}