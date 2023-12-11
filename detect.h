/*  detect.h

    Author: Gen Giddings
    Student Number: 1080689
    Date: March 2022
    Course: COMP30023 - Computer Systems

*/


// TYPE DEFINITIONS
// I adapted this struct from the fileIO.pdf on the LMS
typedef struct {
    int process_id;
    int locked;
    int requested;
} Line;


// FUNCTION DECLARATIONS 
// schedules processes, avoiding deadlocks. Prints the execution order and simulation time
void schedule(Line lines[], int lines_len);

// comparison function for integers, passed to the c standard qsort()
int cmpfunc (const void *a, const void *b);

// counts and returns the number of distinct process-ids in an array
int count_distinct_ids(int id[], int id_len);

// returns the largest number of processes requesting the same file
int processes_requesting_same_file(Line lines[], int lines_len);

// display the output for task 1
void print_task_1(int processes[], int files[], int processes_len, int files_len);

// display the output for tasks 3, 4 and 5
void print_task_3_4_5(Line lines[], int lines_len, int deadlock, int lowest_deadlocked_ids[]);

/* returns 1 if a deadlock has been detected in lines, otherwise returns 0. Also fills 
   lowest_deadlocked_ids with the lowest process-id in each deadlock 
*/
int detect_deadlock(Line lines[], int lines_len, int lowest_deadlocked_ids[]);

// a recursive function which traverses from file to file, searching for deadlocks
int search(Line lines[], int lines_len, int visited[], int current_line);

// fill `array` of length `length` with `value` at each index
void initialise(int array[], int length, int value);

/* finds and returns the lowest critical process-id in the current deadlock. The processes involved in 
   the deadlock cycle are temporarily held in `visited`
*/
int find_lowest_id(Line lines[], int visited[], int lines_len);

// returns 1 if `file1` or `file2` are currently locked by a process, otherwise returns 0
int file_locked(Line lines[], int locked[], int length, int file1, int file2);

// returns 1 if all processes have completed, otherwise returns 0
int all_completed(int completed[], int length);

// copies all values of 1 or 2 from `source` into `destination`
void cpy(int source[], int destination[], int length);