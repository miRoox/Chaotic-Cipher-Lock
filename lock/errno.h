/**
 ** Copyright 2017-2018 Lu <miroox@outlook.com>
 **
 ** Licensed under the Apache License, Version 2.0 (the "License");
 ** you may not use this file except in compliance with the License.
 ** You may obtain a copy of the License at
 **
 **     http://www.apache.org/licenses/LICENSE-2.0
 **
 ** Unless required by applicable law or agreed to in writing, software
 ** distributed under the License is distributed on an "AS IS" BASIS,
 ** WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 ** See the License for the specific language governing permissions and
 ** limitations under the License.
 **/

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