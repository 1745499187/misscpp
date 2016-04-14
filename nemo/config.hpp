/*
 * nemo_config.h
 *
 *  Created on: 2012-11-5
 *      Author: edward
 */

#ifndef NEMO_CONFIG_H_
#define NEMO_CONFIG_H_

#include <nemo/auto_config.h>

#define NEMO_BUF_SZ_XS   256
#define NEMO_BUF_SZ_S    512
#define NEMO_BUF_SZ      1024
#define NEMO_BUF_SZ_L    2048
#define NEMO_BUF_SZ_XL   4096

#define NEMO_SOCK_MAX_CONN       128
#define NEMO_EPOLL_MAX_FD        256
#define NEMO_EPOLL_MAX_EVENT     20

#define DBG(fmt, args...) \
{\
    printf("%s.%s:%d - "fmt"\n", __FILE__, __FUNCTION__, __LINE__, ##args);\
}

#endif /* NEMO_CONFIG_H_ */
