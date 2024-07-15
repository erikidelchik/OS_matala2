#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>
#include <sys/wait.h>
#include <sys/un.h>


#define sockpath "/tmp/my_socket"


void runttt(char *argv[]){
    	if(!strcmp(argv[1],"-e")){
	    char *arg = argv[2];
	    char *num = strchr(arg,' ');
	    if(num==NULL){
		printf("false arguments");
		exit(1);
	    }
	    num++;
	    execl("ttt","ttt",num,NULL);
	    printf("execl failed");
	    exit(1);
    	}
}




void start_tcp_server(int* sockfd,int port){
	*sockfd = socket(AF_INET,SOCK_STREAM,0);
	if(*sockfd==-1){
	    printf("socket opening error\n");
	    exit(1);
	}
	struct sockaddr_in serv_addr;
	
	memset(&serv_addr,0,sizeof(serv_addr));
	
	const int enable = 1;
	if (setsockopt(*sockfd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) < 0){
    		perror("setsockopt failed");
    		exit(1);
	}
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(port);
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	
	if(bind(*sockfd,(struct sockaddr *)&serv_addr,sizeof (serv_addr))<0){
	    printf("bind failed");
	    exit(1);
	
	}
	printf("socket is bound to port %d\n",port);
	if(listen(*sockfd,3)<0){
	    printf("listen");
	    exit(1);
	}
	
	printf("listening to port %d\n",port);
	
	
}


