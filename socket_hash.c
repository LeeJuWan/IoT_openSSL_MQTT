#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <fcntl.h>

#define BUFSIZE 10000
void error_handling( char *message ){
        fputs(message, stderr);
        fputc('\n', stderr);
        exit(1);
}


int main(int argc, char *argv[]){
	int len = 0;
	char buf[BUFSIZE]={0,};
	char *esp="esp8266의 ip번호";
	char *esp_port="esp8266의 포트번호";
        int serv_sd;
	int hash=0;
        struct sockaddr_in serv_adr, clnt_adr;
        socklen_t clnt_adr_sz;


//server.crt의 fingerprint값이 들어있는 파일 경로 입력
	hash = open("/home/roo/shell/hash_file",O_RDONLY);
        
	serv_sd = socket(PF_INET, SOCK_STREAM, 0);

        if (serv_sd == -1)
          error_handling("socket() error");

        memset(&serv_adr, 0, sizeof(serv_adr));
        serv_adr.sin_family = AF_INET;
        serv_adr.sin_addr.s_addr = inet_addr("esp8266의 ip번호");
        serv_adr.sin_port = htons(esp8266의 포트번호);

	 if(connect(serv_sd,(struct sockaddr *)&serv_adr,sizeof(serv_adr))){
	    error_handling("connect() error");	
	 }else{ printf("connect() success\n");}
       
	
	printf("hash sending");	
        sleep(10); //소켓연결시간 보유
        while((len = read(hash,buf,sizeof(buf))) != 0)
        {
          printf(".");
	  write(serv_sd,buf,len);

        }
	printf("\n3sending success!\n");

       
	close(hash);
        close(serv_sd);
        return 0;
}
