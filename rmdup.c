#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>

int main1(int argc, char* argv[])
{
  DIR *dir;
  struct dirent *direntp;
  struct stat stat_buf;
  //int child_counter=0;
  //pid_t pids[255];
  int num_dir=0;
  //int status;
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

  chdir(argv[1]);

  while ((direntp = readdir(dir)) != NULL)
  {
     if (lstat(direntp->d_name, &stat_buf)==-1)
     {
        perror("lstat");
        exit(3);
     }
     if(S_ISDIR(stat_buf.st_mode) && strcmp(direntp->d_name,"..")!=0 && strcmp(direntp->d_name,".")!=0)
     {
        //pid_t pid=fork();
       // if(pid==0)
       // /{
          num_dir++;
          printf("%s\n",direntp->d_name);
        //}
        //child_counter++;
       // pids[child_counter]=pid;
     }

  }
  //int k;
  //for(k=0;k<child_counter;k++)
  //{  waitpid(pids[k],&status,0);}
  printf("Número dir= %d\n",num_dir);
  return 0;
}