//connects to the port that is listening
void start_tcp_listener(int* sockfd,char *hostname,int port){

	    struct sockaddr_in serv_addr;

	    *sockfd = socket(AF_INET, SOCK_STREAM, 0);
	    if (*sockfd < 0) {
		perror("socket failed");
		exit(1);
	    }

	    memset(&serv_addr, 0, sizeof(serv_addr));
	    serv_addr.sin_family = AF_INET;
	    serv_addr.sin_port = htons(port);
	    
	    
	    if (inet_pton(AF_INET, hostname, &serv_addr.sin_addr) <= 0) {
		perror("inet_pton failed");
		close(*sockfd);
		exit(1);
	    }

	    if (connect(*sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
		perror("connect failed");
		close(*sockfd);
		exit(1);
	    }

	    printf("Connected to %s on port %d\n", hostname, port);

}

void without_e(int clientfd,char *argv[]){	
	char buffer[1024];
	pid_t pid = fork();
	
	if(pid==0){
		//redirect the stdinput to the client 
	    	if(!strcmp(argv[1],"-i") || !strcmp(argv[1],"-o")){
    	     
    	    	    int r;
    	    	    while((r=read(STDIN_FILENO,buffer,sizeof(buffer)))>0){
    	    	    	write(clientfd,buffer,r);
    	    	    		
    	    	    
    	    	    }
    	    	    exit(1);
    	    	    
    	    	    
    	    	}
    	    	
    	        //redirect the stdinput to the client, and the stdoutput to the client
    	        if(!strcmp(argv[1],"-b")){
    	            if(dup2(clientfd,STDIN_FILENO)<0){
    	            	printf("stdin");
    	             	exit(1);
    	            }
    	            
    	            if(dup2(clientfd,STDOUT_FILENO)<0){
    	            	printf("stdout");
    	             	exit(1);
    	            }
    	            close(clientfd);
    	            
    	        }
    	       
	}
	
	else{	
		int r;
		
		if(!strcmp(argv[1],"-i") || !strcmp(argv[1],"-o")){
			while((r=read(clientfd,buffer,sizeof(buffer)))>0){
			    write(STDOUT_FILENO,buffer,r);
			    
			
			}
			close(clientfd);
			waitpid(pid,NULL,0);
			exit(1);
		}
		
	}
	
	
}

void run_i_and_o(int clientfd,int listenerfd,char *argv[]){
	pid_t pid = fork();
	
	
	if(pid==0){
		//redirect the input of ttt to the input of the client
		if(dup2(clientfd,STDIN_FILENO)<0) exit(1);
		     //redirect the stdout to the listening socket
	    	     if(dup2(listenerfd,STDOUT_FILENO)<0) exit(1);
	    	     close(clientfd);
	    	     close(listenerfd);
	    	     runttt(argv);
	}
	else{
	    close(clientfd);
	    close(listenerfd);
	    waitpid(pid,NULL,0);
	    exit(1);
	
	}
    	     
}

void run_i_o_b_tcp(int clientfd,char *argv[]){

	pid_t pid;
    	pid = fork();
    	if(pid==-1){
    		printf("fork error");
    		exit(1);
   	}
    
        if(pid==0){

		//redirect the input of the client to ttt
    	        if(!strcmp(argv[3],"-i")){
    	            if(dup2(clientfd,STDIN_FILENO)<0) exit(1);
    	    	    close(clientfd);
    	    	    runttt(argv);
    	        
    	        }
    	        
    	        //redirect the output of ttt to the client
    	        if(!strcmp(argv[3],"-o")){
    	            if(dup2(clientfd,STDOUT_FILENO)<0) exit(1);
    	    	    close(clientfd);
    	    	    runttt(argv);
    	        
    	        }
    	        
    	        
    	        //redirect the ttt input to the client, and the output of ttt to the client
    	        if(!strcmp(argv[3],"-b")){
    	            if(dup2(clientfd,STDIN_FILENO)<0){
    	            	printf("stdin");
    	             	exit(1);
    	            }
    	            
    	            if(dup2(clientfd,STDOUT_FILENO)<0){
    	            	printf("stdout");
    	             	exit(1);
    	            }
    	            close(clientfd);
    	            runttt(argv);
    	        }
    	        
    	 }
    	 
    	 else{
    	 	close(clientfd);
    	 	waitpid(pid,NULL,0);
    	 	
    	 	if(!strncmp(argv[4],"uds",3))
    	 		unlink(sockpath);
    	 	
    	 	exit(1);
    	 
    	 }

}


void run_basic(char *argv[]){
	pid_t pid = fork();
    	
    	if(pid==0){
    		runttt(argv);
    	}
    	
    	else{
    		waitpid(pid,NULL,0);
    		exit(1);
    	}
    	


}


void start_udp_server(int *sockfd, int port) {
    *sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (*sockfd == -1) {
        perror("socket");
        exit(1);
    }

    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    
    const int enable = 1;
    if (setsockopt(*sockfd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) < 0){
    	perror("setsockopt failed");
    	exit(1);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(*sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("bind failed");
        exit(1);
    }

    printf("Socket is bound to port %d\n", port);
}


void start_udp_client(int port){

    struct sockaddr_in server_addr;
    
    int clientfd = socket(AF_INET, SOCK_DGRAM, 0);
    if(clientfd<0){
    	exit(1);
    }
    
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = INADDR_ANY;
    
    int count;

    char buffer[1024];
    while(fgets(buffer, sizeof(buffer), stdin)!=NULL){
    
    	sendto(clientfd, buffer, strlen(buffer), 0, (const struct sockaddr *) &server_addr, sizeof(server_addr));
    	memset(buffer,'\0',sizeof(buffer));
    }
    
    exit(1);
}




void run_i_o_b_udp(int clientfd,char *argv[]){

    char buffer[1024];
    struct sockaddr_in client_addr;
    socklen_t client_len = sizeof(client_addr);
    
    pid_t pid = fork();
    
    if(pid==0){
    	//redirect the input of the client to the ttt
    	        if(!strcmp(argv[3],"-i")){
    	            if(dup2(clientfd,STDIN_FILENO)<0) exit(1);
    	    	    close(clientfd);
    	    	    runttt(argv);
    	        
    	        }
    	        
    	        //redirect the output of ttt to the client
    	        if(!strcmp(argv[3],"-o")){
    	            if(dup2(clientfd,STDOUT_FILENO)<0) exit(1);
    	    	    close(clientfd);
    	    	    runttt(argv);
    	        
    	        }
    	        
    	        
    	 }
    
    // Parent process to forward data between client and ttt	 
     else {
        while (1) {
           getchar(); //consume 1 input
           ssize_t count = recvfrom(clientfd, buffer, sizeof(buffer), 0, (struct sockaddr *)&client_addr, &client_len);
           if (count > 0) {
         	  write(STDOUT_FILENO, buffer, count);
           } 
           else {
        	   break;
           }
      	}
     	close(clientfd);
        waitpid(pid, NULL, 0);
        exit(1);
     }
    

    
}


void shut_down(int signum){
    printf("shutting server down\n");
    unlink(sockpath);
    killpg(0, SIGTERM);
    exit(1);
}


void start_uds_stream_server(int* serverfd){
	
	struct sockaddr_un addr;
	*serverfd = socket(AF_UNIX, SOCK_STREAM, 0);
	if(*serverfd<0){
	    perror("uds stream error");
	    exit(1);
	}
	
	
	addr.sun_family = AF_UNIX;
    	strcpy(addr.sun_path, sockpath);
    	
    	int b = bind(*serverfd, (struct sockaddr*)&addr, sizeof(addr));
	if(b<0){
	    perror("bind");
	    exit(1);
	}
	
	if(listen(*serverfd, 3)<0){
	    perror("listen");
	    exit(1);
	}
	
	printf("listening on %s\n",sockpath);
	
	
	
}

void start_uds_stream_client(){
	
	int clientfd;
        struct sockaddr_un addr;

        
        clientfd = socket(AF_UNIX, SOCK_STREAM, 0);
	
	
	addr.sun_family = AF_UNIX;
	strcpy(addr.sun_path, sockpath);

	
	connect(clientfd, (struct sockaddr*)&addr, sizeof(addr));
	
	char buffer[1024];
	int count;
	
	while(1){
	    if (fgets(buffer, sizeof(buffer), stdin) != NULL) {
       		 send(clientfd, buffer, strlen(buffer), 0);
    	    }
    	    
	
	
	}
	
	close(clientfd);
	exit(1);
}

void start_uds_dgram_server(int* serverfd){
	
	
	struct sockaddr_un addr;
	*serverfd = socket(AF_UNIX, SOCK_DGRAM, 0);
	if(*serverfd<0){
		perror("socket faild");
		exit(1);
	}
	
	memset(&addr, 0, sizeof(addr));
	addr.sun_family = AF_UNIX;
	strcpy(addr.sun_path, sockpath);
	
	
	if (bind(*serverfd, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        	perror("bind error");
        	close(*serverfd);
        	exit(1);
    	}
    	
    	printf("server is bound to %s",sockpath);
    	


}


void start_uds_dgram_client(){
	struct sockaddr_un server_addr;
	
	int clientfd = socket(AF_UNIX, SOCK_DGRAM, 0);
	if(clientfd<0){
		perror("socket faild");
		exit(1);
	}
	
	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sun_family = AF_UNIX;
	strcpy(server_addr.sun_path, sockpath);
	
	
	char buffer[1024];
	int count;
	
	while(1){
	    if (fgets(buffer, sizeof(buffer), stdin) != NULL) {
       		 sendto(clientfd, buffer, strlen(buffer), 0,(struct sockaddr*)&server_addr, sizeof(server_addr));
    	    }
    	    
	
	
	}
	
	close(clientfd);
	exit(1);
	
	
}



int main(int argc,char* argv[]){
    int timeout = -1;
    int udpserverfd = -1;
    int serverfd = -1;
    int clientfd = -1;
    int listenerfd = -1;  //listens to the port output
    char *hostname = NULL;
    int port;
    
    signal(SIGINT, shut_down);
    signal(SIGTERM, shut_down);
    
    // handle alarm
    for (int i = 1; i < argc; i++) {
        if (!strcmp(argv[i], "-t") && (i + 1) < argc) {
            timeout = atoi(argv[i + 1]);
            i++; 
        }
    }

    if (timeout > 0) {
        signal(SIGALRM, shut_down);
        alarm(timeout);
    }
    
    
    //handle without e case
    if(strcmp(argv[1],"-e")){
    	port = atoi(argv[2]+4);
	for(int i=1;i<argc;i++){
	    if(!strncmp(argv[i],"tcps",4)){
		start_tcp_server(&serverfd,port);
		struct sockaddr_in client_addr;
		socklen_t client_len = sizeof(client_addr);
				 
		//open fd to send messages to the client 
		clientfd = accept(serverfd, (struct sockaddr *)&client_addr, &client_len);
		if (serverfd < 0) {
		    perror("accept failed");
		    exit(1);
		}
			 
		printf("Accepted input connection on port %d\n", port);
    		break;
    	    }
    	    
    	    else if(!strncmp(argv[i],"udps",4)){
    	    	 port = atoi(argv[i]+4);
		 start_udp_server(&udpserverfd,port);
		 printf("Listening on port %d\n", port);
    	    	 clientfd = udpserverfd;
    	    
    	    
    	    }
    	    
    	    
    	}
    	
    	without_e(clientfd,argv);
    }
    
    
    //handle q2 case
    if(argc==3 && !strncmp("ttt",argv[2],3)){
    	run_basic(argv);
    }
    
    
    //handle q3 and q4 and q6 (tcp, udp and uds servers and clients)
    if(argc>=3){
    	
	 for(int i=1;i<argc;i++){
	 	
	 	//in case: mync -e "ttt 123456789" -i tcps4050 || mync -e "ttt 123456789" -o tcps4050
	 	//create a server and listen on the port
	 	 if((!strcmp(argv[i],"-i") || !strcmp(argv[i],"-o")) && !strncmp(argv[i+1],"tcps",4)){
			//open server
			 
			port = atoi(argv[i+1]+4);
		    	            
			start_tcp_server(&serverfd,port);
			struct sockaddr_in client_addr;
			socklen_t client_len = sizeof(client_addr);
			 
			//open fd to send messages to the client 
			clientfd = accept(serverfd, (struct sockaddr *)&client_addr, &client_len);
			if (serverfd < 0) {
	         	    perror("accept failed");
		        exit(1);
			}
		 
			printf("Accepted input connection on port %d\n", port);
		 
		 }
		 
	
		 //in case: mync -e "ttt 123456789" -i tcps4050 -o tcpclocalhost,4455
		 //creat a listener client and connect him to hostname and port number of tcpc parameter
		 else if(!strcmp(argv[i],"-o") && !strncmp(argv[i+1],"tcpc",4) && argc>4){
		 	hostname = strtok(argv[i+1] + 4, ",");  //split the string where ,
		 	if(!strcmp(hostname,"localhost"))
		 	    hostname = "127.0.0.1";
		 	port = atoi(strtok(NULL,","));  //get what after the ,
		 	start_tcp_listener(&listenerfd, hostname, port);
		 }
		 
		 
		 //in case: mync -e "ttt 123456789" -i udps4050 || mync -e "ttt 123456789" -o udps4050
		 else if(!strcmp(argv[i],"-i") && !strncmp(argv[i+1],"udps",4)){
		 
		 	port = atoi(argv[i+1]+4);
		 	start_udp_server(&udpserverfd,port);
		 	printf("Listening on port %d\n", port);
		 
		 }
		 
		 //in case: mync -e -o udpc4050
		 else if(!strcmp(argv[i],"-o") && !strncmp(argv[i+1],"udpc",4)){
		 
		 	port = atoi(argv[i+1]+4);
		 	start_udp_client(port);

		 }
		 

		 // mync -e "ttt 123456789" -i udsss
		 else if((!strcmp(argv[i],"-i") || !strcmp(argv[i],"-o")) && !strncmp(argv[i+1],"udsss",4)){
			start_uds_stream_server(&serverfd);
		 	clientfd = accept(serverfd, NULL, NULL);
		 	
		 	run_i_o_b_tcp(clientfd,argv);
		 	
		 }
		 
		 //mync -e udssc
		 else if(!strncmp(argv[i],"udssc",4)){
		 	start_uds_stream_client();
		 	
		 }
		 
		 else if(!strncmp(argv[i],"udsds",4)){
		 	start_uds_dgram_server(&udpserverfd);
		 	
		 	run_i_o_b_udp(udpserverfd,argv);
		 
		 }
		 
		 else if(!strncmp(argv[i],"udsdc",4)){
		 	start_uds_dgram_client();
	
		 }
		 
		 
		 
		
		 
		 
		 
		 
         }
         //if tcp client connected
         if(clientfd!=-1){
             //if started the listener
             if(listenerfd!=-1){
             	run_i_and_o(clientfd,listenerfd,argv);
             }
             else{
                run_i_o_b_tcp(clientfd,argv);
             
             }
         }
         //if udp client connected
         else if(udpserverfd!=-1){
         	//if started the listener
             if(listenerfd!=-1){
             	run_i_and_o(udpserverfd,listenerfd,argv);
             }
             else{
                run_i_o_b_udp(udpserverfd,argv);
             	
             }
         
         }
         
         
         

    }
    
    
    
    return 0;
    
    
    
    	


}
