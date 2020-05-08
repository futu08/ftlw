#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/ioctl.h>

#define DEVICE "/dev/fliocontrol"

int main(){
	int i, fd;
	char ch, write_buf1[100], read_buf1[100], write_buf2[100], read_buf2[100];
	fd = open(DEVICE, _IORW);

	if(fd == -1){
		printf("file %s either does not exist or has been locked by another process\n",DEVICE);
		exit(-1);

	}
	printf("1(r)=read from device1\n2(w) = write to device1\n3(r)=read from device2\n4(w) = write to device2\nenter command");
	scanf("%d", &i);
	int tset;

	switch(i){ 
		case 1:
			tset = ioctl(fd,1,read_buf1);
			printf("tset : %d\n", tset);
			printf("device(1): %s\n", read_buf1);
			break;
		case 2:
			printf("enter data(1): ");
			scanf(" %[^\n]",write_buf1);
			tset = ioctl(fd,2, write_buf1);
			printf("tset : %d\n", tset);
			break; 
		case 3:
			ioctl(fd,3,read_buf1);
			printf("device(2): %s\n", read_buf2);
			break;
		case 4:
			printf("enter data(2): ");
			scanf(" %[^\n]",write_buf2);
			ioctl(fd,4, write_buf2);
			break; 
		
		default:
			printf("command not recognized");
			break;
	}
	close(fd);
	return 0;
}