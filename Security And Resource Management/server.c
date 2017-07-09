#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <netdb.h>
#include <netinet/in.h>
#include <openssl/md5.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include "support.h"


/*
 * help() - Print a help message
 */
void help(char *progname) {
    printf("Usage: %s [OPTIONS]\n", progname);
    printf("Initiate a network file server\n");
    printf("  -l    number of entries in cache\n");
    printf("  -p    port on which to listen for connections\n");
}

/*
 * die() - print an error and exit the program
 */
void die(const char *msg1, char *msg2) {
    fprintf(stderr, "%s, %s\n", msg1, msg2);
    exit(0);
}

/*
 * open_server_socket() - Open a listening socket and return its file
 *                        descriptor, or terminate the program
 */
int open_server_socket(int port) {
    int                listenfd;    /* the server's listening file descriptor */
    struct sockaddr_in addrs;       /* describes which clients we'll accept */
    int                optval = 1;  /* for configuring the socket */

    /* Create a socket descriptor */
    if ((listenfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
        die("Error creating socket: ", strerror(errno));

    /* Eliminates "Address already in use" error from bind. */
    if (setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR,
                   (const void *)&optval , sizeof(int)) < 0)
        die("Error configuring socket: ", strerror(errno));

    /* Listenfd will be an endpoint for all requests to the port from any IP
       address */
    bzero((char *) &addrs, sizeof(addrs));
    addrs.sin_family = AF_INET;
    addrs.sin_addr.s_addr = htonl(INADDR_ANY);
    addrs.sin_port = htons((unsigned short)port);
    if (bind(listenfd, (struct sockaddr *)&addrs, sizeof(addrs)) < 0)
        die("Error in bind(): ", strerror(errno));

    /* Make it a listening socket ready to accept connection requests */
    if (listen(listenfd, 1024) < 0)  // backlog of 1024
        die("Error in listen(): ", strerror(errno));

    return listenfd;
}

/*
 * handle_requests() - given a listening file descriptor, continually wait
 *                     for a request to come in, and when it arrives, pass it
 *                     to service_function.  Note that this is not a
 *                     multi-threaded server.
 */
void handle_requests(int listenfd, void (*service_function)(int, int), int param) {
    while (1) {
        /* block until we get a connection */
        struct sockaddr_in clientaddr;
        int clientlen = sizeof(clientaddr);
        int connfd;
        if ((connfd = accept(listenfd, (struct sockaddr *)&clientaddr, &clientlen)) < 0)
            die("Error in accept(): ", strerror(errno));

        /* print some info about the connection */
        struct hostent *hp = gethostbyaddr((const char *)&clientaddr.sin_addr.s_addr,
                           sizeof(clientaddr.sin_addr.s_addr), AF_INET);
        if (hp == NULL) {
            fprintf(stderr, "DNS error in gethostbyaddr() %d\n", h_errno);
            exit(0);
        }
        char *haddrp = inet_ntoa(clientaddr.sin_addr);
        printf("server connected to %s (%s)\n", hp->h_name, haddrp);

        /* serve requests */
        service_function(connfd, param);

        /* clean up, await new connection */
        if (close(connfd) < 0)
            die("Error in close(): ", strerror(errno));
    }
}

void put_file(int fd, char *put_name, char * check_sum) {

  /* Create Buffer To Store Max Size Of Transmission */
  const int MAXSIZE = 8192;
  char putbuf[MAXSIZE];
  bzero(putbuf, MAXSIZE);

  /* Open File */
  FILE *file = fopen(put_name, "r");

  /* Create Put Message*/
  char put[6];
  put[0] = '\0';

  /* Create Put Message*/
  if(check_sum != NULL){
    strcpy(put,"PUTC\n");
  }else{
    strcpy(put, "PUT\n");
  }

  /* Get File Name And Append Newline*/
  char *bname = basename(put_name);
  char filename[strlen(bname) + 2];
  strcpy(filename, bname);
  strcat(filename, "\n");
  printf("%s", filename);

  /* Get File Size */
  struct stat fstat;
  char *statbuffer;
  statbuffer = (char *)malloc(sizeof(char *)*256);
  strcat(statbuffer,put_name);
  stat(statbuffer, &fstat);
  char byte_char[32];
  sprintf(byte_char, "%d\n", (intmax_t)fstat.st_size);
  printf("%s", byte_char);
  free(statbuffer);

  /* Concatenate Strings */
  strcat(putbuf, put);
  if(check_sum != NULL){
    printf("%s", check_sum);
    strcat(putbuf, check_sum);
  }
  strcat(putbuf, filename);
  strcat(putbuf, byte_char);
  char line[1024];
  while(fgets(line, 1024, file)){strcat(putbuf, line);}
  fclose(file);
  /* Send To Server */
  write(fd, putbuf, sizeof(putbuf));

  /* Read From Server */
  char ok[4];
  read(fd, ok, sizeof(ok));
  printf("%s\n", ok);
}

void putc_file(int fd, char *put_name){

  /* Create Buffer To Store Max Size Of Transmission */
  const int LINESIZE = 1024;
  char putbuf[LINESIZE];
  bzero(putbuf, LINESIZE);

  /* Open File To Read Bytes*/
  FILE *file = fopen(put_name, "rb");

  /* Initialize MD5 */
  unsigned char digest[MD5_DIGEST_LENGTH];
  MD5_CTX  md5c;
  MD5_Init(&md5c);

  /* Create CheckSum */
  int bytes;
  unsigned char line[LINESIZE];
  while((bytes = fread(line, 1, LINESIZE, file)) != 0)
    MD5_Update(&md5c, line, bytes);

  MD5_Final(digest, &md5c);

  /* Concatenate CheckSum */
  char checksum[LINESIZE];
  int i;
  for(i=0; i < MD5_DIGEST_LENGTH; i++){
    sprintf(checksum, "%02x", digest[i]);
    strcat(putbuf, checksum);
  }
  strcat(putbuf,"\n");
  fclose(file);
  put_file(fd, put_name, putbuf);

}

int check_hash( char * put_name, char * check_sum){
  /* Create Buffer To Store Max Size Of Transmission */
  const int LINESIZE = 1024;
  char putbuf[LINESIZE];
  bzero(putbuf, LINESIZE);

  /* Open File To Read Bytes*/
  FILE *file = fopen(put_name, "rb");

  /* Initialize MD5 */
  unsigned char digest[MD5_DIGEST_LENGTH];
  MD5_CTX  md5c;
  MD5_Init(&md5c);

  /* Create CheckSum */
  int bytes;
  unsigned char line[LINESIZE];
  while((bytes = fread(line, 1, LINESIZE, file)) != 0)
    MD5_Update(&md5c, line, bytes);

  MD5_Final(digest, &md5c);

  /* Concatenate CheckSum */
  char checksum[LINESIZE];
  int i;
  for(i=0; i < MD5_DIGEST_LENGTH; i++){
    sprintf(checksum, "%02x", digest[i]);
    strcat(putbuf, checksum);
  }
  fclose(file);
  printf("Client Check Sum: \n\t%s\n", check_sum);
  printf("Server Check Sum: \n\t%s\n", putbuf);
  if(strcmp(putbuf, check_sum) == 0)
    return 1;
  else
    return 0;
  
}

int check_put(char *bu, int check_flag){
  
  int success = 0;
  char filename[100];
  char clientCheckSum[1024];
  int namecount = 0;
  int checksumcount = 0;
  int i = 0;
  int bulen = strlen(bu);


  /* Print PUT */
  while((bu[i] != '\n')){ 
    printf("%c", bu[i]); 
    i++;
  }
  i++; printf("\n");

  /* For PUTC Store Check SUM*/
  if(check_flag == 1){
    while(bu[i] != '\n'){
      clientCheckSum[checksumcount] = bu[i];
      i++; checksumcount++;
    }
    i++;
    clientCheckSum[checksumcount] = '\0';
  }
  
  /* Print And Store File Name */
  while(bu[i] != '\n'){ 
    printf("%c", bu[i]);
    filename[namecount] = bu[i];
    i++; namecount++;
  }
  i++;
  filename[namecount] = '\0';
  printf("\n");
  
  /* Print Bytes */
  while((bu[i] != '\n')){ printf("%c", bu[i]); i++; }
  i++; printf("\n");

  /* Write File */
  FILE *fp = fopen(filename, "w+"); 
  while(i < bulen){ fputc(bu[i],fp); i++;}
  fclose(fp);

  if(check_flag == 1){
    if(check_hash(filename, clientCheckSum))
      return 1;
    else
      return 0;
  }else{
    clientCheckSum[0] = '\0';
  }
  return 1;
}

int check_get(int connfd, char *bu, int checksum_flag){

  const char s[2] = "\n";
  char *token;

  int tcount = 1;
  token = strtok(bu, s);
  tcount += 1;
  printf("%s\n",token);

  /* File Name */
  token = strtok(NULL, s);
  tcount += 1;
  //  printf("%s\n",token);
  if(checksum_flag == 0)
    put_file(connfd, token, NULL);
  else
    putc_file(connfd, token);
  return 1;
}

/*
 * file_server() - Read a request from a socket, satisfy the request, and
 *                 then close the connection.
 */
void file_server(int connfd, int lru_size) {
  /* TODO: set up a few static variables here to manage the LRU cache of
     files */

  /* TODO: replace following sample code with code that satisfies the
     requirements of the assignment */

  const int MAX = 8192;
  char bu[MAX];
  bzero(bu, MAX);
  size_t n;
  n = read(connfd, bu, MAX);
  
  if(bu[0] == 'P' && bu[1] == 'U' && bu[2]== 'T' && bu[3] == '\n'){
    // check the lru cache

    if(check_put(bu,0)) {
      write(connfd, "OK", sizeof("OK"));
    } else
      write(connfd, "!OK", sizeof("!OK"));
  }else if(bu[0] == 'G' && bu[1] == 'E' && bu[2] == 'T' && bu[3] == '\n'){
    check_get(connfd,bu, 0);
  }else if(bu[0] == 'P' && bu[1] == 'U' && bu[2]== 'T' && bu[3] == 'C' && bu[4] == '\n'){
    if(check_put(bu, 1))
      write(connfd, "OK",sizeof("OK"));
    else
      write(connfd, "!OK", sizeof("!OK"));
  }else if (bu[0] == 'G' && bu[1] == 'E' && bu[2] == 'T' && bu[3] == 'C' && bu[4] == '\n'){
    check_get(connfd,bu,1);
  }
}
/*
 * main() - parse command line, create a socket, handle requests
 */
int main(int argc, char **argv) {
    /* for getopt */
    long opt;
    /* NB: the "part 3" behavior only should happen when lru_size > 0 */
    int  lru_size = 0;
    int  port     = 9000;

    check_team(argv[0]);

    /* parse the command-line options.  They are 'p' for port number,  */
    /* and 'l' for lru cache size.  'h' is also supported. */
    while ((opt = getopt(argc, argv, "hl:p:")) != -1) {
        switch(opt) {
          case 'h': help(argv[0]); break;
          case 'l': lru_size = atoi(argv[0]); break;
          case 'p': port = atoi(optarg); break;
        }
    }

    /* open a socket, and start handling requests */
    int fd = open_server_socket(port);
    handle_requests(fd, file_server, lru_size);

    exit(0);
}
