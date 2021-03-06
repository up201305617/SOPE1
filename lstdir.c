#include <stdio.h>
#include <dirent.h>
#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>
#include <stdlib.h>
#include <limits.h>
#include <sys/wait.h>
#include <string.h>

int main(int argc, char* argv[])
{
	int fd1;
	int owner=0;
	int group=0;
	int other=0;
	struct dirent *direntp;
	struct stat stat_buf;
	DIR *dir;
	char str[20];
	strcpy(str, argv[1]);
	fd1 = atoi(str);

	if(argc!=4)
	{
		fprintf(stderr, "Usage: %s <file descriptor> <path> <dir>\n", argv[0]);
		exit(1);
	}

	if ((dir = opendir(argv[3])) == NULL)
	{
	    perror(argv[3]);
	    exit(2);
	}

	dup2(fd1, STDOUT_FILENO);

	while ((direntp = readdir(dir)) != NULL)
	{
		owner=0;
		group=0;
		other=0;
		char new_folder [PATH_MAX];
		if(strcmp(direntp->d_name,".")==0 || strcmp(direntp->d_name,"..")==0)
		{
			//ignora
		}
		else
		{
            sprintf(new_folder, "%s/%s", argv[3], direntp->d_name);
			if (lstat(new_folder, &stat_buf)==-1)
			{
			   perror("lstat1");
			   exit(3);
			}
			//permissions
			if(S_ISREG(stat_buf.st_mode))
			{
				//ownwer
				if(stat_buf.st_mode & S_IRUSR)
				{
					owner+=4;
				}
				if(stat_buf.st_mode & S_IWUSR)
				{
					owner+=2;
				}
				if(stat_buf.st_mode & S_IXUSR)
				{
					owner+=1;
				}
				//group
				if(stat_buf.st_mode & S_IRGRP)
				{
					group+=4;
				}
				if(stat_buf.st_mode & S_IWGRP)
				{
					group+=2;
				}
				if(stat_buf.st_mode & S_IXGRP)
				{
					group+=1;
				}
				//others
				if(stat_buf.st_mode & S_IROTH)
				{
					other+=4;
				}
				if(stat_buf.st_mode & S_IWOTH)
				{
					other+=2;
				}
				if(stat_buf.st_mode & S_IXOTH)
				{
					other+=1;
				}
				//last modification date
				struct tm * aux = localtime(&stat_buf.st_mtim.tv_sec);
				int day=aux->tm_mday;
				int month=aux->tm_mon+1;
				int year=aux->tm_year+1900;
				if(day<10)
				{
					printf("%25s %10lld 0%d%d%d %d/%d/0%d %s\n",direntp->d_name,(long long) stat_buf.st_size,owner,group,other,year,month,day,new_folder);
				}
				else
				{
					printf("%25s %10lld 0%d%d%d %d/%d/%d %s\n",direntp->d_name,(long long) stat_buf.st_size,owner,group,other,year,month,day,new_folder);
				}
			}
			if(S_ISDIR(stat_buf.st_mode))
			{
				pid_t pid = fork();
				int status;
				if(pid == 0)
				{
					execlp(argv[2], "lsdir", str,argv[2],new_folder, NULL);
				}
				else
				{
					wait(&status);
				}
			}
		}

	}
	closedir(dir);
	return 0;
}
