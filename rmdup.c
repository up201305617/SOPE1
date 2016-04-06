#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <time.h>
#include <limits.h>

int main(int argc, char* argv[])
{
  DIR *dir;
  int fd1; //ficheiro temporário com a lista de files desordenada
  int fd2; //ficheiro final com a lista de files ordenada
  struct dirent *direntp;
  struct stat stat_buf_1;
  int child_counter=0;
  pid_t pids[128];
  int rem_status;
  char *filename = "/tmp/files_aux.txt";
  char *filename1 = "/tmp/files.txt";
  int saved_stdout = dup(STDOUT_FILENO);

  if (argc != 2)
  {
    fprintf(stderr, "Usage: %s <dir name>\n", argv[0]);
    exit(1);
  }

  if ((dir = opendir(argv[1])) == NULL)
  {
    perror(argv[1]);
    exit(2);
  }

  if ((fd1 = open(filename, O_WRONLY|O_CREAT|O_APPEND|O_SYNC, 0600)) == -1)
  {
	perror(filename);
	exit(3);
  }

  if((fd2=open(filename1,O_WRONLY|O_CREAT|O_APPEND|O_SYNC, 0600)) == -1)
  {
	  perror(filename1);
	  exit(4);
  }

  char str[15];
  sprintf(str, "%d", fd1);

  char cwd[PATH_MAX + 1];
  if (getcwd(cwd, PATH_MAX + 1) == NULL)
  {
	  perror("getcwd");
	  exit(2);
  }

  char path[PATH_MAX + 1];
  snprintf(path, PATH_MAX + 1, "%s/lstdir", cwd);

  if (chdir(argv[1]) != 0)
  {
      perror("chdir");
      exit(3);
  }

  while ((direntp = readdir(dir)) != NULL)
  {
	 if (lstat(direntp->d_name, &stat_buf_1)==-1)
     {
        perror("lstat1");
        exit(4);
     }
     //verifica se é um directório e se não é o directório corrente(.) ou o pai do directório corrente(..)
     if(S_ISDIR(stat_buf_1.st_mode) && strcmp(direntp->d_name,".")!=0 && strcmp(direntp->d_name,"..")!=0)
     {
    	pid_t pid=fork();

    	if (pid == -1)
    	{
    		 printf ("Error on fork().\n");
    	}

    	if(pid==0)
        {
        	chdir(direntp->d_name);
        	execl(path,"lstdir",str,NULL);
        	exit(5);
        }
        child_counter++;
        pids[child_counter]=pid;
     }

  }

  int k;

  for(k=0;k<child_counter;k++)
  {
	  waitpid(pids[k],NULL,0);
  }
  dup2(fd2, STDOUT_FILENO);

  pid_t pid1 = fork();
  int stat;

  if (pid1 == -1)
  {
       printf ("Error on fork().\n");
  }
  if(pid1==0)
  {
	  execlp ("sort", "sort", filename, NULL);
	  perror("execlp");
  }
  if(pid1>0)
  {
	  pid_t pid0 = wait(&stat);
	  printf ("My son (%d) just finished.\n", pid0);
  }

  close(fd1);

  dup2(saved_stdout, STDOUT_FILENO);

  close(fd2);
  closedir(dir);

  rem_status=remove(filename);

  if (rem_status == 0)
      printf ("\nFile %s removed.\n",filename);
  else
      printf ("\nFile %s not removed.\n",filename);

  return 0;
}
