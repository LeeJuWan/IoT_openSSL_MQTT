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
        char *esp="esp8266 ip번호";
        char *esp_port="esp8266 포트번호";
	      int serv_sd;
       	int client_key=0;

        
        struct sockaddr_in serv_adr, clnt_adr;
        socklen_t clnt_adr_sz;

  // client.key의 변환된 der파일이 있는 경로 입력
	client_key = open("/home/roo/shell/espkey.der", O_RDONLY);

	serv_sd = socket(PF_INET, SOCK_STREAM, 0);

        if (serv_sd == -1)
          error_handling("socket() error");

        memset(&serv_adr, 0, sizeof(serv_adr));
        serv_adr.sin_family = AF_INET;
        serv_adr.sin_addr.s_addr = inet_addr("esp8266 ip번호");
        serv_adr.sin_port = htons(esp8266 포트번호);

         if(connect(serv_sd,(struct sockaddr *)&serv_adr,sizeof(serv_adr))){
            error_handling("connect() error");
         }else{ printf("connect() success\n");}

        printf("espkey.der sending");
        sleep(10); //소켓연결시간을 보유

	while((len = read(client_key,buf,sizeof(buf))) != 0)
        {
           printf(".");
           write(serv_sd,buf,len);
        }
        printf("\n2sending success!\n");
	
	close(client_key);
 	close(serv_sd);
        return 0;
}
