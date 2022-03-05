#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <unistd.h>

int main ()
{
	// Spawn a child process
	pid_t chld = fork();
	if (chld < 0) {
		perror("fork");
		return 1;
	} else if (chld) {
		// parent process
		printf("Parent: defenselessly sleeping...\n");
		if (wait(NULL) < 0) {// wait for the child
			perror("wait");
		} else {
			// The child process exited without killing the parent
			fprintf(stderr, "I am invincible! (not really...)\n");
		}
		return 1; // the parent should be killed, this should not be executed
	} else {
		// child process; commit parricide
		printf("Child: murdering parent!\n");
		if (kill(getppid(), SIGKILL) < 0) {
			perror ("kill"); // an error occurred
			return 1;
		}
	}
	return 0;
}
