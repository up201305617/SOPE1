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

//booleanos
#define FALSE 0
#define TRUE 1

struct info
{
	char name[100];
	char size[100];
	char permissions [100];
	char date [100];
	char path [100];
};
//para serem iguais têm que:
//- ser ficheiros regulares;
//- ter o mesmo nome;
//- ter as mesmas permissões;
//- ter o mesmo conteúdo
int are_equals(struct info i1, struct info i2)
{
	if(strcmp(i1.name,i2.name)==FALSE && strcmp(i1.permissions,i2.permissions)==FALSE)
	{
		if(are_same_content(i1.path,i2.path)==TRUE)
		{
			unlink(i2.path);
			link(i1.path, i2.path);
		}
	}
}

struct info fill_struct_info(char str[])
{
	struct info i1;
	char * name=strtok(str," ");
	char * size=strtok(NULL," ");
	char * permissions=strtok(NULL," ");
	char * date=strtok(NULL," ");
	char * path=strtok(NULL," ");
	strcpy(i1.name,name);
	strcpy(i1.size,size);
	strcpy(i1.permissions,permissions);
	strcpy(i1.date,date);
	strcpy(i1.path,path);
	return i1;
}

int are_same_content(char path1 [], char path2[])
{
	FILE *fp1;
	FILE *fp2;

	char ch1;
	char ch2;

	fp1 = fopen(path1, "r");
	fp2 = fopen(path2, "r");

	if (fp1 == NULL)
	{
		printf("Cannot open %s for reading ", path1);
		exit(1);
	}
	if (fp2 == NULL)
	{
	    printf("Cannot open %s for reading ", path2);
	    exit(1);
	}
    ch1 = fgetc(fp1);
    ch2 = fgetc(fp2);

	while ((ch1 != EOF) && (ch2 != EOF) && (ch1 == ch2))
	{
		ch1 = fgetc(fp1);
		ch2 = fgetc(fp2);
	}

    fclose(fp1);
  	fclose(fp2);

    if (ch1 == ch2)
    {
    	printf("Files are identical. \n");
    	return TRUE;
    }

    if (ch1 != ch2)
    {
    	printf("Files are Not identical. \n");
    	return FALSE;
    }
    return FALSE;
}

int main(int argc, char* argv[])
{
  int fd1; //ficheiro temporário com a lista de files desordenada
  int fd2; //ficheiro final com a lista de files ordenada
  int rem_status;
  char *filename = "/tmp/files_aux.txt";
  char *filename1 = "/tmp/files.txt";
  int saved_stdout = dup(STDOUT_FILENO);

  if (argc != 2)
  {
    fprintf(stderr, "Usage: %s <dir name>\n", argv[0]);
    exit(1);
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

  pid_t pid=fork();
  if (pid == -1)
  {
      printf ("Error on fork().\n");
  }
  if(pid==0)
  {
      execlp (path, "lstdir", str,path,argv[1], NULL);
	  perror("execlp");
  }
  if(pid>0)
  {
	  int status;
	  wait(&status);
	  printf("lstdir just finished.\n");
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
  dup2(saved_stdout, STDOUT_FILENO);
  sleep(5);
  if(pid1>0)
  {
	  wait(&stat);
	  printf ("sort just finished.\n");
  }

  close(fd1);
  close(fd2);

  //remove ficheiro temporário files_aux.txt
  rem_status=remove(filename);

  if (rem_status == 0)
  {
    printf ("File %s removed.\n",filename);
  }
  else
  {
    printf ("File %s not removed.\n",filename);
  }
  char tg [100] = "         file.txt 123 0600 12/122/12 e/y/yyyy";
  struct info i;
  i=fill_struct_info(tg);
  printf("%s\n",i.name);
  printf("%s\n",i.size);
  printf("%s\n",i.permissions);
  printf("%s\n",i.date);
  printf("%s\n",i.path);
  return 0;
}


