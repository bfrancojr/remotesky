/**
 * Camera Daemon: keeps a connection opened to a local
 * usb camera and allows clients to request photo capture
 * via a tcp/ip. 
 *
 * The protocol this daemon use to talk with the camera 
 * is handled by libphoto2.
 *
 * The photo capture is triggered by connecting on the tcp/ip
 * port that the daemon is listening. After the photo is taken,
 * the filename of the original photo and a thumbnail are returned.
 *
 * $ telnet localhost 5555
 * photo_1234.jpg
 * thumb_1234.jpg
 *
 * Initiate the daemon passing listening port and photos directory:
 * $ nohup camerad 5555 /tmp/photos/ &
 *
 * Reason to have a daemon instead of just calling gphoto2 cli: 
 * RPi has several issues of releasing the usb port after gphoto2
 * cli is called. Even using usb reset scripts doesn't seem to be
 * a solid solution. This daemon keeps the usb port connected avoid
 * those issues.
 */ 
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <time.h> 
#include <signal.h>
#include <linux/limits.h>
#include "camerad_photo.h"

#define OUT_BUF_LEN PATH_MAX*2

camerad_photo_conn conn;

void sig_handler(int signo) {
  if (signo == SIGINT) {
    fprintf(stderr, "received SIGINT. closing camera.\n");
    camerad_photo_close(&conn);
    exit(0);
  }
}

int main(int argc, char *argv[]) {
  int listenfd = 0, connfd = 0;
  struct sockaddr_in serv_addr; 
  char photo_path[PATH_MAX];
  char thumb_path[PATH_MAX];
  char out_buf[OUT_BUF_LEN];

  if (argc != 3) {
    fprintf(stderr, "%s [port] [photo dir]\n", argv[0]);
    exit(-1);
  }
  
  int port = atoi(argv[1]);
  char *photo_dir = argv[2];

  listenfd = socket(AF_INET, SOCK_STREAM, 0);
  memset(&serv_addr, '0', sizeof(serv_addr));

  serv_addr.sin_family = AF_INET;
  serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
  serv_addr.sin_port = htons(port); 

  bind(
    listenfd, 
    (struct sockaddr*) &serv_addr, 
    sizeof(serv_addr)
  ); 

  listen(listenfd, 10); 

  if (camerad_photo_connect(&conn, photo_dir) != 0) {
    return -1;
  }

  if (signal(SIGINT, sig_handler) == SIG_ERR) {
    fprintf(stderr, "can't catch SIGINT\n");
    return -1;
  }

  while(1) {
    connfd = accept(listenfd, (struct sockaddr*)NULL, NULL); 

    camerad_photo_capture(&conn, photo_path, thumb_path);

    snprintf(out_buf, OUT_BUF_LEN, "%s\n", photo_path);
    write(connfd, out_buf, strlen(out_buf)); 

    snprintf(out_buf, OUT_BUF_LEN, "%s\n", thumb_path);
    write(connfd, out_buf, strlen(out_buf)); 

    close(connfd);
    sleep(1);
  }
}
