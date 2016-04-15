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


struct info fill_struct_info(char str[])
{
	struct info i1;
	char * name=strtok(str," ");
	char * size=strtok(NULL," ");
	char * permissions=strtok(NULL," ");
	char * date=strtok(NULL," ");
	char * path=strtok(NULL,"\n");//<- mudei de " " para "\n"
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
			
			return TRUE;
		}
		return FALSE;
	}
	return FALSE;
}
//não utilizada
int info_equals(struct info i1, struct info i2){
	if(i1.name == i2.name){
		if(i1.size == i2.size){
			if (i1.permissions == i2.permissions){
				if (i1.date == i2.date){
					if (i1.path == i2.path){
						return TRUE;
					}
				}
			}
		}
	}
	return FALSE;
}
//não utilizada
int delete_duplicates(struct info buf[], int len){
	int i = 0, j = 0, k = 0;
	
	for (i = 0; i < len; i++) {//elimina valores duplicados
		for (j = i + 1; j < len;) {
			if (info_equals(buf[j], buf[i]) == TRUE) {
				for (k = j; k < len; k++) {
					buf[k] = buf[k + 1];
				}
				len--;
			} else
				j++;
		}
	}
	
	for (i = 0; i < len; i++){
		printf("%s\n", buf[i].path);
	}
	
	for (i = 0; i < len; i++){
		unlink(buf[i].path);
		//link()
	}
	
	return 0;
}

struct info * read_file(char path[], int *file_nr){
	/* //read version - not working
	char *filename1 = "/tmp/files.txt";
	//struct info  *files  = malloc(sizeof(info));
	int fd;//ficheiro final com a lista de files ordenada
	char * ch = NULL;
	char line[500];
	int line_ctr = 0;
	int file_ctr = 0;
	
	if((fd=open(filename1, O_RDONLY, 0600)) == -1)
	{
		perror(filename1);
		exit(5);
	}
	
	while(read(fd, ch, sizeof(ch)) != 0){
		printf("...\n");
		printf("%i\n", *ch);
		printf("...\n");
		if (*ch == '\n'){
			//files[file_ctr] = fill_struct_info(line);
			struct info i1 = fill_struct_info(line);
			
			int i = 0;
			for (; i < line_ctr; i++){//"limpa" conteudo da string
				if (line[i] != ' '){
					line[i] = ' ';
				}
			}
			
			//struct info i1 = files[file_ctr];
			
			line_ctr = 0;
			file_ctr++;
			
			printf("%s\n", i1.name);
			printf("%s\n", i1.size);
			printf("%s\n", i1.permissions);
			printf("%s\n", i1.date);
			printf("%s\n", i1.path);
		}
		else {
			line[line_ctr++] = *ch;
		}
	}
	
	//return files;*/
	
	FILE * fp;
	char * line = NULL;
	size_t len = 0;
	ssize_t read;
	
	struct info files[512];
	int file_ctr = 0;
	
	if ((fp = fopen(path, "r")) == NULL){
		perror(path);
		exit(5);
	}
	
	while((read = getline(&line, &len, fp)) != -1){
		files[file_ctr] = fill_struct_info(line);
		file_ctr++;
	}
	
	fclose(fp);
	
	struct info * infptr;
	infptr = files;
	
	//printf("%s---\n", files[0].path);

	*file_nr = file_ctr;
	return infptr;
	
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
	
	unlink(filename1);

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
	//sleep(5);
	
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
	
	
	int delete[512], deleted = FALSE;
	int del_ctr = 0;
	int file_nr = 0;
	
	struct info * infptr = malloc(sizeof(struct info));
	infptr = read_file(filename1, &file_nr);	
	
	struct info i1;
	struct info i2;
	int i = 0, j = 0, k = 0;
	
	for (i = 0; i < file_nr; i++){
		i1 = *infptr;
		
		for (j = i+1; j < file_nr; j++){
			
			infptr++;
			i2 = *infptr;
			
			//printf("i1:%s - i2:%s\n", i1.name, i2.name);
			
			for (k = 0; k < del_ctr; k++){
				if (delete[k] == j || delete[k] == i){
					deleted = TRUE;
					break;
				}
				deleted = FALSE;
			}
			
			if (deleted == FALSE){
				if (are_equals(i1, i2) == TRUE){
					printf("Deleted\n");
					delete[del_ctr] = j;
					del_ctr++;
				}
			}
		}
		//printf(".............................................................\n");	
		
		for (k = 0; k < (file_nr-(i+2)); k++){//reposiciona apontador em i
			infptr--;
		}
	}
	
	for (k = 0; k < (file_nr-1); k++){//reposiciona apontador no inicio
		infptr--;
	}
	
	int hlinks;//ficheiro hlinks.txt
	char hlinks_path_name[128];
	char hlinks_path[128];
	strcpy(hlinks_path, "/hlinks.txt");
	strcpy(hlinks_path_name, argv[1]);
	strcat(hlinks_path_name, hlinks_path);
	
	printf("%s\n", hlinks_path_name);
	
	unlink(hlinks_path_name);
	
	if((hlinks=open(hlinks_path_name, O_WRONLY|O_CREAT|O_APPEND, 0600)) == -1)
	{
		perror(filename1);
		exit(4);
	}
	
	dup2(hlinks, STDOUT_FILENO);
	
	for (i = 0; i < file_nr; i++){
		for (j = 0; j < del_ctr; j++){
			if (delete[j] == i){
				i1 = *infptr;
				
				printf("%25s %10s %4s %8s %s\n",(*infptr).name, (*infptr).size, (*infptr).permissions, (*infptr).date, (*infptr).path);
			}
		}
		infptr++;		
	}
	
	close(hlinks);
	
	return 0;
}