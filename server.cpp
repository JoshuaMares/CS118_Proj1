#include <cstdio>
#include <cstring>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <csignal>
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>
#include <dirent.h>
#include <ctype.h>

#define PORT 8080
#define LOCALHOST "127.0.0.1"
#define PATH_MAX 4096
#define ERR_404 "404.html"
#define FN_MAX 256

extern int errno;
int sock, new_sock;

//CTRL+C HANDLER
void catch_fun(int signal_message){
  if(signal_message == SIGINT){
    fprintf(stderr, "CTRL+C\n");
    close(new_sock);
    close(sock);
    exit(1);
  }
}

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
  int ret;
  sockaddr_in addr;
  socklen_t socklen;
  int errnum;

  signal(SIGINT, catch_fun);

  //socket setup
  sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

  //bind process
  addr.sin_family = AF_INET;
  addr.sin_port = htons(PORT);
  addr.sin_addr.s_addr = INADDR_ANY;
  if((ret = bind(sock, (sockaddr*)&addr, sizeof(addr)))){//0 if no error
    fprintf(stderr, "bind error\n");
    close(sock);
    exit(1);
  }

  //listen
  if((ret = listen(sock, 0))){//size 0 queue ie only communicating w/ 1 person
    fprintf(stderr, "listen error\n");
    close(sock);
    exit(1);
  }

  while(1){
    //accept
    socklen = sizeof(addr);
    new_sock = accept(sock, (sockaddr*)&addr, &socklen);//og sock used to establish connection, new sock used for communication
    //socket setup complete

    //recv/send
    char buf[PATH_MAX];
    if((ret = recv(new_sock, buf, PATH_MAX, 0)) < 0){
      fprintf(stderr, "recv error\n");
      close(new_sock);
      close(sock);
      exit(1);
    }
    //char* end_of_header = strstr(buf, eoh);
    //memcpy(header, buf, end_of_header - buf + 4);
    printf("%s\n", buf);

    //parse
    char file_name[FN_MAX];
    //file name from 1st space to last, http type after last space
    char* eo_header_req = strstr(buf, "\r\n");
    char header_req[128];
    memcpy(header_req, buf, eo_header_req -buf);
    int first_space;
    int last_space;
    for(int i = 0; strlen(header_req) ; i++){
      if(header_req[i] == ' '){
        first_space = i + 1;//+1 to get rid of the /
        break;
      }
    }
    for(int i = strlen(header_req) - 1; i >= 0; i--){
      if(header_req[i] == ' '){
        last_space = i;
        break;
      }
    }
    int file_name_length = last_space - first_space - 1;
    //printf("file name length = %i\n", file_name_length);
    memset(file_name, 0, FN_MAX);
    memcpy(file_name, header_req + first_space + 1, file_name_length);
    //printf("header_req:%s\n", header_req);
    //printf("filename1:%s\n", file_name);
    file_name[file_name_length] = '\0';
    //printf("filename2:%s\n", file_name);
    char file_name_fixed[FN_MAX];
    int fn_index = 0;
    int fnf_index = 0;
    //remove %20 and replace w/ space
    while(fn_index < file_name_length){
      if(file_name[fn_index] == '%' && file_name[fn_index] == '2' && file_name[fn_index] == '0'){
        file_name_fixed[fnf_index] = ' ';
        fn_index+=3;
        fnf_index++;
      }else{
        file_name_fixed[fnf_index] = file_name[fn_index];
        fn_index++;
        fnf_index++;
      }
    }
    file_name_fixed[fnf_index] = '\0';
    memcpy(file_name, file_name_fixed, FN_MAX);
    //printf("filename3:%s\n", file_name);
    char content_type[128];
    char content_type_msg[128];
    memset(content_type, 0, 128);
    memset(content_type_msg, 0, 128);
    char* dot_loc = strstr(file_name, ".");
    if(dot_loc != NULL){
      memcpy(content_type, dot_loc+1, strlen(dot_loc+1));
      //html, htm, txt, jpg, jpeg, png, gif
      if(!strcmp(content_type, "html")){
        memcpy(content_type_msg, "Content-type: text/html\r\n", sizeof("Content-type: text/html\r\n"));
      }else if(!strcmp(content_type, "htm")){
        memcpy(content_type_msg, "Content-type: text/html\r\n", sizeof("Content-type: text/html\r\n"));
      }else if(!strcmp(content_type, "txt")){
        memcpy(content_type_msg, "Content-type: text/plain\r\n", sizeof("Content-type: text/plain\r\n"));
      }else if(!strcmp(content_type, "jpg")){
        memcpy(content_type_msg, "Content-type: image/jpeg\r\n", sizeof("Content-type: image/jpeg\r\n"));
      }else if(!strcmp(content_type, "jpeg")){
        memcpy(content_type_msg, "Content-type: image/jpeg\r\n", sizeof("Content-type: image/jpeg\r\n"));
      }else if(!strcmp(content_type, "png")){
        memcpy(content_type_msg, "Content-type: image/png\r\n", sizeof("Content-type: image/png\r\n"));
      }else if(!strcmp(content_type, "gif")){
        memcpy(content_type_msg, "Content-type: image/gif\r\n", sizeof("Content-type: image/gif\r\n"));
      }else{
        //404
        memcpy(content_type_msg, "Content-type: text/html\r\n", sizeof("Content-type: text/html\r\n"));
      }
    }
    char file_name_lc[FN_MAX];
    char found_file[FN_MAX];
    for(int i = 0; i < strlen(file_name); i++){
      file_name_lc[i] = tolower(file_name[i]);
    }
    if((dot_loc = strstr(file_name_lc, "."))){
      dot_loc[0] = '\0';
    }
    DIR *d;
    struct dirent *dir;
    d = opendir(".");
    if (d) {
      while ((dir = readdir(d)) != NULL) {
        //turn file name and dir->d_name into all lowercase
        //compare
        memcpy(found_file, dir->d_name, strlen(dir->d_name));
        for(int i = 0; i < strlen(found_file); i++){
          found_file[i] = tolower(found_file[i]);
        }
        if((dot_loc = strstr(found_file, "."))){
          dot_loc[0] = '\0';
        }
        if(!strcmp(file_name_lc, found_file)){
          memcpy(file_name, dir->d_name, strlen(dir->d_name));
          break;
        }
      }
      closedir(d);
    }
    //build response
    unsigned char file_buf[PATH_MAX];
    char file_ctime[100];
    char header_response[128];
    char last_modified_header[128];
    char content_length_header[128];
    char status_cm[128];
    memset(header_response, 0, 128);
    memset(last_modified_header, 0, 128);
    memset(content_length_header, 0, 128);
    memset(status_cm, 0, 128);
    //get file stats
    //printf("opening %s\n", file_name);
    int file_fd = open(file_name, O_RDONLY);
    memcpy(status_cm, "200 OK\r\n", sizeof("200 OK\r\n"));
    if(file_fd < 0){
      printf("error opening proper file\n");
      errnum = errno;
      fprintf(stderr, "errno: %i\nerrmsg: %s\n", errno, strerror(errnum));
      file_fd = open("404.html", O_RDONLY);
      memcpy(status_cm, "404 Not Found\r\n", sizeof("404 Not Found\r\n"));
    }
    int bytes_read;
    struct stat fd_stat;
    fstat(file_fd, &fd_stat);
    //last access time
    struct tm* file_tm;
    file_tm = gmtime(&(fd_stat.st_ctime));
    strftime(file_ctime, sizeof(file_ctime), "%a,%e %b %G %T GMT\r\n", file_tm);
    //headers
    sprintf(last_modified_header, "Last-Modified: %s\r\n", file_ctime);
    sprintf(content_length_header, "Content-Length: %ld\r\n", fd_stat.st_size);
    //put headers together
    char header[PATH_MAX];
    memset(header, 0, PATH_MAX);
    //status (depends on req)
    strcat(header, "HTTP/1.0 ");
    strcat(header, status_cm);
    strcat(header, "Sever: 005154394\r\n");
    strcat(header, file_ctime);
    strcat(header, content_type_msg);
    strcat(header, content_length_header);
    strcat(header, last_modified_header);
    //strcat(header, "\r\n");

    //send response (just responding to get)
    //printf("about to send\n");
    //sending header
    if((ret = send(new_sock, header, strlen(header), 0)) < 0){
      fprintf(stderr, "send error\n");
      close(new_sock);
      close(sock);
      exit(1);
    }
    //sending response
    if(fd_stat.st_size > PATH_MAX){
      //printf("filesize:%li\nmultiple messages needed\n", fd_stat.st_size);
      int bytes_left = fd_stat.st_size;
      int iteration = 0;
      while(bytes_left > 0){
        memset(file_buf, 0, PATH_MAX);
        //printf("iteration:%i\nbytesleft%i\n", iteration, bytes_left);
        if((bytes_read = read(file_fd, file_buf, PATH_MAX)) < 0){
          fprintf(stderr, "read err\n");
          close(new_sock);
          close(sock);
          exit(1);
        }
        //printf("bytesread:%i\n", bytes_read);
        if((ret = send(new_sock, file_buf, bytes_read, 0)) < 0){
          fprintf(stderr, "send error\n");
          close(new_sock);
          close(sock);
          exit(1);
        }
        //printf("bytessent:%i\n", ret);
        if(bytes_read != ret){
          //printf("bytes sent not equal to bytes read\n");
        }
        bytes_left = bytes_left - bytes_read;
        iteration++;
      }
    }else{//just read and send
      //printf("file small enough to send in one msg\n");
      if((bytes_read = read(file_fd, file_buf, sizeof(file_buf))) < 0){
        fprintf(stderr, "read err\n");
        close(new_sock);
        close(sock);
        exit(1);
      }
      if((ret = send(new_sock, file_buf, bytes_read, 0)) < 0){
        fprintf(stderr, "send error\n");
        close(new_sock);
        close(sock);
        exit(1);
      }
    }
    //printf("sent\n");

    close(new_sock);
    //ret = send(new_sock, "message", sizeof("message"), 0);//ret would be #of bytes sent
    //ret = recv(new_sock, buf, sizeof(buf), 0);
  }

  //close connection
  close(new_sock);
  close(sock);
  return 0;
}
