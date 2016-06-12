#ifndef __STRINGBUFFER_H
#define __STRINGBUFFER_H

#ifdef __cplusplus
extern "C" {
#endif

#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include "bufferItem.h"

	typedef struct {
		bufferItem *first;
		bufferItem *last;
		uint8_t count;
	} stringBuffer;

	void stringBufferInit(stringBuffer *b);

	void stringBufferAdd(stringBuffer *b, char str);
	void stringBufferRemove(stringBuffer *b);

#ifdef __cplusplus
}
#endif

#endif