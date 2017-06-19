#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#define PORT 9876
int main(){
  char destination[32];
  int dstSocket;
  struct sockaddr_in dstAddr;
  struct hostent *hp;
  char   buf[1024];
  int    numrcv;
  printf("サーバーマシンのIPは？:");
  scanf("%s", destination);
  bzero((char *)&dstAddr, sizeof(dstAddr));
  dstAddr.sin_family = AF_INET;
  dstAddr.sin_port = htons(PORT);
  hp = gethostbyname(destination);
  bcopy(hp->h_addr, &dstAddr.sin_addr, hp->h_length);
  dstSocket = socket(AF_INET, SOCK_STREAM, 0);
  if (connect(dstSocket, (struct sockaddr *)&dstAddr, sizeof(dstAddr)) < 0){
    printf("%s に接続できませんでした\n",destination);
    return(-1);
  }
  printf("%s に接続しました\n",destination);
  printf("適当なアルファベットを入力してください\n");
  
  while (1){
    scanf("%s",buf);
    write(dstSocket, buf, 1024);
    numrcv = read(dstSocket, buf, 1024);
    printf("%s\n\n",buf);
  }
  close(dstSocket);
  return(0);
}
