#include "stringBuffer.h"

void stringBufferInit(stringBuffer * b)
{
	b->first = NULL;
	b->last = NULL;
	b->count = 0;
}

char* stringBufferAdd(stringBuffer *b)
{
	char *str = malloc(10 * sizeof(char));

	bufferItem *tmp = malloc(sizeof(struct bufferItem));
	tmp->next = NULL;
	tmp->str = str;

	if (b->last != NULL) {
		b->last->next = tmp;
	}
	else {
		b->last = tmp;
		b->first = b->last;
	}

	b->count++;

	return str;
}

void stringBufferRemove(stringBuffer *b)
{
	bufferItem *oldFirst = b->first;

	b->first = b->first->next;

	free(oldFirst->str);
	free(oldFirst);

	b->count--;
}
