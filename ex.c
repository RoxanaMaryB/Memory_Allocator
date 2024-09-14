#include "def.h"
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(void)
{
	char command[16];
	heap *h;        // h de tip pointer la heap
	array *a;       // vector de structuri array
	info_mem *info; // structura cu informatii despre memorie
	info = (info_mem *)malloc(sizeof(info_mem)); // alocare info
	DIE(!info, "malloc failed");          // erori pt info
	info->tot_mem = 0 /*memorie totala*/;
	info->alloc_blocks = 0 /*dimensiune a*/;
	info->alloc_bytes = 0 /*suma spatiilor alocate (suma a->spatiu)*/;
	info->free_blocks = 0 /*nr de blocuri de memorie libere*/;
	info->malloc_calls = 0 /*nr de apeluri malloc*/;
	info->nr_fragm = 0 /*nr de fragmentari*/;
	info->nr_free = 0 /*nr de apeluri free*/;
	while (1) {
		scanf("%s", command);
		if (strncmp(command, "INIT_HEAP", 9) == 0) {
			h = initialize_heap();
			info->tot_mem = h->nr_bytes_per_l * h->nr_liste;
			info->free_blocks = h->size;
			a = (array *)malloc(h->size * sizeof(array)); // alocare a
			DIE(!a, "malloc failed"); // erori pt a
			for (unsigned int i = 0; i < h->size; i++)
				a[i].data = NULL;
		} else if (strncmp(command, "DUMP_MEMORY", 11) == 0) {
			memory_dump(h, info, a);
		} else if (strncmp(command, "MALLOC", 6) == 0) {
			my_mal(h, info, a);
		} else if (strncmp(command, "FREE", 4) == 0) {
			my_free(h, info, a);
		} else if (strncmp(command, "WRITE", 5) == 0) {
			my_write(h, info, a);
		} else if (strncmp(command, "READ", 4) == 0) {
			my_read(h, info, a);
		} else if (strncmp(command, "DESTROY_HEAP", 12) == 0) {
			destroy_heap(h, info, a);
		} else {
			printf("Invalid command\n");
		}
	}
	return 0;
}
