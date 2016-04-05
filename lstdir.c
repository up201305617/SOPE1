#include <stdio.h>
#include <dirent.h>
#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>
#include <stdlib.h>

int main(int argc, char* argv[])
{
	int fd1;
	int owner=0;
	int group=0;
	int other=0;
	struct dirent *direntp;
	struct stat stat_buf;
	DIR *dir;
	char *filename = "files_aux.txt";

	if(argc!=2)
	{
		fprintf(stderr, "Usage: %s <dir name>\n", argv[0]);
		exit(1);
	}

	if ((dir = opendir(argv[1])) == NULL)
	{
	    perror(argv[1]);
	    exit(2);
	}

	if ((fd1 = open(filename, O_WRONLY|O_CREAT|O_EXCL|O_SYNC, 0600)) == -1)
	{
	    perror(filename);
	    exit(3);
	}

	dup2(fd1, STDOUT_FILENO);

	chdir(argv[1]);

	while ((direntp = readdir(dir)) != NULL)
	{
		owner=0;
		group=0;
		other=0;
		if (lstat(direntp->d_name, &stat_buf)==-1)
	    {
	       perror("lstat");
	       exit(3);
	    }
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
	    	printf("%s %lld 0%d%d%d %s\n",direntp->d_name,(long long) stat_buf.st_size,owner,group,other,argv[1]);
	    }
	}

	return 0;
}
/*
 printf( (fileStat.st_mode & S_IRUSR) ? "r" : "-");
    printf( (fileStat.st_mode & S_IWUSR) ? "w" : "-");
    printf( (fileStat.st_mode & S_IXUSR) ? "x" : "-");

    printf( (fileStat.st_mode & S_IRGRP) ? "r" : "-");
    printf( (fileStat.st_mode & S_IRGRP) ? "w" : "-");
    printf( (fileStat.st_mode & S_IXGRP) ? "x" : "-");

    printf( (fileStat.st_mode & S_IROTH) ? "r" : "-");
    printf( (fileStat.st_mode & S_IWOTH) ? "w" : "-");
    printf( (fileStat.st_mode & S_IXOTH) ? "x" : "-");
 */
