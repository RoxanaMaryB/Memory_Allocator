#define MAX_STRING_SIZE 10000

typedef struct dll_node_t dll_node_t;
struct dll_node_t {
	void *data; /* Pentru ca datele stocate sa poata avea orice tip, folosim un
								pointer la void. */
	dll_node_t *prev, *next;
};

typedef struct doubly_linked_t doubly_linked_t;
struct doubly_linked_t {
	dll_node_t *head;
	unsigned int data_size;
	unsigned int nr_bytes;
	unsigned int nr_noduri;
	unsigned int size;
};

typedef struct heap {
	unsigned int start, nr_liste, nr_bytes_per_l, tip, size;
	doubly_linked_t **list; // vector de liste
} heap;

typedef struct array {
	unsigned int adr, spatiu, adr_write;
	void *data; // pointer la sirul de caractere ex: "GIGEL"
} array;

typedef struct info_mem {
	unsigned int tot_mem, alloc_blocks, alloc_bytes, free_blocks,
		malloc_calls, nr_fragm, nr_free;
} info_mem;

doubly_linked_t *dll_create(unsigned int nr_bytes, unsigned int data_size);
dll_node_t *dll_get_nth_node(doubly_linked_t *list, unsigned int n);
void dll_add_nth_node(doubly_linked_t *list, unsigned int n,
					  const void *new_data);
dll_node_t *dll_remove_nth_node(doubly_linked_t *list, unsigned int n);
unsigned int dll_get_size(doubly_linked_t *list);
void dll_free(doubly_linked_t **pp_list);
void dll_print_addr(doubly_linked_t *list);
void my_write(heap *h, info_mem *info, array *a);

heap *initialize_heap(void);
void memory_dump(heap *h, info_mem *info, array *a);
void my_mal(heap *h, info_mem *info, array *a);
void my_free(heap *h, info_mem *info, array *a);
void my_write(heap *h, info_mem *info, array *a);
void my_read(heap *h, info_mem *info, array *a);
void destroy_heap(heap *h, info_mem *info, array *a);
void help_my_write(array *a, unsigned int i, char *str,
				   unsigned int cnt, unsigned int len);
int restruct(info_mem *info, array *a, unsigned int adresa,
			 unsigned int nr_alloc);

#define DIE(assertion, call_description)                                      \
	do {                                                                      \
		if (assertion) {                                                      \
			fprintf(stderr, "(%s, %d): ", __FILE__, __LINE__);                \
			perror(call_description);                                         \
			exit(errno);                                                      \
		}                                                                     \
	} while (0)
