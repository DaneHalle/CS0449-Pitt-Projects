#include <stdio.h>
#include <unistd.h>

#include "mymalloc.h"

int main() {
	// You can use sbrk(0) to get the current position of the break.
	// This is nice for testing cause you can see if the heap is the same size
	// before and after your tests, like here.
	void* heap_at_start = sbrk(0);

	void* block1 = my_malloc(64);
	void* block2 = my_malloc(1);
	void* block3 = my_malloc(64);
	void* block4 = my_malloc(1);
	void* block5 = my_malloc(64);
	void* block6 = my_malloc(1);
	void* block7 = my_malloc(64);
	void* block8 = my_malloc(1);
	block8=block8;
	
	my_free(block1);
	my_free(block3);
	my_free(block5);
	my_free(block7);
	my_free(block4);
	my_free(block6);


	void* seg1 = my_malloc(1);
	void* seg2 = my_malloc(1);
	void* seg3 = my_malloc(1);
	void* seg4 = my_malloc(1);

	my_free(block2);
	my_free(seg1);
	my_free(seg2);
	my_free(seg3);
	my_free(seg4);
	my_free(block8);	

	void* heap_at_end = sbrk(0);
	unsigned int heap_size_diff = (unsigned int)(heap_at_end - heap_at_start);

	if(heap_size_diff){
		printf("Hmm, the heap got bigger by %u (0x%X) bytes...\n", heap_size_diff, heap_size_diff);
	}else{
		printf("It worked\n");
	}

	// ADD MORE TESTS HERE.


	return 0;
}
