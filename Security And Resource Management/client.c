#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <netdb.h>
#include <netinet/in.h>
#include <openssl/bio.h>
#include <openssl/err.h>
#include <openssl/evp.h>
#include <openssl/md5.h>
#include <openssl/pem.h>
#include <openssl/rsa.h>
#include <openssl/ssl.h>
#include <openssl/engine.h>
#include <ctype.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <libgen.h>
#include "support.h"
#define privatekey "private.pem"
#define publickey "public.pem"

// client 2

/*
 * help() - Print a help message
 */
void help(char *progname) {
    printf("Usage: %s [OPTIONS]\n", progname);
    printf("Perform a PUT or a GET from a network file server\n");
    printf("  -P    PUT file indicated by parameter\n");
    printf("  -G    GET file indicated by parameter\n");
    printf("  -C    use checksums for PUT and GET\n");
    printf("  -e    use encryption, with public.pem and private.pem\n");
    printf("  -s    server info (IP or hostname)\n");
    printf("  -p    port on which to contact server\n");
    printf("  -S    for GETs, name to use when saving file locally\n");
}

/*
 * die() - print an error and exit the program
 */
void die(const char *msg1, char *msg2) {
    fprintf(stderr, "%s, %s\n", msg1, msg2);
    exit(0);
}

/*
 * connect_to_server() - open a connection to the server specified by the
 *                       parameters
 */
