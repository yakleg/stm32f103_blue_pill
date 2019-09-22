#include <stdint.h>
#include <errno.h>
#include <sys/unistd.h> // STDOUT_FILENO, STDERR_FILENO
#include <usbd_cdc_if.h>

extern USBD_HandleTypeDef hUsbDeviceFS;

int _write(int file, char *data, int len)
{
    if ((file != STDOUT_FILENO) && (file != STDERR_FILENO))
    {
        errno = EBADF;
        return -1;
    }

    if (CDC_Transmit_FS((uint8_t *)data, len) != USBD_OK)
    {
        errno = EBUSY;

        return -1;
    }
    return len;
}
