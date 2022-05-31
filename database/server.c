#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <unistd.h>
#include <time.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define PORT 4443

int compare1(char a1[50],char a2[50]){
	if (strcmp(a1,a2)==0)
		return 1;
	return 0;
}
int compare2(char a1[50],char a2[50], char b1[50], char b2[50]){
	if (strcmp(a1,a2)==0)
		if (strcmp(b1,b2)==0)
		return 1;
	return 0;
}

struct allowed
{
	char name[10000];
	char password[10000];
};

struct allowed_database
{
	char database[10000];
	char name[10000];
};

struct table
{
	int jumlahkolom;
	char type[100][10000];
	char data[100][10000];
};

void createUser(char *name, char *pass)
{
	struct allowed user;
	strcpy(user.name, name);
	strcpy(user.password, pass);

	FILE *fp;
	fp = fopen("databases/user.dat", "ab");
	fwrite(&user, sizeof(user), 1, fp);
	fclose(fp);
}

int cekUserExist(char *username)
{
	struct allowed user;
	int id, found = 0;

	FILE *fp;
	fp = fopen("../database/databases/user.dat", "rb");
	while (1)
	{
		fread(&user, sizeof(user), 1, fp);
		if (strcmp(user.name, username) == 0)
		{
			fclose(fp);
			return 1;
		}
		if (feof(fp))
		{
			fclose(fp);
			return 0;
		}
	}
}

void insertPermission(char *name, char *database)
{
	struct allowed_database user;
	strcpy(user.name, name);
	strcpy(user.database, database);

	FILE *fp;
	fp = fopen("databases/permission.dat", "ab");
	fwrite(&user, sizeof(user), 1, fp);
	fclose(fp);
}

int cekAllowedDatabase(char *name, char *database)
{
	struct allowed_database user;

	FILE *fp;
	fp = fopen("../database/databases/permission.dat", "rb");
	while (1)
	{
		fread(&user, sizeof(user), 1, fp);
		if (strcmp(user.name, name) == 0)
		{
			if (strcmp(user.database, database) == 0)
			{
				fclose(fp);
				return 1;
			}
		}
		if (feof(fp))
		{
			fclose(fp);
			return 0;
		}
	}
}

int findColumn(char *table, char *kolom)
{
	struct table user;

	FILE *fp;
	fp = fopen(table, "rb");
	fread(&user, sizeof(user), 1, fp);

	for (int i = 0; i < user.jumlahkolom; i++)
	{
		if (strcmp(user.data[i], kolom) == 0)
		{
			return i; // index column
			fclose(fp);
		}
	}

	fclose(fp);
	return -1;
}

// ??
int deleteColumn(char *table, int index)
{
	struct table user;

	FILE *fp, *fp1;
	fp = fopen(table, "rb");
	fp1 = fopen("temp", "wb");

	fread(&user, sizeof(user), 1, fp);
	if (feof(fp))
	{
		fclose(fp);
		fclose(fp1);

		// replace table awal dgn table temp yang baru
		remove(table);
		rename("temp", table);
		return 0;
	}

	struct table userCopy;
	int indeksCopy = 0;
	int j = 0;
	while (j < user.jumlahkolom)
	{
		// skip column misal index remove
		if (j == index)
		{
			j++;
		}
		// duplikat data column table
		strcpy(userCopy.data[indeksCopy], user.data[j]);
		strcpy(userCopy.type[indeksCopy], user.type[j]);
		indeksCopy++;
		j++
	}
	userCopy.jumlahkolom = user.jumlahkolom - 1;
	fwrite(&userCopy, sizeof(userCopy), 1, fp1);

	fclose(fp);
	fclose(fp1);

	// replace table awal dgn table temp yang baru
	remove(table);
	rename("temp", table);
	return 0;
}

int deleteTable(char *table)
{
	struct table user;

	FILE *fp, *fp1;
	fp = fopen(table, "rb");
	fp1 = fopen("temp", "ab");

	fread(&user, sizeof(user), 1, fp);
	struct table userCopy;

	int j = 0;
	while (j < user.jumlahkolom)
	{
		strcpy(userCopy.data[j], user.data[j]);
		strcpy(userCopy.type[j], user.type[j]);
		j++;
	}

	userCopy.jumlahkolom = user.jumlahkolom;
	fwrite(&userCopy, sizeof(userCopy), 1, fp1);
	fclose(fp);
	fclose(fp1);

	remove(table);
	rename("temp", table);
	return 1;
}

