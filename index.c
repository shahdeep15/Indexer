// Deep Shah
// Shashank Seeram
// PA4: Index



#include "sorted-list.h"
#include "sorted-listmain.h"
#include "tokenizer.h"
#include <sys/stat.h>
#include <sys/types.h>
#include <linux/limits.h>
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>

// For the input file
int indexFile(MSortedListPtr mlist, FILE *fp, char *filepath) {
	Input *in;
	char *token;
	MNode *search;
	in = (Input *)malloc(sizeof(Input));
	if (in == NULL) { // Always good to check if there was enough memory 
		printf("Error: not enough memory.\n");
		return 0;
	}
	resetInput(in, fp);
	while((token = IGetNextToken(in)) != NULL) {
		search = MSLInsert(mlist, token);
		if(search == NULL) {
			printf("Search for token failed.\n");
			exit(EXIT_FAILURE);
		}
		if(SLInsert(search->sl, filepath) == 0) { //Error Checking
			printf("Error: file indexing failed.\n");
			free(in);
			return 0;
		}
		free(token);
	}
	free(in);
	return 1;
}
// For when the input is a directory, we need to go through it and find the files and/or subdirectories
int indexDir(MSortedListPtr mlist, DIR *dp, char *currpath) {
	struct dirent *entry;
	FILE *fp;
	while ((entry = readdir(dp)) != NULL) {
		char *path;
		path = (char *)malloc((strlen(currpath) + 1 + strlen(entry->d_name) + 1) * sizeof(char));
		int newfplength;
		strcpy(path, currpath);
		strcat(path, "/");
		strcat(path, entry->d_name);
		newfplength = strlen(path); 
		if (newfplength >= PATH_MAX) { //Error Checking
			printf("Error: path length is too long.\n");
			free(path);
			exit(EXIT_FAILURE);
		}
		struct stat statbuf;
		stat(path, &statbuf);
		if(S_ISDIR(statbuf.st_mode)) {
			if(strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
				printf("Dir not a proper dir. Skipping %s\n", path);
				continue;
			}
			DIR *dirptr;
			if ((dirptr = opendir(path)) != NULL) {
				indexDir(mlist, dirptr, path);
				closedir(dirptr);
			}
			else {
				printf("Error: directory %s could not be opened.\n", path);
				exit(EXIT_FAILURE);
			}
		}
		else if(S_ISREG(statbuf.st_mode)) {
			if(strstr(entry->d_name, ".txt") == NULL) { //Error Checking
				printf("File not a text file. Skipping file %s\n", path);
				continue;
			}
			if(strstr(entry->d_name, "~") != NULL) { //Error Checking
				printf("File is a temporary file. Skipping file %s\n", path);
				continue;
			}
			if ((fp = fopen(path, "r")) != NULL) {
				indexFile(mlist, fp, path);
				fclose(fp);
			}
			else { //Error Checking
				printf("Error: file %s could not be opened.\n", path);
				exit(EXIT_FAILURE);
			}
		}
	free(path);	
	}
	return 1;
}

// For the output file
void output(MSortedListPtr mlist, char *outputname) {
	FILE *fp;
	fp = fopen(outputname, "w");
	MNode *mainptr;
	Node *ptr;
	int counter = 0;
	mainptr = mlist->front;
	while(mainptr != NULL) {
		fprintf(fp, "<list> %s\n", mainptr->token);
		ptr = mainptr->sl->front;
		counter = 0;
		while(ptr != NULL) {
			counter++;
			if(counter >= 5) {
				fprintf(fp, "%s %d\n", ptr->filename, ptr->freq);
				counter = 0;
			}
			else {
				fprintf(fp, "%s %d ", ptr->filename, ptr->freq);
			}
			ptr = ptr->next;
		}
		if(counter != 0) {
			fprintf(fp, "\n");
		}
		fprintf(fp, "</list>\n");
		mainptr = mainptr->next;
	}
	fclose(fp);
}

int main(int argc, char **argv) {
	if(argc != 3){ //Error Checking
		printf("Error: invalid number of arguments\n");
		return 1;
	}
	struct stat statbuf;
	stat(argv[1], &statbuf);
	if(!S_ISREG(statbuf.st_mode)) {
		if(errno != ENOENT) { //Error Checking
			printf("Error: output file is not a text file.\n");
			return 1;
		}
	}
	if(strstr(argv[1], ".txt") == NULL) { //Error Checking
		printf("Error: output file not a text file.\n");
		return 1;
	}
	char *path;
	path = (char *)malloc((strlen(argv[2]) + 1) * sizeof(char));
	strcpy(path, argv[2]);
	if (strlen(path) >= PATH_MAX) { //Error Checking
		printf("Error: path length is too long.\n");
		free(path);
		exit(EXIT_FAILURE);
	}
	stat(argv[2], &statbuf);
	if(S_ISREG(statbuf.st_mode)) {
		if(strstr(argv[2], ".txt") == NULL) { //Error Checking
			printf("Error: Input file not a text file.\n");
			return 1;
		} else {
			if(strcmp(argv[1], argv[2]) == 0) { //IF the input and output files are the same, must ask if they want to continue with the process of indexing
				printf("Input and output file are the same.\nAre you sure you want to continue and overwrite the input file (y/n)(default: n)?");
				char i;
				scanf(" %c", &i);
				switch (i) {
					case 'y': // if the user wants to continue overriding the file
						break;
					case 'n': //If the user backs out and doesn't want to continue overriding the file 
						printf("Error: input file and output file are the same.\n");
						return 1;
						break;
					default:
						printf("Error: input file and output file are the same.\n"); //Error Checking
						return 1;
				}
			}
			FILE *fp;
			fp = fopen(argv[2], "r");
			MSortedListPtr mlist;
			mlist = MSLCreate();
			indexFile(mlist, fp, path);
			fclose(fp);
			output(mlist, argv[1]);
			MSLDestroy(mlist);
		}
	} else if(S_ISDIR(statbuf.st_mode)) {
		DIR *dir;
		dir = opendir(path);
		MSortedListPtr mlist;
		mlist = MSLCreate();
		indexDir(mlist, dir, path);
		closedir(dir);
		output(mlist, argv[1]);
		MSLDestroy(mlist);
	}
	free(path);
	return 0;
}
