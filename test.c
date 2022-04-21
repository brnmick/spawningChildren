#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <sys/wait.h>
#include <time.h>


void unix_error(char *msg){

	fprintf(stderr, "%s\n", msg, strerror(errno));
	exit(0);
}

pid_t Fork(void){

	pid_t pid;
	
	if ((pid = fork()) < 0)
		unix_error("Fork Error");
	return pid;
}

int main(){

	// test fork capability
	pid_t pid;
	pid = Fork(); // two pids exists in memory....

	pid_t whoam1, whoam2;

	if (pid == 0){ // i am the child
		whoam1 = getpid();
		printf("Child: my pid is: %d and my parent pid is: %d\n", whoam1, getppid());
	}
	else // i am the parent, and my childs pid is...
	{
		whoam2 = getpid();
		printf("Parent: my pid is: %d, and my child pid is: %d\n", whoam2, pid);
	}
	

	int count = 5; // max loop value
	if ( (pid == 0) ) // child process doing something
	{ 
		printf("Child: i will count to %d\n", count);
		for(int i=0; i <= count; ++i){ // child shall count up to said number
			printf("Child: ... %d\n", i);
            fflush(stdout);
			sleep(1);
		}
		printf("Child: I finished counting!");
		exit(1); // child is terminated here
	}	

	
//---------------------------------- child will not go passed here ------------------------------------------
	
	
	
	int status; // store the child's autopsy report
	time_t t; // useful for timestamps

 	
	while( 1 ) // check on child(ren)
	{
		pid = waitpid(pid, &status, WNOHANG); // parent shall wait for child(ren) to finish their respective processes


		if(pid == -1){ // error occurred while waiting
		
			switch(errno){
				case EINTR:
					printf("waitpid was interrupted\n");
					break;
				case ECHILD:
					printf("specified pid does not exists or is not a child or process group pid DNE\n");
					break;
				case EINVAL:
					printf("bad options\n");
					break;
				default:
					unix_error("catch missed");
			}break;// exit for loop
			
		} else if( (pid == 0) ){ // my child is doing something, continue to wait...
			time(&t);
			printf("\nParent: my child is running: %s\n", ctime(&t));
            fflush(stdout);
			sleep(3);

		} else{ //  child has been accounted for (reaped)
			//printf("Parent: my child pid: %d, finished at: %s\n", pid, ctime(&t));
			break; // pid > 0 ....
		}
		
	}
	

	// what was the child's fate?
	if (WIFEXITED(status))
		printf("\nParent: child %d terminated normally with exit status = %d\n", pid, WEXITSTATUS(status));
	else
		printf("Parent: child %d terminated abnormally\n", pid);
	
	

	
	
	return 0;
}