#ifndef __DEBUG_H__
#define __DEBUG_H__

#include <stdio.h>

#define DEBUG


#ifdef DEBUG
#define DMSG(args...)       printf(args)
#define DMSG_STR(str)       printf("%s\n", str)
#define DMSG_HEX(num)       printf("%X", (num>>4)&0x0F); printf("%X", num&0x0F)
#define DMSG_INT(num)       printf("%d", num)
#else
#define DMSG(args...)
#define DMSG_STR(str)
#define DMSG_HEX(num)
#define DMSG_INT(num)
#endif

#endif
