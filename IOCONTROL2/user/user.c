#include <stdio.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/ioctl.h>
 
typedef struct
{
    char buf1[100];
    char buf2[100];
} device_struct;

#define GET_BUFFER_1 _IOR('device_inf', 1, device_struct *)
#define SET_BUFFER_1 _IOW('device_inf', 2, device_struct *)
#define GET_BUFFER_2 _IOR('device_inf', 3, device_struct *)
#define SET_BUFFER_2 _IOW('device_inf', 4, device_struct *)

device_struct device_userspace; 
 
void get_vars(int fd, int buf)
{   

    if(buf == 1)
    {
        if (ioctl(fd, GET_BUFFER_1, &device_userspace) == -1)
        {
            perror("ioctl2 get1 failed");
        }
        printf("Buf[1] : %s\n", device_userspace.buf1);
    }
    else
    {
        if (ioctl(fd, GET_BUFFER_2, &device_userspace) == -1)
        {
            perror("ioctl2 get2 failed");
        }
        printf("Buf[2] : %s\n", device_userspace.buf2);
    }
}

void set_vars(int fd, int buf)
{
    char local_buf[100];
    if(buf == 1){
        printf("Enter buf1: \n");
        scanf(" %[^\n]", local_buf);
        strcpy(device_userspace.buf1, local_buf);
        if (ioctl(fd, SET_BUFFER_1, &device_userspace) == -1)
        {
            perror("ioctl set1");
        }
    }
    else
    {
        printf("Enter buf2: \n");
        scanf(" %[^\n]", local_buf);
        strcpy(device_userspace.buf2, local_buf);
        if (ioctl(fd, SET_BUFFER_2, &device_userspace) == -1)
        {
            perror("ioctl set2");
        }
    }
    
}
 
int main(int argc, char *argv[])
{
    char *file_name = "/dev/ioctl2";
    int fd;
    enum
    {
        get1,
        set1,
        get2,
        set2
    } option;
 
    if (argc == 1)
    {
        option = get1;
    }
    else if (argc == 2)
    {
        if (strcmp(argv[1], "-1") == 0)
        {
            option = get1;
        }
        else if (strcmp(argv[1], "-2") == 0)
        {
            option = set1;
        }
        else if (strcmp(argv[1], "-3") == 0)
        {
            option = get2;
        }
        else if (strcmp(argv[1], "-4") == 0)
        {
            option = set2;
        }
        else
        {
            fprintf(stderr, "Usage: %s \n[-1(get buf1)\n-2(set buf1)\n-3(get buf2)\n-4(set buf2)]\n", argv[0]);
            return 1;
        }
    }
    else
    {
        fprintf(stderr, "Usage: %s \n[-1(get buf1)\n-2(set buf1)\n-3(get buf2)\n-4(set buf2)]\n", argv[0]);
        return 1;
    }
    fd = open(file_name, O_RDWR);
    if (fd == -1)
    {
        perror("ioctl2 open failed");
        return 2;
    }
 
    switch (option)
    {
        case get1:
            get_vars(fd,1);
            break;
        case set1:
            set_vars(fd,1);
            break;
        case get2:
            get_vars(fd,2);
            break;
        case set2:
            set_vars(fd,2);
            break;
        default:
            break;
    }
 
    close (fd);
 
    return 0;
}