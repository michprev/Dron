#include "stringBuffer.h"

void stringBufferInit(stringBuffer * b)
{
	b->first = NULL;
	b->last = NULL;
	b->count = 0;
}

void stringBufferAdd(stringBuffer *b, char str)
{
	bufferItem *tmp = malloc(sizeof(struct bufferItem));
	tmp->next = NULL;
	tmp->str = str;

	if (b->last != NULL) {
		b->last->next = tmp;
		b->last = tmp;
	}
	else {
		b->last = tmp;
		b->first = tmp;
	}

	b->count++;
}

void stringBufferRemove(stringBuffer *b)
{
	bufferItem *oldFirst = b->first;

	b->first = b->first->next;

	free(oldFirst);

	b->count--;
}