int updateColumn(char *table, int index, char *ganti)
{
	struct table user;

	FILE *fp, *fp1;
	fp = fopen(table, "rb");
	fp1 = fopen("temp", "ab");

	int inpo = 0; // >0 brarti msk ke inpo data dr column tsb
	while (1)
	{
		fread(&user, sizeof(user), 1, fp);
		if (feof(fp))
		{
			break;
		}
		struct table userCopy;
		int iterasi = 0;

		int j = 0;
		while (j < user.jumlahkolom)
		{
			if (j == index && inpo != 0)
			{
				strcpy(userCopy.data[iterasi], ganti);
			}
			else
			{
				strcpy(userCopy.data[iterasi], user.data[j]);
			}
			strcpy(userCopy.type[iterasi], user.type[j]);
			iterasi++;
			j++;
		}

		userCopy.jumlahkolom = user.jumlahkolom;
		fwrite(&userCopy, sizeof(userCopy), 1, fp1);
		inpo++;
	}
	fclose(fp);
	fclose(fp1);
	remove(table);
	rename("temp", table);
	return 0;
}

int updateColumnWhere(char *table, int index, char *ganti, int indexGanti, char *where)
{
	struct table user;

	FILE *fp, *fp1;
	fp = fopen(table, "rb");
	fp1 = fopen("temp", "ab");
	int inpo = 0; // Column atau datanya
	while (1)
	{
		fread(&user, sizeof(user), 1, fp);
		if (feof(fp))
		{
			break;
		}
		struct table userCopy;
		int iterasi = 0;
		int j = 0;
		while (j < user.jumlahkolom)
		{
			if (j == index && inpo != 0 && strcmp(user.data[indexGanti], where) == 0)
			{
				strcpy(userCopy.data[iterasi], ganti);
			}
			else
			{
				strcpy(userCopy.data[iterasi], user.data[j]);
			}
			strcpy(userCopy.type[iterasi], user.type[j]);
			iterasi++;
			j++;
		}

		userCopy.jumlahkolom = user.jumlahkolom;
		fwrite(&userCopy, sizeof(userCopy), 1, fp1);
		inpo++;
	}
	fclose(fp);
	fclose(fp1);
	remove(table);
	rename("temp", table);
	return 0;
}

int deleteTableWhere(char *table, int index, char *kolom, char *where)
{
	FILE *fp, *fp1;
	struct table user;
	int id, found = 0;
	fp = fopen(table, "rb");
	fp1 = fopen("temp", "ab");
	int datake = 0;
	while (1)
	{
		found = 0;
		fread(&user, sizeof(user), 1, fp);
		if (feof(fp))
		{
			break;
		}
		struct table userCopy;
		int iterasi = 0;
		int j = 0;
		while (j < user.jumlahkolom)
		{
			if (j == index && datake != 0 && strcmp(user.data[j], where) == 0)
			{
				found = 1;
			}
			strcpy(userCopy.data[iterasi], user.data[j]);
			strcpy(userCopy.type[iterasi], user.type[j]);
			iterasi++;
			j++;
		}

		userCopy.jumlahkolom = user.jumlahkolom;
		if (found != 1)
		{
			fwrite(&userCopy, sizeof(userCopy), 1, fp1);
		}
		datake++;
	}
	fclose(fp);
	fclose(fp1);
	remove(table);
	rename("temp", table);
	return 0;
}

void writelog(char *perintah, char *name)
{
	time_t rawtime;
	struct tm *timeinfo;
	time(&rawtime);
	timeinfo = localtime(&rawtime);

	char infoWriteLog[1000];

	FILE *fp;
	fp = fopen("logUser.log", "ab");

	sprintf(infoWriteLog, "%d-%.2d-%.2d %.2d:%.2d:%.2d::%s::%s\n", timeinfo->tm_year + 1900, timeinfo->tm_mon + 1, timeinfo->tm_mday, timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec, name, perintah);

	fputs(infoWriteLog, fp);
	fclose(fp);
	return;
}

