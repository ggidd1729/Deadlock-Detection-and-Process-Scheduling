# deadlock-detection-and-process-scheduling

I wrote this program as an assignment for my final year undergraduate subject on Computer Systems.

The program takes input through the command line. Each line should contain 3 integers, a process ID, followed by the file ID that that process has locked, followed by the file 
ID that that process is waiting on. The program will then detect whether a deadlock has occurred and if so which processes to terminate in order to resolve the deadlock.   
Optional flags `e` and `c` described below offer alternative functions. 

To clean the directory run `make clean`.  
To compile the code `make -B`.  
To execute the code run `./detect <command line arguments>`  
  
Valid input flags from the command line:  
  -f followed by the path to a file describing process resource requests  
  -e when provided, absense of deadlock is assumed and execution time for all processes is calculated.  
  -c when provided, outputs a schedule for the processes which is faster than a naive sequential scheduling algorithm and the time it takes for processes to release all files.   This is explained further in report.txt.
