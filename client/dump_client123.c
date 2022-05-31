#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/stat.h>
#include <dirent.h>
#include <ctype.h>
#include <time.h>
#define PORT 4443

int AuthentikasiCheck();

struct allowed{
	char name[10000];
	char password[10000];
};

int AuthentikasiCheck(char *username, char *password){
	FILE *fp;
	struct allowed user;
	int usercheck = 0;

	fp=fopen("../database/databases/user.dat","rb");
    while(1){	
		fread(&user,sizeof(user),1,fp);
		if(strcmp(user.name, username)==0){
			if(strcmp(user.password, password)==0){
				usercheck = 1;
			}
		}
		if(feof(fp)){
			break;
		}
	}
	fclose(fp);
	if(usercheck == 0){
		printf("Authentikasi Ditolak\n");
		return 0;
	}else{
        printf("Authentikasi Diterima, selamat datang %s\n",username);
		return 1;
	}
}

int main(int argc, char *argv[]){
	int usercheck=0;
	int id_user = geteuid();
	char database_used[1000];
	if(geteuid() == 0){
		usercheck=1;
        printf("Authentikasi Diterima");
	}
    else{
		int id = geteuid();
		usercheck = AuthentikasiCheck(argv[2],argv[4]);
	}

	if(usercheck==0){
		return 0;
	}

    FILE *fp;
    char path[10000];
	snprintf(path, sizeof path, "../database/log/log%s.log", argv[2]);
    fp = fopen(path, "rb");

    char buffer[20000];
    char logdata[100][10000];
    int Check2=0;

    while (fgets(buffer, 20000 , fp)){
        char *token;
        char buffercopy[20000];
        snprintf(buffercopy, sizeof buffercopy, "%s", buffer);
        token = strtok(buffercopy, ":");
        int i = 0;
        
        while ( token != NULL){
            strcpy(logdata[i], token);
			i++;
			token = strtok(NULL, ":");
        }

	    for(i = 0; i < strlen(logdata[4]); i++){
            if (logdata[4][i] == '\n')
				logdata[4][i+1] = '\0';
    	}
        printf("%s", logdata[4]);
        
        char *b = strstr(logdata[4], "USE");
        
        if(b != NULL){
            char *tokens;
            char perintahCopy[20000];
            strcpy(perintahCopy, logdata[4]);
            tokens = strtok(perintahCopy, "; ");
            int j=0;
            char perintahUse[100][10000];
            while ( tokens != NULL){
                strcpy(perintahUse[j], tokens);
                // printf("%s\n", perintahUse[j]);
                j++;
                tokens = strtok(NULL, "; ");
            }
            char databaseTarget[20000];
            // sprintf(databaseTarget, "%s;", argv[5]);
            // printf("%s\n", perintahUse[1]);
            // printf("%s\n", databaseTarget);
            if(strcmp(perintahUse[1], argv[5])==0){
                Check2 = 1;
            }else{
                Check2 = 0;
            }
        }
        // printf("%d\n", found);
        if(Check2 == 1){
        //    printf("%s", buffer);
        }
        // printf("%s", buffer);
    }
    fclose(fp);
}
