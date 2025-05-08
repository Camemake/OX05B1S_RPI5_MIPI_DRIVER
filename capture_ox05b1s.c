
/* Minimal example that starts streaming and dumps raw frames */
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <linux/videodev2.h>

int main(void)
{
    const char *dev = "/dev/video0";
    int fd = open(dev, O_RDWR);
    if (fd < 0) {
        perror("open");
        return 1;
    }

    struct v4l2_format fmt = {0};
    fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    fmt.fmt.pix.width  = 2592;
    fmt.fmt.pix.height = 1944;
    fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_SRGGB12;
    fmt.fmt.pix.field  = V4L2_FIELD_NONE;
    if (ioctl(fd, VIDIOC_S_FMT, &fmt) < 0) {
        perror("S_FMT");
        return 1;
    }

    /* … add streaming & mmap … */
    printf("Format set OK\n");
    close(fd);
    return 0;
}
