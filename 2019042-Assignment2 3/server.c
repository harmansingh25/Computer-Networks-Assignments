#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <dirent.h>
#include<fcntl.h>
#include<errno.h>
#include <arpa/inet.h>
#define PORT 8080


//Creating a struct to process and send to client
struct process_info{

	char name[100];
	int pid;
	unsigned long cpu_user;
	unsigned long cpu_kernel;
	unsigned long totalTime;
};


//Arguments to pass in the thread (information about client)
struct argsPass{

	struct sockaddr_in cadrs;
	int cfd;
	int fileNo;
};


void *clientReq(void *args){


	

	
	struct argsPass *recvd = args;
	int clFd = recvd->cfd;
	int fNo= recvd->fileNo;
	struct sockaddr_in clAdr = recvd->cadrs;


	

	char iniReq[10000];

	read(clFd, iniReq, sizeof(iniReq));
	char strPrint[1024];
	snprintf(strPrint, sizeof(strPrint), "%s%s%s%d%s\n\n", "Client with sin_addr: ", inet_ntoa(clAdr.sin_addr), " and sin_port: ", ntohs(clAdr.sin_port), " requested top 10 CPU consuming processes." );
	printf("%s\n", strPrint);
	


	
	while(1){

//Finding max pid in the system
    char cTemp[1000];
    int fdMaxPid = open("/proc/sys/kernel/pid_max", O_RDONLY);
    if(fdMaxPid==-1){
    	printf("Can't find max_pid");
    	
    }



    //Reading max pid possible
	int sz2 = read(fdMaxPid,cTemp,500);
	cTemp[sz2] = '\0';

	//Converting string to int

    int maxPid = atoi(cTemp);

    //Array to store structs of process_info

    struct process_info process[3000]; //Assumption: Max process taking to be 3000(From analysis)
 


 

    int j=0;
    //Iterating from 0 to maxpid
    for(int i=0; i<maxPid; i++){



  

    	char pidStr[100];
    	sprintf(pidStr, "%d", i);

    	//Concatenating file to be opened
    	char finalString[100];
    	snprintf(finalString, sizeof(finalString), "%s%s%s", "/proc/", pidStr, "/stat");

    	int fd = open(finalString, O_RDONLY);

    	
    	//If such a process exists, extract infromation from /../..stat
    	if(fd!=-1){

	    	char ch[1000];
	    	int szh=read(fd,ch,500);
	    	ch[szh]='\0';
	    	char chTemp[1000]; //For storing copy of the string
	    	strcpy(chTemp, ch);
	    	char* temp2 = NULL;
	    	char *token = strtok_r(ch," ",&temp2);
	    	int c = 1; 

	    	
	    	char namew[100];
	    	int pidw;
			unsigned long cpu_userw;
			unsigned long cpu_kernelw;
			unsigned long totalTimew;

	
	    	while(token!=NULL){
	    		
	    		if(c==1){
	    			char temp[100];
	    			strcpy(temp,token);
	    			pidw = atoi(temp);
	    		}
	    		if(c==2){

	    			//Handling edge case

	    			char hurray[1000];
	    			strcpy(hurray, token);

	    			int len = strlen(hurray);
	    			//printf("%c\n", hurray[len-1]);
	    			if(hurray[len-1]!=')'){

	    				//int count_spaces = 0;

	    				char tempo[1000];
	    				strcpy(tempo,token);

	    				int ola = 1;

	    				while(token!=NULL){


	    					if(ola!=1){
	    					strcat(tempo,token);
	    				}
	    					int len2 = strlen(tempo);
	    					if(tempo[len2-1]==')'){
	    					
	    						break;
	    					}
	    					else{
	    						//count_spaces++;
	    						strcat(tempo," ");
	    					
	    					}

	    					token=strtok_r(NULL," ",&temp2);
	    					ola++;
	    				

	    				}

	    				strcpy(namew, tempo);



	    			}
	    			else{
	    				strcpy(namew,token);
	    			}





	    		}
	    		if(c==14){
	    			char *eptr;
	    			char temp[100];
	    			strcpy(temp,token);
	    			cpu_userw=strtoul(temp,&eptr,10);

	    		}
	    		if(c==15){
	    			char *eptr;
	    			char temp[100];
	    			strcpy(temp,token);
	    			cpu_kernelw=strtoul(temp,&eptr,10);

	    		}
	    		c++;
	    		token=strtok_r(NULL," ",&temp2);


	    	}
	    
	    	//Total CPU time
	    	totalTimew = cpu_kernelw+cpu_userw;
	    	//struct process_info currProcess = {namew,pidw,cpu_userw,cpu_kernelw,totalTimew};

	    	memcpy(process[j].name, namew, sizeof(namew));
	    	process[j].pid = pidw;
	    	process[j].cpu_user = cpu_userw;
	    	process[j].cpu_kernel = cpu_kernelw;
	    	process[j].totalTime = totalTimew;

	    	j++;
	    	// printf("Content of proc: %s\n",chTemp);
	    	// printf("Tokenised content is %s %d %lu %lu %lu\n\n",namew,pidw,cpu_userw,cpu_kernelw,totalTimew);

    	}

    	close(fd);

    }


   

    j++;
 
    if(j<1000){
    	process[j].pid = -1;
    }

  

    int count = 0;
    int i=0;
    while(process[i].pid!=-1){
    	count++;
    	i++;
    }

    struct process_info finalArr[count];
    
    //copying for final array
    for(int i=0;i<count;i++){
    	finalArr[i]=process[i];
    }




    //Sorting processes in decreasing order

    struct process_info temp;

    for(int i=0;i<count-1;i++){
    	for(int j=0;j<(count-1-i);j++){
    		if(finalArr[j].totalTime<finalArr[j+1].totalTime){

    			temp = finalArr[j];
    			finalArr[j] = finalArr[j+1];
    			finalArr[j+1] = temp;

    		}
    	}
    }





    
    FILE* fp;

    char fileName[100];
    snprintf(fileName, sizeof(fileName), "%s%s%d%s%d%s", inet_ntoa(clAdr.sin_addr),".", ntohs(clAdr.sin_port), ".",fNo,".txt");

    fp = fopen(fileName,"w");
    if(fp==NULL){
    	perror("error");
    }


    fprintf(fp,"%s\n\n","Top 10 CPU consuming processes are: ");


    for(int i=0;i<10;i++){
    
    	
    	fprintf(fp,"%d %s %s %d %s %s %s %lu %s %lu %s %lu\n\n",(i+1),":","Process ID:",finalArr[i].pid,"Process name:", finalArr[i].name, "CPU_UserSpace_time:",finalArr[i].cpu_user,"CPU_KernelSpace_time:",finalArr[i].cpu_kernel,"Total_CPU_Consumption:",finalArr[i].totalTime);
    }
    fclose(fp);
   



    char server_buff[10000], client_buff[10000];
    //FILE* fp2;
    int fp2 = open(fileName,O_RDONLY);

    int kk = read(fp2,server_buff, sizeof(server_buff));
    server_buff[kk] = '\0';

    /*char buffer_read[10000];
   	while(fgets(buffer_read,sizeof(buffer_read),fp2)!=NULL){
   		
   		strcat(server_buff, buffer_read);
   		
   	}

   	printf("%s\n",server_buff);*/
   	



   	close(fp2);

    send(clFd, server_buff, strlen(server_buff), 0);

    char confirm[1024];
 

    snprintf(confirm, sizeof(confirm), "%s%s%s%d\n\n", "Details sent to client with sin_addr: ", inet_ntoa(clAdr.sin_addr), " and sin_port: ", ntohs(clAdr.sin_port) );
    printf("%s\n", confirm);


    char confirmRecv[1024];

    int readSys = read(clFd, client_buff, sizeof(client_buff));

    client_buff[readSys] = '\0';
 

    snprintf(confirmRecv, sizeof(confirmRecv), "%s%s%s%d%s\n\n", "Details sent by client with sin_addr: ", inet_ntoa(clAdr.sin_addr), " and sin_port: ", ntohs(clAdr.sin_port),": ");



    
     printf("%s\n", confirmRecv);

         printf("%s\n", client_buff);


	}



	close(clFd);
	pthread_exit(0);




}
int main(){

	//Creating TCP socket

	int socketfd = socket(AF_INET, SOCK_STREAM, 0);
	int afk = 1;

	//Checking if file_descriptor is less than 0
	if(socketfd<0){
		perror("Socket can't be created due to some error.");
		return -1;

	}

	//Optional but writing to prevent error such as "address already in use". Source: tutorialspoint.dev, GFG
	if (setsockopt(socketfd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &afk, sizeof(afk))){

		perror("Can't attach socket ");
		return -1;
	}

	//Initialising server address by port and IP
	struct sockaddr_in adrs;
	struct sockaddr_in cadrs;
	adrs.sin_family = AF_INET;
    adrs.sin_addr.s_addr = INADDR_ANY;
    adrs.sin_port = htons(PORT);


    //Assigning local protocol address to the socket
    int bindFd = bind(socketfd, (struct sockaddr *)&adrs, sizeof(adrs));
    if(bindFd<0){
    	perror("Can't assign local protocol address to a socket");
    	return -1;
    }

  // Listen for clients
    int listens = listen(socketfd, 5);
    if(listens<0){
    	perror("Listening error");
    	return -1;
    }




    pthread_t pid;
    int i=0;

    while(1){

    	struct sockaddr_in cadrs;
    	int lenAdrs = sizeof(cadrs);
    //Accepting request and storing client's address
    	int clientFd = accept(socketfd, (struct sockaddr *)&cadrs,(socklen_t*)&lenAdrs);
    	i++;
    	
   		if(clientFd<0){
    		perror("Accept failed from a client");
    		return -1;
   		 }


   		 struct argsPass topass={cadrs, clientFd, i};
   		 pthread_create(&pid, NULL, clientReq, (void*)&topass);
   		 pthread_detach(pid);




    }

   
    





}