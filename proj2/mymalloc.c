#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>

#include "mymalloc.h"

// USE THIS GODDAMN MACRO OKAY
#define PTR_ADD_BYTES(ptr, byte_offs) ((void*)(((char*)(ptr)) + (byte_offs)))

// Don't change or remove these constants.
#define MINIMUM_ALLOCATION  16
#define SIZE_MULTIPLE       8


unsigned int round_up_size(unsigned int data_size) {
	if(data_size == 0)
		return 0;
	else if(data_size < MINIMUM_ALLOCATION)
		return MINIMUM_ALLOCATION;
	else
		return (data_size + (SIZE_MULTIPLE - 1)) & ~(SIZE_MULTIPLE - 1);
}

//------------------------------------------------------------------------------
//Allocating methods

typedef struct Header
{
	unsigned int size;
	void* next;
	void* previous;
	bool used;
} Header;

Header* head=NULL;
Header* tail=NULL;

void* splitHeaders(unsigned int size, Header* begin)
{
	unsigned int blockSize=begin->size;
	begin->size=size;
	begin->used=true;
	Header* newAllocate=PTR_ADD_BYTES(begin, size+sizeof(Header));
	newAllocate->size=blockSize-size-sizeof(Header);
	newAllocate->used=false;

	if(begin->next==NULL){
		newAllocate->next=NULL;
		newAllocate->previous=begin;
		begin->next=newAllocate;
		tail=newAllocate;
	}else{
		Header* next=begin->next;
		newAllocate->next=next;
		next->previous=newAllocate;
		newAllocate->previous=begin;
		begin->next=newAllocate;
		
	}
	return PTR_ADD_BYTES(begin, sizeof(Header));
}

void* setBlock(Header* here)
{
	here->used=true;
	return PTR_ADD_BYTES(here, sizeof(Header));
}

void* parseHeaders(unsigned int size)
{
	for(Header* current=head; current->next!=NULL; current=current->next){
		int alteredSplit=current->size-size-sizeof(Header)-16;
		int frag=current->size-size;
		if(!current->used && alteredSplit>=0){
			return splitHeaders(size, current);
		}
		if(!current->used && frag>=0){
			return setBlock(current);
		}
	}
	return NULL;
}

void* makeHeadTail(unsigned int size)
{
	Header* here=sbrk(sizeof(Header));
	here->size=size;
	here->used=true;
	here->previous=NULL;
	here->next=NULL;
	sbrk(size);
	return here;
}

void* allocate(unsigned int size)
{
	Header* here = sbrk(sizeof(Header));
	sbrk(size);
	here->size=size;
	here->used=true;

	here->next=NULL;
	tail->next=here;
	here->previous=tail;
	tail=here;

	return PTR_ADD_BYTES(sbrk(0), -size);
}

//------------------------------------------------------------------------------

void* my_malloc(unsigned int size) {
	if(size == 0)
		return NULL;

	size = round_up_size(size);

	// ------- Don't remove anything above this line. -------
	
	if(head==NULL){
		Header* headTail=makeHeadTail(size);
		head=headTail;
		tail=headTail;
		return PTR_ADD_BYTES(head, sizeof(Header));
	}

	void* p=parseHeaders(size);

	if(p==NULL){
		return allocate(size);
	}

	return p;
}

//------------------------------------------------------------------------------
//Freeing methods

Header* coTail(Header* previous, Header* here)
{
	previous->size=previous->size+here->size+sizeof(Header);
	previous->next=NULL;
	tail=previous;
	return previous;
}

Header* coPrevious(Header* previous, Header* middle, Header* next)
{
	previous->size=middle->size+previous->size+sizeof(Header);
	previous->next=next;
	next->previous=previous;
	return previous;
}

Header* coNext(Header* here, Header* next, Header* nextnext)
{
	here->size=here->size+next->size+sizeof(Header);
	here->next=nextnext;
	nextnext->previous=here;
	return here;
}

Header* coalesce(Header* here)
{
	here->used=false;
	if(here->previous!=NULL && here->next!=NULL){
		Header* previous=here->previous;
		Header* next=here->next;

		if(!next->used){
			here=coNext(here, next, next->next);
		}
		if(!previous->used){
			here=coPrevious(previous, here, here->next);
		}
		return here;
	}else if(here->previous==NULL && here->next!=NULL){

		Header* next=here->next;
		if(!next->used){
			here=coNext(here, next, next->next);
		}
		return here;
	}else if(here->previous!=NULL && here->next==NULL){
		Header* previous=here->previous;
		if(!previous->used){
			here=coTail(here->previous, here);
		}
		return here;
	}
	return here;
}

void clearEverything(Header* here)
{
	here->used=false;
	head=NULL;
	tail=NULL;
	sbrk(-here->size-sizeof(Header));
}

void freeTail(Header* here)
{
	Header* previous=here->previous;
	tail=previous;
	previous->next=NULL;
	sbrk(-here->size-sizeof(Header));
}

void freeMiddle(Header* here)
{
	here->used=false;
}

//------------------------------------------------------------------------------

void my_free(void* ptr) {
	if(ptr == NULL)
		return;

	Header* here=coalesce(PTR_ADD_BYTES(ptr, -sizeof(Header)));
	if(here->next!=NULL){
		freeMiddle(here);
	}else if(here->next==NULL && here->previous!=NULL){
		freeTail(here);
	}else{
		clearEverything(here);
	}
}