int main()
{

	int sockfd, ret;
	struct sockaddr_in serverAddr;

	int newSocket;
	struct sockaddr_in newAddr;

	socklen_t addr_size;

	char buffer[1024];
	pid_t childpid;

	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0)
	{
		printf("[-]Error in connection.\n");
		exit(1);
	}
	printf("[+]Server Socket is created.\n");

	memset(&serverAddr, '\0', sizeof(serverAddr));
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(PORT);
	serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");

	ret = bind(sockfd, (struct sockaddr *)&serverAddr, sizeof(serverAddr));
	if (ret < 0)
	{
		printf("[-]Error in binding.\n");
		exit(1);
	}
	printf("[+]Bind to port %d\n", 4444);

	if (listen(sockfd, 10) == 0)
	{
		printf("[+]Listening....\n");
	}
	else
	{
		printf("[-]Error in binding.\n");
	}

	while (1)
	{
		newSocket = accept(sockfd, (struct sockaddr *)&newAddr, &addr_size);
		if (newSocket < 0)
		{
			exit(1);
		}
		printf("Connection accepted from %s:%d\n", inet_ntoa(newAddr.sin_addr), ntohs(newAddr.sin_port));

		if ((childpid = fork()) == 0)
		{
			close(sockfd);

			while (1)
			{
				recv(newSocket, buffer, 1024, 0);
				// printf("buffer %s\n", buffer);
				char *token;
				char buffercopy[32000];
				strcpy(buffercopy, buffer);
				char perintah[100][10000];
				token = strtok(buffercopy, ":");
				int j = 0;
				char database_used[1000];
				while (token != NULL)
				{
					strcpy(perintah[j], token);
					j++;
					token = strtok(NULL, ":");
				}
				if (strcmp(perintah[0], "cUser") == 0 && strcmp(perintah[3], "0") == 0)
				{
					createUser(perintah[1], perintah[2]);
				}
				else if (strcmp(perintah[0], "cUser") == 0)
				{
					send(newSocket, "Ga berhak", strlen("Ga berhak"), 0);
					bzero(buffer, sizeof(buffer));
				}
				else if (strcmp(perintah[0], "gPermission") == 0 && strcmp(perintah[3], "0") == 0)
				{
					int exist = cekUserExist(perintah[2]);
					if (exist == 1)
					{
						insertPermission(perintah[2], perintah[1]);
					}
					else
					{
						send(newSocket, "User ga ada", strlen("User ga ada"), 0);
						bzero(buffer, sizeof(buffer));
					}
				}
				else if (strcmp(perintah[0], "gPermission") == 0)
				{
					send(newSocket, "Ga berhak", strlen("Ga berhak"), 0);
					bzero(buffer, sizeof(buffer));
				}
				else if (strcmp(perintah[0], "cDatabase") == 0)
				{
					char lokasi[20000];
					snprintf(lokasi, sizeof lokasi, "databases/%s", perintah[1]);
					printf("lokasi = %s, nama = %s , database = %s\n", lokasi, perintah[2], perintah[1]);
					mkdir(lokasi, 0777);
					insertPermission(perintah[2], perintah[1]);
				}
				else if (strcmp(perintah[0], "uDatabase") == 0 && strcmp(perintah[3], "0") != 0)
				{
					int allowed = cekAllowedDatabase(perintah[2], perintah[1]);
					if (allowed != 1)
					{
						send(newSocket, "Access_database : Ga berhak", strlen("Access_database : Ga berhak"), 0);
						bzero(buffer, sizeof(buffer));
					}
					else
					{
						strcpy(database_used, perintah[1]);
						send(newSocket, "Access_database : Berhak", strlen("Access_database : Berhak"), 0);
						bzero(buffer, sizeof(buffer));
					}
				}
				else if (strcmp(perintah[0], "cekCurrentDatabase") == 0 && database_used[0] == '\0')
				{
					send(newSocket, "Tidak ada database yang digunakan", strlen("Tidak ada database yang digunakan"), 0);
					bzero(buffer, sizeof(buffer));
				}
				else if (strcmp(perintah[0], "cekCurrentDatabase") == 0)
				{
					send(newSocket, database_used, strlen(database_used), 0);
					bzero(buffer, sizeof(buffer));
				}
				else if (strcmp(perintah[0], "cTable") == 0 && database_used[0] == '\0')
				{
					char *tokens;
					strcpy(database_used, "Tidak ada database yang digunakan");
					send(newSocket, database_used, strlen(database_used), 0);
					bzero(buffer, sizeof(buffer));
				}
				else if (strcmp(perintah[0], "cTable") == 0)
				{
					char *tokens;
					char daftarQuery[100][10000];
					char copyPerintah[20000];
					snprintf(copyPerintah, sizeof copyPerintah, "%s", perintah[1]);
					tokens = strtok(copyPerintah, "(), ");
					int jumlah = 0;
					while (tokens != NULL)
					{
						strcpy(daftarQuery[jumlah], tokens);
						printf("%s\n", daftarQuery[jumlah]);
						jumlah++;
						tokens = strtok(NULL, "(), ");
					}
					char buatTable[20000];
					snprintf(buatTable, sizeof buatTable, "../database/databases/%s/%s", database_used, daftarQuery[2]);
					int iterasi = 0;
					int iterasiData = 3;
					struct table kolom;
					while (jumlah > 3)
					{
						strcpy(kolom.data[iterasi], daftarQuery[iterasiData]);
						printf("%s\n", kolom.data[iterasi]);
						strcpy(kolom.type[iterasi], daftarQuery[iterasiData + 1]);
						iterasiData = iterasiData + 2;
						jumlah = jumlah - 2;
						iterasi++;
					}
					kolom.jumlahkolom = iterasi;
					printf("iterasi = %d\n", iterasi);
					FILE *fp;
					fp = fopen(buatTable, "ab");
					fwrite(&kolom, sizeof(kolom), 1, fp);
					fclose(fp);
				}
				else if (strcmp(perintah[0], "dDatabase") == 0)
				{
					int allowed = cekAllowedDatabase(perintah[2], perintah[1]);
					if (allowed != 1)
					{
						send(newSocket, "Database ga berhak", strlen("Database ga berhak"), 0);
						bzero(buffer, sizeof(buffer));
						continue;
					}
					else
					{
						char hapus[20000];
						snprintf(hapus, sizeof hapus, "rm -r databases/%s", perintah[1]);
						system(hapus);
						char peringatan[] = "Database Has Been Removed";
						send(newSocket, peringatan, strlen(peringatan), 0);
						bzero(buffer, sizeof(buffer));
					}
				}
				else if (strcmp(perintah[0], "dTable") == 0 && database_used[0] == '\0')
				{
					send(newSocket, "Tidak ada database yang digunakan", strlen("Tidak ada database yang digunakan"), 0);
					bzero(buffer, sizeof(buffer));
					continue;
				}
				else if (strcmp(perintah[0], "dTable") == 0)
				{
					char hapus[20000];
					snprintf(hapus, sizeof hapus, "databases/%s/%s", database_used, perintah[1]);
					remove(hapus);
					send(newSocket, "Table dah dihapus", strlen("Table dah dihapus"), 0);
					bzero(buffer, sizeof(buffer));
				}
				else if (strcmp(perintah[0], "dColumn") == 0 && database_used[0] == '\0')
				{
					send(newSocket, "Tidak ada database yang digunakan", strlen("Tidak ada database yang digunakan"), 0);
					bzero(buffer, sizeof(buffer));
					continue;
				}
				else if (strcmp(perintah[0], "dColumn") == 0)
				{
					char pathTable[20000];
					snprintf(pathTable, sizeof pathTable, "databases/%s/%s", database_used, perintah[2]);
					int index = findColumn(pathTable, perintah[1]);
					if (index == -1)
					{
						send(newSocket, "Column ga nemu", strlen("Column ga nemu"), 0);
						bzero(buffer, sizeof(buffer));
						continue;
					}
					deleteColumn(pathTable, index);
					send(newSocket, "Column dah dihapus", strlen("Column dah dihapus"), 0);
					bzero(buffer, sizeof(buffer));
				}
				else if (strcmp(perintah[0], "insert") == 0 && database_used[0] == '\0')
				{
					send(newSocket, "Tidak ada database yang digunakan", strlen("Tidak ada database yang digunakan"), 0);
					bzero(buffer, sizeof(buffer));
					continue;
				}
				else if (strcmp(perintah[0], "insert") == 0)
				{
					char daftarQuery[100][10000];
					char copyPerintah[20000];
					snprintf(copyPerintah, sizeof copyPerintah, "%s", perintah[1]);
					char *tokens;
					tokens = strtok(copyPerintah, "\'(), ");
					int jumlah = 0;
					while (tokens != NULL)
					{
						strcpy(daftarQuery[jumlah], tokens);
						jumlah++;
						tokens = strtok(NULL, "\'(), ");
					}
					char buatTable[20000];
					snprintf(buatTable, sizeof buatTable, "databases/%s/%s", database_used, daftarQuery[2]);
					int banyakKolom;

					FILE *fp;
					fp = fopen(buatTable, "r");
					if (fp == NULL)
					{
						send(newSocket, "TABLE NOT FOUND", strlen("TABLE NOT FOUND"), 0);
						bzero(buffer, sizeof(buffer));
						continue;
					}
					else
					{
						struct table user;
						fread(&user, sizeof(user), 1, fp);
						banyakKolom = user.jumlahkolom;
						fclose(fp);
					}
					int iterasi = 0;
					int iterasiData = 3;
					struct table kolom;
					while (jumlah > 3)
					{
						strcpy(kolom.data[iterasi], daftarQuery[iterasiData]);
						printf("%s\n", kolom.data[iterasi]);
						strcpy(kolom.type[iterasi], "string");
						iterasiData++;
						jumlah--;
						iterasi++;
					}
					kolom.jumlahkolom = iterasi;
					if (banyakKolom != kolom.jumlahkolom)
					{
						send(newSocket, "Input tidak sesuai", strlen("Input tidak sesuai"), 0);
						bzero(buffer, sizeof(buffer));
						continue;
					}
					FILE *fp1;
					fp1 = fopen(buatTable, "ab");
					fwrite(&kolom, sizeof(kolom), 1, fp1);
					fclose(fp1);
					send(newSocket, "Data dah masuk", strlen("Data dah masuk"), 0);
					bzero(buffer, sizeof(buffer));
				}
				else if (strcmp(perintah[0], "update") == 0 && database_used[0] == '\0')
				{
					send(newSocket, "Tidak ada database yang digunakan", strlen("Tidak ada database yang digunakan"), 0);
					bzero(buffer, sizeof(buffer));
					continue;
				}
				else if (strcmp(perintah[0], "update") == 0)
				{
					char daftarQuery[100][10000];
					char copyPerintah[20000];
					snprintf(copyPerintah, sizeof copyPerintah, "%s", perintah[1]);
					char *tokens;
					tokens = strtok(copyPerintah, "\'(),= ");
					int jumlah = 0;
					while (tokens != NULL)
					{
						strcpy(daftarQuery[jumlah], tokens);
						printf("%s\n", daftarQuery[jumlah]);
						jumlah++;
						tokens = strtok(NULL, "\'(),= ");
					}
					char buatTable[20000];
					snprintf(buatTable, sizeof buatTable, "databases/%s/%s", database_used, daftarQuery[1]);
					if (jumlah == 5)
					{
						int index = findColumn(buatTable, daftarQuery[3]);
						if (index == -1)
						{
							send(newSocket, "Column ga nemu", strlen("Column ga nemu"), 0);
							bzero(buffer, sizeof(buffer));
							continue;
						}
						printf("index = %d\n", index);
						updateColumn(buatTable, index, daftarQuery[4]);
					}
					else if (jumlah == 8)
					{
						printf("buat table = %s, kolumn = %s", buatTable, daftarQuery[3]);
						int index = findColumn(buatTable, daftarQuery[3]);
						if (index == -1)
						{
							send(newSocket, "Column ga nemu", strlen("Column ga nemu"), 0);
							bzero(buffer, sizeof(buffer));
							continue;
						}
						printf("%s\n", daftarQuery[7]);
						int indexGanti = findColumn(buatTable, daftarQuery[6]);
						updateColumnWhere(buatTable, index, daftarQuery[4], indexGanti, daftarQuery[7]);
					}
					else
					{
						send(newSocket, "Data dah dihapus", strlen("Data dah dihapus"), 0);
						bzero(buffer, sizeof(buffer));
						continue;
					}
					send(newSocket, "Data dah diupdate", strlen("Data dah diupdate"), 0);
					bzero(buffer, sizeof(buffer));
				}
				else if (strcmp(perintah[0], "delete") == 0)
				{
					if (database_used[0] == '\0')
					{
						strcpy(database_used, "You're not selecting database yet");
						send(newSocket, database_used, strlen(database_used), 0);
						bzero(buffer, sizeof(buffer));
						continue;
					}
					char sql_Query[100][10000];
					char copyPerintah[20000];
					snprintf(copyPerintah, sizeof copyPerintah, "%s", perintah[1]);
					// printf("%s\n", copyPerintah);
					char *tokens;
					tokens = strtok(copyPerintah, "\'(),= ");
					int jumlah = 0;
					while (tokens != NULL)
					{
						strcpy(sql_Query[jumlah], tokens);
						printf("%s\n", sql_Query[jumlah]);
						jumlah++;
						tokens = strtok(NULL, "\'(),= ");
					}
					printf("jumlah = %d\n", jumlah);
					char buatTable[20000];
					snprintf(buatTable, sizeof buatTable, "databases/%s/%s", database_used, sql_Query[2]);
					if (jumlah == 3)
					{
						deleteTable(buatTable);
					}
					else if (jumlah == 6)
					{
						int index = findColumn(buatTable, sql_Query[4]);
						if (index == -1)
						{
							char peringatan[] = "Column Not Found";
							send(newSocket, peringatan, strlen(peringatan), 0);
							bzero(buffer, sizeof(buffer));
							continue;
						}
						printf("index  = %d\n", index);
						deleteTableWhere(buatTable, index, sql_Query[4], sql_Query[5]);
					}
					else
					{
						char peringatan[] = "Input Salah";
						send(newSocket, peringatan, strlen(peringatan), 0);
						bzero(buffer, sizeof(buffer));
						continue;
					}
					char peringatan[] = "Data Has Been Deleted";
					send(newSocket, peringatan, strlen(peringatan), 0);
					bzero(buffer, sizeof(buffer));
				}
				else if (compare2(perintah[0], "select",database_used[0] ,'\0')){
					send(newSocket, "Tidak ada database yang digunakan", strlen("Tidak ada database yang digunakan"), 0);
					bzero(buffer, sizeof(buffer));
					continue;
				}
				else if (compare1(perintah[0],"select")){
					char sql_Query[150][15000];
					char log_perintah[20000];
					snprintf(log_perintah, sizeof log_perintah, "%s", perintah[1]);
					char *tokens;
					tokens = strtok(log_perintah, "\'(),= ");
					int countingdata = 0;

					while (tokens != NULL) {
						strcpy(sql_Query[countingdata], tokens);
						countingdata++;
						tokens = strtok(NULL, "\'(),= ");
					}

					if (countingdata == 4) {
						char TableColomn[20000], perintahKolom[1000];
						snprintf(TableColomn, sizeof TableColomn, "databases/%s/%s", database_used, sql_Query[3]);
						if (compare1(sql_Query[1], "*")){
							FILE *fp, *fp1;
							char buffers[40000], DatabaseSending[40000];
							struct table user;
							int id, found = 0;
							fp = fopen(TableColomn, "rb");
							bzero(buffer, sizeof(buffer));
							bzero(DatabaseSending, sizeof(DatabaseSending));
							while (1) {
								fread(&user, sizeof(user), 1, fp);
								snprintf(buffers, sizeof buffers, "\n");
								if (feof(fp))
									break;
								for (int i = 0; i < user.jumlahkolom; i++) {
									char padding[20000];
									snprintf(padding, sizeof padding, "%s\t", user.data[i]);
									strcat(buffers, padding);
								}
								strcat(DatabaseSending, buffers);
							}
							send(newSocket, DatabaseSending, strlen(DatabaseSending), 0);
							bzero(DatabaseSending, sizeof(DatabaseSending));
							bzero(buffer, sizeof(buffer));
							fclose(fp);
						}
						else {
							int index = findColumn(TableColomn, sql_Query[1]);
							FILE *fp, *fp1;
							struct table user;
							int id, found = 0;
							char buffers[40000];
							char DatabaseSending[40000];
							fp = fopen(TableColomn, "rb");
							bzero(buffer, sizeof(buffer));
							bzero(DatabaseSending, sizeof(DatabaseSending));
							while (1) {
								fread(&user, sizeof(user), 1, fp);
								snprintf(buffers, sizeof buffers, "\n");
								if (feof(fp))
									break;
								int i;
								while(i<user.jumlahkolom){
									if (i == index){
										char padding[20000];
										snprintf(padding, sizeof padding, "%s\t", user.data[i]);
										strcat(buffers, padding);
									}
									i++;
								}	
								strcat(DatabaseSending, buffers);
							}
							fclose(fp);
							send(newSocket, DatabaseSending, strlen(DatabaseSending), 0);
							bzero(DatabaseSending, sizeof(DatabaseSending));
							bzero(buffer, sizeof(buffer));
						}
					}
					else if (countingdata == 7 && strcmp(sql_Query[4], "WHERE") == 0)
					{
						char buatTable[20000];
						snprintf(buatTable, sizeof buatTable, "databases/%s/%s", database_used, sql_Query[3]);
						printf("buat table = %s", buatTable);
						char perintahKolom[1000];
						printf("masuk 4\n");
						if (strcmp(sql_Query[1], "*") == 0)
						{
							FILE *fp, *fp1;
							struct table user;
							int id, found = 0;
							fp = fopen(buatTable, "rb");
							char buffers[40000];
							char sendDatabase[40000];
							int index = findColumn(buatTable, sql_Query[5]);
							printf("%d\n", index);
							bzero(buffer, sizeof(buffer));
							bzero(sendDatabase, sizeof(sendDatabase));
							while (1)
							{
								fread(&user, sizeof(user), 1, fp);
								snprintf(buffers, sizeof buffers, "\n");
								if (feof(fp))
									break;
								int i = 0;
								while (i<user.jumlahkolom){
									if (strcmp(user.data[index], sql_Query[6]) == 0)
									{
										char padding[20000];
										snprintf(padding, sizeof padding, "%s\t", user.data[i]);
										strcat(buffers, padding);
									}
									i++;
								}
								strcat(sendDatabase, buffers);
							}
							send(newSocket, sendDatabase, strlen(sendDatabase), 0);
							bzero(sendDatabase, sizeof(sendDatabase));
							bzero(buffer, sizeof(buffer));
							fclose(fp);
						}
						else
						{
							int index = findColumn(buatTable, sql_Query[1]);
							printf("%d\n", index);
							FILE *fp, *fp1;
							struct table user;
							int id, found = 0;
							int indexGanti = findColumn(buatTable, sql_Query[5]);
							fp = fopen(buatTable, "rb");
							char buffers[40000];
							char sendDatabase[40000];
							bzero(buffer, sizeof(buffer));
							bzero(sendDatabase, sizeof(sendDatabase));
							while (1)
							{
								char enter[] = "\n";
								fread(&user, sizeof(user), 1, fp);
								snprintf(buffers, sizeof buffers, "\n");
								if (feof(fp))
									break;
								int i =1;
								while(i<user.jumlahkolom){
									if (i == index && (strcmp(user.data[indexGanti], sql_Query[6]) == 0 || strcmp(user.data[i], sql_Query[5]) == 0))
									{
										char padding[20000];
										snprintf(padding, sizeof padding, "%s\t", user.data[i]);
										strcat(buffers, padding);
									}
									i++;
								}
								strcat(sendDatabase, buffers);
							}
							fclose(fp);
							send(newSocket, sendDatabase, strlen(sendDatabase), 0);
							bzero(sendDatabase, sizeof(sendDatabase));
							bzero(buffer, sizeof(buffer));
						}
					}
					else
					{

						if (strcmp(sql_Query[countingdata - 3], "WHERE") != 0)
						{
							char buatTable[20000];
							snprintf(buatTable, sizeof buatTable, "databases/%s/%s", database_used, sql_Query[countingdata - 1]);
							int index[100];
							int i = 1;
							while (i < countingdata -2){
								index[i-1] = findColumn(buatTable, sql_Query[i]);
								printf("%d\n", index[i-1]);
								i++;
							}
						}
						else if (strcmp(sql_Query[countingdata - 3], "WHERE") == 0)
						{
							printf("dengan where");
						}
					}
				}
				if (strcmp(buffer, ":exit") == 0){
					printf("koneksi server %s : %d telah hilang\n", inet_ntoa(newAddr.sin_addr), ntohs(newAddr.sin_port));
					break;
				}
				else {
					printf("Client message : %s\n", buffer);
					send(newSocket, buffer, strlen(buffer), 0);
					bzero(buffer, sizeof(buffer));
				}
			}
		}
	}

	close(newSocket);

	return 0;
}
