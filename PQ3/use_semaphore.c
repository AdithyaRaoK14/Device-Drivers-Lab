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

#define SCULL_QUANTUM_SIZE 4000
#define READ_BUF_SIZE      4001

void catch_ctrlc(int signo) {
	printf ("Got SIGINT (%d), closing driver.\n",signo);
	keep_spinning = 0;
        close(fd);
}

//cr s
/* Read and print all bytes of a quantum starting at given offset.
 * scull returns at most one quantum per read() call, so we loop. */
void print_quantum(int fd, off_t offset, int quantum_num)
{
        char read_buf[READ_BUF_SIZE];
        ssize_t n;
        long total = 0;
 
        lseek(fd, offset, SEEK_SET);
        printf("quantum[%d]: ", quantum_num);
        while (1) {
                memset(read_buf, 0, sizeof(read_buf));
                n = read(fd, read_buf, sizeof(read_buf) - 1);
                if (n <= 0)
                        break;
                read_buf[n] = '\0';
                printf("%s", read_buf);
                total += n;
                /* stop after reading one full quantum */
                if (total >= SCULL_QUANTUM_SIZE)
                        break;
        }
        printf("\n(total bytes read: %ld)\n", total);
}
//cr

int main ( ) 
{

        int i, k = 2000, l = 3000;
        int *p;
        char ch, write_buf[4000], read_buf[4000];
        ssize_t n;
        int nth; //cr
	
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
                        printf("--- Before swap ---\n");
                        print_quantum(fd, 0, 0);
                        print_quantum(fd, (off_t)SCULL_QUANTUM_SIZE, 1);
 
                        /* Invoke the swap ioctl */
                        if (ioctl(fd, SWAP_FIRST_TWO_PTRS) < 0) {
                                perror("ioctl SWAP_FIRST_TWO_PTRS failed");
                                break;
                        }
 
                        printf("--- After swap ---\n");
                        print_quantum(fd, 0, 0);
                        print_quantum(fd, (off_t)SCULL_QUANTUM_SIZE, 1);
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
