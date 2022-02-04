#include <cstdio>
#include <cstring>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <arpa/inet.h>
#include <sys/socket.h>


#define PORT 8080
#define LOCALHOST "127.0.0.1"
int main(int argc, char *argv[])
{
  /*
  Outline:
  Socket setup
  listen
  connect
  upon recieving input, 
  */
  //vars
  int sock, new_sock, ret;
  sockaddr_in addr;
  socklen_t socklen;
  int buflen;
  //socket setup
  sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

  //bind process
  addr.sin_family = AF_INET;
  addr.sin_port = htons(PORT);
  addr.sin_addr.s_addr = INADDR_ANY;
  ret = bind(sock, (sockaddr*)&addr, sizeof(addr));//0 if no error
  if(ret){
    //err
  }

  //listen
  ret = listen(sock, 0);//size 0 queue ie only communicating w/ 1 person

  //accept
  socklen = sizeof(addr);
  new_sock = accept(sock, (sockaddr*)&addr, &socklen);//og sock used to establish connection, new sock used for communication
  //socket setup complete  

  //send/recv
  ret = send(new_sock, "message", sizeof("message"), 0);//ret would be #of bytes sent
  ret = recv(new_sock, buf, sizeof(buf), 0);

  //close connection
  ret = close(new_sock);
  ret = close(sock);
  return 0;
}
