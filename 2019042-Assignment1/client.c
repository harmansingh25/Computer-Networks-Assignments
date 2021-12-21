#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include<fcntl.h>
#include<errno.h>
#define PORT 8080

struct process_info{

	char name[1000];
	int pid;
	unsigned long cpu_user;
	unsigned long cpu_kernel;
	unsigned long totalTime;
};

int main(int argc, char *argv[]){


	


	//Creating client socket
	int clientfd = socket(AF_INET, SOCK_STREAM, 0);
	if(clientfd<0){
		perror("Can't create socket");
		return -1;
	}

	struct sockaddr_in server;
	//Initialising server's address information
	server.sin_family = AF_INET;
	server.sin_port = htons(PORT);


	//Converting IPv4 and IPv6 from text to binary. Source : GFG
	int convert = inet_pton(AF_INET, "127.0.0.1", &server.sin_addr);
	if(convert<=0){
		perror("Error in conversion of IP from text to binary");
	}

	//Sending a connection request to server
	int connectServ = connect(clientfd, (struct sockaddr *)&server, sizeof(server));
	if(connectServ<0){
		perror("Can't connect to the server");
		return -1;
	}



	char insp;
	printf("Press any character to request server's top 10 CPU consuming processes: ");
	scanf("%c",&insp);



	char server_buff[10000], client_buff[10000];
	
	send(clientfd, "Send me processes", strlen("Send me processes"),0);
	//Reading message sent by server
	int readSys = read(clientfd, client_buff, sizeof(client_buff) );
	if(readSys<0){
    	perror("Can't read from server\n");
    	return -1;
    }
	printf("%s\n", client_buff);

	




	//Storing contents sent by server to a file
	 FILE* fp;
    fp = fopen("server1.txt","w");
     fprintf(fp,"%s",client_buff);
     fclose(fp);




     //Sending the highest CPU consuming process (Using same code from server.c)

     //Finding max pid in the system
    char cTemp[1000];
    int fdMaxPid = open("/proc/sys/kernel/pid_max", O_RDONLY);
    if(fdMaxPid==-1){
    	printf("Can't find max_pid");
    	return -1;
    }

    //Reading max pid possible
	int sz2 = read(fdMaxPid,cTemp,500);
	cTemp[sz2] = '\0';

	//Converting string to int

    int maxPid = atoi(cTemp);

    //Array to store structs of process_info

    struct process_info process[1000];
 

    int j=0;
    //Iterating from 0 to maxpid
    for(int i=0; i<maxPid; i++){

  

    	char pidStr[10000];
    	sprintf(pidStr, "%d", i);

    	//Concatenating file to be opened
    	char finalString[10000];
    	snprintf(finalString, sizeof(finalString), "%s%s%s", "/proc/", pidStr, "/stat");

    	int fd = open(finalString, O_RDONLY);

    	
    	//If such a process exists, extract infromation from /../..stat
    	if(fd!=-1){

	    	char ch[10000];
	    	int szh=read(fd,ch,5000);
	    	ch[szh]='\0';
	    	char chTemp[1000]; //For storing copy of the string
	    	strcpy(chTemp, ch);
	    	char* temp2 = NULL;
	    	char *token = strtok_r(ch," ",&temp2);
	    	int c = 1; 

	    	
	    	char namew[10000];
	    	int pidw;
			unsigned long cpu_userw;
			unsigned long cpu_kernelw;
			unsigned long totalTimew;

	
	    	while(token!=NULL){
	    		
	    		if(c==1){
	    			char temp[10000];
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
	    			char temp[10000];
	    			strcpy(temp,token);
	    			cpu_userw=strtoul(temp,&eptr,10);

	    		}
	    		if(c==15){
	    			char *eptr;
	    			char temp[10000];
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

    char temp3[10000]="Top CPU Consuming process: \n";
    snprintf(server_buff,sizeof(server_buff),"%s%d%s%s%s%lu%s%lu%s%lu\n\n","Process ID: ",finalArr[0].pid," Process name: ", finalArr[0].name, " CPU_UserSpace_time: ",finalArr[0].cpu_user," CPU_KernelSpace_time: ",finalArr[0].cpu_kernel," Total_CPU_Consumption: ",finalArr[0].totalTime);
    strcat(temp3,server_buff);
    //Sending message to client
	send(clientfd, temp3, strlen(temp3), 0);
	printf("Information about my top CPU consuming process sent to server\n");

	sleep(30);
	close(clientfd);
	return 0;




}