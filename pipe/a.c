#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(){

	int pid,pid_fd[2],status;
	 
	status = pipe(pid_fd);
	 
	if (status) {
        perror("pipe");
        return status;
    }

	pid = fork();
    if (pid < 0)
        perror("fork");
	else if(pid == 0){
		char text[64];
		printf("Type something!\n");
		read(0,text,sizeof(text));
		close(pid_fd[0]);
		write(pid_fd[1],text,sizeof(text));
		close(pid_fd[1]);
	}
	else{
		char res[64];
		close(pid_fd[1]);
		read(pid_fd[0],res,sizeof(res));
		close(pid_fd[0]);
		printf("Got string : %s\n",res);
	}
}