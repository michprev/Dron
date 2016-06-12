#ifndef __BUFFERITEM_H
#define __BUFFERITEM_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

	typedef struct bufferItem bufferItem;

	typedef struct bufferItem {
		bufferItem *next;
		char str;
	} bufferItem;


#ifdef __cplusplus
}
#endif

#endif