/* application to invoke and KEEP OPEN the device driver module, kernel_semaphore, and read the device file randomly*/  
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/ioctl.h> //_io(), _iow(), _ior() 
#include "sc_usr.h"
#include <string.h>

int fd;
int keep_spinning = 1;

void catch_ctrlc(int signo) {
	printf ("Got SIGINT (%d), closing driver.\n",signo);
	keep_spinning = 0;
        close(fd);
}
int main ( ) 
{

        int i, k = 2000, l = 3000;
        int *p;
        char ch, write_buf[80], read_buf[80];
        ssize_t n;
	
        signal(2, catch_ctrlc);

        fd = open("/dev/scull0", O_RDWR);

        if (fd == -1)
        {
                printf("Error in opening file \n");
                exit(-1);
        }
        printf ("What do you want? PRESS r for reading or w for write or i for ioctl command: ");
        scanf ("%c", &ch);  /* First time, should choose writing */


        switch (ch) {
                case 'w': 
                        printf (" Enter the message to device: ");
                        scanf (" %[^\n]", write_buf);
                        write(fd,write_buf, strlen(write_buf));
                        break;
                case 'r':
                        lseek(fd, 0L, SEEK_SET);
                        read(fd, read_buf, sizeof(read_buf));
                        n = read(fd, read_buf, sizeof(read_buf) - 1);
                        if (n > 0) read_buf[n] = '\0';   /* guarantee null termination */
        	        printf ("The message from the device is : %s\n", read_buf);
			break; 	
                //cr s
                case 'i':
                        if (ioctl(fd, EMPTY_DEVICE) < 0) {
                                perror("ioctl EMPTY_DEVICE failed");
                                break;
                        }
                        printf("Device trimmed (emptied) successfully.\n");
                        break;
                //cr
                default:
                        printf(" press either r or w or i \n");
                        break;
        }
	while (keep_spinning)
		;
	
	
  //      close(fd);


}
