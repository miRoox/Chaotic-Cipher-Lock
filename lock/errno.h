#ifndef __ERRNO_H_
#define __ERRNO_H_

#define EPERM 1 /* Operation not permitted */
#define EINTR 4 /* Interrupted system call */
#define EIO 5 /* I/O error */
#define ENXIO 6 /* No such device or address */
#define E2BIG 7 /* Argument list too long */
#define ENOMEM 12 /* Out of memory */
#define EFAULT 14 /* Bad address */
#define EINVAL 22 /* Invalid argument */
#define ENODATA 61 /* No data available */
#define ENOBUFS 105 /* No buffer space available */
#define EREMOTEIO 121 /* Remote I/O error */
#define EPSW 128 /* Wrong password */
#define EPSW2 129 /* Wrong key to open */

#endif