int connect_to_server(char *server, int port) {
    int clientfd;
    struct hostent *hp;
    struct sockaddr_in serveraddr;
    char errbuf[256];                                   /* for errors */

    /* create a socket */
    if ((clientfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
        die("Error creating socket: ", strerror(errno));

    /* Fill in the server's IP address and port */
    if ((hp = gethostbyname(server)) == NULL) {
        sprintf(errbuf, "%d", h_errno);
        die("DNS error: DNS error ", errbuf);
    }
    bzero((char *) &serveraddr, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    bcopy((char *)hp->h_addr_list[0],
          (char *)&serveraddr.sin_addr.s_addr, hp->h_length);
    serveraddr.sin_port = htons(port);

    /* connect */
    if (connect(clientfd, (struct sockaddr *) &serveraddr, sizeof(serveraddr)) < 0)
        die("Error connecting: ", strerror(errno));
    return clientfd;
}

/*
 * put_file() - send a file to the server accessible via the given socket fd
 */
void put_file(int fd, char *put_name, char * check_sum, int encrypt_flag) {

  /* Create Buffer To Store Max Size Of Transmission */
  const int MAXSIZE = 8192;
  char putbuf[MAXSIZE];
  bzero(putbuf, MAXSIZE);
  
  /* Open File */
  FILE *file = fopen(put_name, "r");
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

  /* Get File Size */
  struct stat fstat;
  char *statbuffer;
  statbuffer = (char *)malloc(sizeof(char *)*256);
  strcat(statbuffer,put_name);
  stat(statbuffer, &fstat);
  char byte_char[32];
  sprintf(byte_char, "%d\n", (intmax_t)fstat.st_size);
  free(statbuffer);

  /* Concatenate Strings */
  printf("%s", put);
  strcat(putbuf, put);
  if(check_sum != NULL){
    printf("%s", check_sum);
    strcat(putbuf, check_sum);
  }
  printf("%s", filename);
  strcat(putbuf, filename);
  printf("%s", byte_char);
  strcat(putbuf, byte_char);
  char line[1024];

  /* Before sending to the server, encrypt the file */

  /* open the public key -> public.pem */
  FILE *public_pem_file = fopen(publickey, "rb");
  
  /* read the contents of the public key and assign to char */
  fseek(public_pem_file, 0L, SEEK_END);
  long size = ftell(public_pem_file);
  char * public_buffer;
  rewind(public_pem_file);
  public_buffer = calloc(1, size+1);
  fread(public_buffer, size, 1, public_pem_file);
    
  BIO *pub_mem = BIO_new_mem_buf((void *)public_buffer, -1);
  BIO_set_flags(pub_mem, BIO_FLAGS_BASE64_NO_NL);

  //RSA *public_rsa = PEM_read_RSA_PUBKEY(public_pem_file, NULL, NULL, NULL);
  RSA *public_rsa = PEM_read_bio_RSA_PUBKEY(pub_mem, NULL, NULL, NULL);

  int * res;
  int pub_len = sizeof(public_rsa);
  int result;
  if (encrypt_flag == 1) {
    while(fgets(line, 1024, file)){
      unsigned char* x = (unsigned char *)malloc(pub_len);
      result = RSA_public_encrypt(sizeof(line), (unsigned char*)line, x, public_rsa, RSA_PKCS1_PADDING);
      strcat(putbuf, x);
      free(x);
    }
  } else {
    while(fgets(line, 1024, file)){strcat(putbuf, line);}  
  }

  fclose(file);
  fclose(public_pem_file);
  free(public_buffer);
  BIO_free(pub_mem);
  free(public_rsa);

  /* Send To Server */
  write(fd, putbuf, sizeof(putbuf));

  /* Read From Server */
  char ok[4];
  read(fd, ok, sizeof(ok));
  printf("%s\n", ok);
}

void putc_file(int fd, char *put_name, int encrypt_flag){

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
  put_file(fd, put_name, putbuf, encrypt_flag);
  
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

int check_put(char *bu, char * save_name, int check_flag, int encrypt_flag){
 
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
  i++; printf(" From Server\n");

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
  
  printf("RECIEVED: %s\n", filename);
  if(save_name != NULL)
    printf("SAVED AS: %s\n", save_name);
  else
    printf("SAVED AS: %s\n", filename);

  /* Print Bytes */
  while((bu[i] != '\n')){ printf("%c", bu[i]); i++; }
  i++; printf("\n");

  /* Write File */
  FILE *fp;
  if(save_name != NULL){
    fp = fopen(save_name, "w");
  }else{
    fp = fopen(filename, "w");
  }

  FILE *private_pem_file = fopen(privatekey, "rb");

  /* read the contents of the private key and assign to char */
  fseek(private_pem_file, 0L, SEEK_END);
  long size = ftell(private_pem_file);
  char * private_buffer;
  rewind(private_pem_file);
  private_buffer = calloc(1, size+1);
  fread(private_buffer, size, 1, private_pem_file);

  BIO *pri_mem = BIO_new_mem_buf((void *)private_buffer, -1);
  BIO_set_flags(pri_mem, BIO_FLAGS_BASE64_NO_NL);

  RSA *private_rsa = PEM_read_bio_RSAPrivateKey(pri_mem, NULL, NULL, NULL);

  int * res;
  int pri_len = sizeof(private_rsa);
  int result;
  int len;
  unsigned char* x = (unsigned char *)malloc(pri_len);
  
  if (encrypt_flag == 0) {
    while(i < bulen){ fputc(bu[i],fp); i++;}
  } else {
    while(i < bulen) {
      //result = RSA_private_decrypt(bulen, bu[i], x, private_rsa, RSA_PKCS1_PADDING);
      fputc(bu[i], fp); 
      i++;
    }
  }
  fclose(fp);
  fclose(private_pem_file);
  free(private_buffer);
  BIO_free(pri_mem);
  free(private_rsa);

  if(check_flag == 1){
    int check = 0;
    if(save_name == NULL){
      check = check_hash(filename, clientCheckSum);
    }else{
      check = check_hash(save_name, clientCheckSum);
    }
    if(check)
      return 1;
    else
      return 0;
  }else{
    clientCheckSum[0] = '\0';
  }

  return 1;

}
/*
 * get_file() - get a file from the server accessible via the given socket
 *              fd, and save it according to the save_name
 */
void get_file(int fd, char *get_name, char *save_name, int check_flag, int encrypt_flag) {
    /* TODO: implement a proper solution, instead of calling the echo() client */
  //    echo_client(fd);
  char getbuffer[8192];
  char recvbuffer[8192];
  bzero(getbuffer, 8192);
  bzero(recvbuffer, 8192);
  if(check_flag == 0)
    strcat(getbuffer, "GET\n");
  else
    strcat(getbuffer, "GETC\n");
  strcat(getbuffer, get_name);

  write(fd, getbuffer, sizeof(getbuffer));
  read(fd, recvbuffer, sizeof(recvbuffer));
  if(check_put(recvbuffer, save_name, check_flag, encrypt_flag) == 1){
    printf("GOT IT");
  }else{
    printf("DONT GOT IT");
  }

}


/*
 * main() - parse command line, open a socket, transfer a file
 */
int main(int argc, char **argv) {
    /* for getopt */
    long  opt;
    char *server = NULL;
    char *put_name = NULL;
    char *get_name = NULL;
    int   port;
    char *save_name = NULL;
    int check_sum_flag = 0;
    int encrypt_flag = 0;
    check_team(argv[0]);

    /* parse the command-line options. */
    /* TODO: add additional opt flags */
    while ((opt = getopt(argc, argv, "hs:P:G:S:p:Ce")) != -1) {
        switch(opt) {
          case 'h': help(argv[0]); break;
          case 's': server = optarg; break;
          case 'P': put_name = optarg; break;
          case 'G': get_name = optarg; break;
          case 'S': save_name = optarg; break;
          case 'p': port = atoi(optarg); break;
	  case 'C': check_sum_flag = 1; break;
          case 'e': encrypt_flag = 1; break;
        }
    }

    /* open a connection to the server */
    int fd = connect_to_server(server, port);

    /* put or get, as appropriate */
    if (put_name != NULL && check_sum_flag == 0)
      put_file(fd, put_name, NULL, encrypt_flag);
    else if(get_name != NULL && check_sum_flag == 0)
      get_file(fd, get_name, save_name, 0, encrypt_flag);
    else if(put_name != NULL && check_sum_flag == 1)
      putc_file(fd, put_name, encrypt_flag);
    else if(get_name != NULL && check_sum_flag == 1)
      get_file(fd, get_name, save_name, 1, encrypt_flag);
    /* close the socket */
    int rc;
    if ((rc = close(fd)) < 0)
        die("Close error: ", strerror(errno));
    exit(0);
}
