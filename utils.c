#include "def.h"
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

heap *initialize_heap(void)
{
	int k = 8;
	heap *h = (heap *)malloc(sizeof(heap)); // alocare h
	DIE(!h, "malloc failed");        // erori pt h

	scanf("%x%d%d%d", &h->start, &h->nr_liste, &h->nr_bytes_per_l, &h->tip);
	h->size = 0; // nr de blocuri de memorie alocate

	h->list = (doubly_linked_t **)malloc(h->nr_liste *
											sizeof(doubly_linked_t *));
	DIE(!h->list, "malloc failed"); // erori pt h->list

	for (unsigned int i = 0; i < h->nr_liste; i++) {
		h->list[i] = dll_create(k, sizeof(unsigned int)); // creare liste;
		// k - cati octeti sunt pe fiecare nod din lista i =>   l->size
		k *= 2;
	}

	unsigned int adr_curr = h->start; // adresa curenta

	// adaugare in h a blocurilor de memorie
	for (unsigned int i = 0; i < h->nr_liste; i++) {
		h->list[i]->size = h->nr_bytes_per_l / h->list[i]->nr_bytes;
		for (unsigned int j = 0; j < h->list[i]->size; j++) {
			dll_add_nth_node(h->list[i], j, &adr_curr);
			adr_curr += h->list[i]->nr_bytes;
			h->size++;
		}
	}

	return h;
}

void memory_dump(heap *h, info_mem *info, array *a)
{
	printf("+++++DUMP+++++\n");
	printf("Total memory: %d bytes\n", info->tot_mem);
	printf("Total allocated memory: %d bytes\n", info->alloc_bytes);
	printf("Total free memory: %d bytes\n", info->tot_mem - info->alloc_bytes);
	printf("Free blocks: %d\n", info->free_blocks);
	printf("Number of allocated blocks: %d\n", info->alloc_blocks);
	printf("Number of malloc calls: %d\n", info->malloc_calls);
	printf("Number of fragmentations: %d\n", info->nr_fragm);
	printf("Number of free calls: %d\n", info->nr_free);
	for (unsigned int i = 0; i < h->nr_liste; i++) {
		if (h->list[i] &&
			h->list[i]->head) { // cand se aloca tot din lista, dar nu vreau
			//sa distrug lista de tot, modif doar afisarea
			printf("Blocks with %d bytes - %d free block(s) : ",
				   h->list[i]->nr_bytes,
				   h->list[i]->nr_noduri);
			dll_print_addr(h->list[i]);
		}
	}
	printf("Allocated blocks :");
	for (unsigned int i = 0; i < info->alloc_blocks; i++) {
		if (i == 0)
			printf(" ");
		printf("(%#x - %d)", a[i].adr, a[i].spatiu);
		if (i != info->alloc_blocks - 1)
			printf(" ");
	}
	printf("\n");
	printf("-----DUMP-----\n");
}

int restruct(info_mem *info, array *a, unsigned int adresa,
			 unsigned int nr_alloc)
{
	for (unsigned int j = 0; j < info->alloc_blocks; j++) {
		if (a[j].adr > adresa) {
			for (unsigned int k = info->alloc_blocks; k > j; k--) {
				a[k].adr = a[k - 1].adr;
				a[k].spatiu = a[k - 1].spatiu;
				if (a[k].data) {
					free(a[k].data);
					a[k].data = NULL;
				}
				a[k].data = (char *)malloc(a[k - 1].spatiu *
											sizeof(char));
				memcpy(a[k].data, a[k - 1].data, a[k - 1].spatiu);
			}
			a[j].adr = adresa;
			a[j].spatiu = nr_alloc;
			if (a[j].data) {
				free(a[j].data);
				a[j].data = NULL;
			}
			a[j].data = (char *)malloc((nr_alloc + 1) * sizeof(char));
			DIE(!a[j].data, "malloc failed");
			((char *)a[j].data)[0] = '\0';
			return 1;
		}
	}
	return 0;
}

void my_mal(heap *h, info_mem *info, array *a)
{
	unsigned int nr_alloc, contor, ramas, gasit = 0, ok2 = 0, adresa = 0;
	scanf("%d", &nr_alloc);
	for (unsigned int i = 0; i < h->nr_liste; i++) {// scot blocul din lista
		if (h->list[i]->nr_bytes >= nr_alloc && h->list[i]->nr_noduri > 0) {
			gasit = 1;
			dll_node_t *n = dll_remove_nth_node(h->list[i], 0);
			if (n)
				adresa = *(unsigned int *)n->data;
			if (n->data) {
				free(n->data);
				n->data = NULL;
			}
			if (n) {
				free(n);
				n = NULL;
			}
			ok2 = restruct(info, a, adresa, nr_alloc);
			if (ok2 == 0) {
				a[info->alloc_blocks].adr = adresa;
				a[info->alloc_blocks].spatiu = nr_alloc;
				a[info->alloc_blocks].data =
					(char *)malloc((nr_alloc + 1) * sizeof(char));
				DIE(!a[info->alloc_blocks].data, "malloc failed");
				((char *)a[info->alloc_blocks].data)[0] = '\0';
			}
			info->alloc_bytes += nr_alloc;
			info->alloc_blocks++;
			info->free_blocks--;
			contor = i;
			break;
		}
	}
	if (gasit == 0) { // daca nu am gasit loc in nicio lista
		printf("Out of memory\n");
		return;
	}
	info->malloc_calls++;
	if (nr_alloc <
		h->list[contor]->nr_bytes) { // daca am fragmentare
		info->nr_fragm++;
		info->free_blocks++;
		ramas = h->list[contor]->nr_bytes - nr_alloc;
		unsigned int adr = adresa + nr_alloc;
		for (unsigned int i = 0; i < h->nr_liste; i++) { //caut mem ramasa
			if (h->list[i]->nr_bytes == ramas) { // pun in lista existenta
				int ok = 0;
				for (unsigned int j = 0; j < h->list[i]->nr_noduri; j++) {
					//caut locul unde sa pun in lista pe care am gasit-o
					if (*(unsigned int *)dll_get_nth_node(h->list[i], j)->data
						> adr) {
						h->list[i]->size++;
						dll_add_nth_node(h->list[i], j, &adr);
						ok = 1;
						break;
					}
				}
				if (ok == 0) {
					dll_add_nth_node(h->list[i], h->list[i]->nr_noduri, &adr);
					h->list[i]->size++;
				}
				break;
			} else if (h->list[i]->nr_bytes >
						ramas) { // fac lista noua pt memoria ramasa
				doubly_linked_t **tmp = (doubly_linked_t **)realloc(h->list,
					(h->nr_liste + 1) *
					sizeof(doubly_linked_t *));
				DIE(!tmp, "realloc failed");
				h->list = tmp;
				for (unsigned int j = h->nr_liste; j > i; j--)
					h->list[j] = h->list[j - 1]; //shift
				h->nr_liste++;
				h->list[i] = dll_create(ramas, sizeof(unsigned int));
				h->list[i]->size = 1;
				dll_add_nth_node(h->list[i], 0, &adr);
				break;
			}
		}
	}
}

void my_free(heap *h, info_mem *info, array *a)
{
	unsigned int adr_free, contor = 0, gasit = 0;
	scanf("%x", &adr_free);
	if (h->tip == 0) {
		for (unsigned int i = 0; i < info->alloc_blocks; i++) {
			if (a[i].adr == adr_free) {
				gasit = 1;
				info->alloc_bytes -= a[i].spatiu;
				info->free_blocks++;
				info->nr_free++;
				contor = i;
				break;
			}
		}
		if (gasit == 0) {
			printf("Invalid free\n");
			return;
		}
		for (unsigned int i = 0; i < h->nr_liste; i++) {
			if (h->list[i]->nr_bytes == a[contor].spatiu) {
				int ok = 0;
				for (unsigned int j = 0; j < h->list[i]->nr_noduri; j++) {
					if (*(unsigned int *)dll_get_nth_node(h->list[i], j)->data
						> adr_free) {
						h->list[i]->size++;
						dll_add_nth_node(h->list[i], j, &adr_free);
						ok = 1;
						break;
					}
				}
				if (ok == 0) {
					h->list[i]->size++;
					dll_add_nth_node(h->list[i], h->list[i]->nr_noduri,
									 &adr_free);
				}
				break;
			} else if (h->list[i]->nr_bytes > a[contor].spatiu) {
				doubly_linked_t **tmp = (doubly_linked_t **)
										realloc(h->list,
												(h->nr_liste + 1) *
												sizeof(doubly_linked_t *));
				DIE(!tmp, "realloc failed");
				h->list = tmp;
				for (unsigned int j = h->nr_liste; j > i; j--)
					h->list[j] = h->list[j - 1];
				h->nr_liste++;
				h->list[i] = dll_create(a[contor].spatiu, sizeof(unsigned int));
				h->list[i]->size = 1;
				dll_add_nth_node(h->list[i], 0, &adr_free);
				break;
			}
		}
		for (unsigned int i = contor; i < info->alloc_blocks - 1; i++) {
			a[i].adr = a[i + 1].adr;
			a[i].spatiu = a[i + 1].spatiu;
			if (a[i].data) {
				free(a[i].data);
				a[i].data = NULL;
			}
			a[i].data = (char *)malloc(a[i + 1].spatiu * sizeof(char));
			memcpy(a[i].data, a[i + 1].data, a[i + 1].spatiu);
		}
		if (a[info->alloc_blocks - 1].data) {
			free(a[info->alloc_blocks - 1].data);
			a[info->alloc_blocks - 1].data = NULL;
		}
		info->alloc_blocks--;
	}
}

void help_my_write(array *a, unsigned int i, char *str,
				   unsigned int cnt, unsigned int len)
{
	unsigned int p = i;
	while (1) {
		a[p].adr_write = a[p].adr;
		if (len - cnt <= a[i].spatiu) {
			//daca am suficient spatiu in blocul curent
			unsigned int min = 0;
			if (len - cnt < a[p].spatiu)
				min = len - cnt;
			else
				min = a[p].spatiu;
			memmove(a[p].data, str + cnt, min - 1);
			break;
		}
		//daca nu am suficient spatiu in blocul curent
		memmove(a[p].data, str + cnt, a[p].spatiu);
		cnt += a[p].spatiu;
		p++;
	}
}

void my_write(heap *h, info_mem *info, array *a)
// WRITE 0x1 "Bla bla "ceva" BLA" 40
{
	unsigned int adresa = 0, nr_bytes = 0, ok = 0, cnt = 0;
	char c;
	char *buff = (char *)malloc(MAX_STRING_SIZE * sizeof(char)); //alocare str
	char *str = (char *)malloc(MAX_STRING_SIZE * sizeof(char)); //alocare str
	char *aux = (char *)malloc(MAX_STRING_SIZE * sizeof(char)); //alocare str
	scanf("%x", &adresa); //adresa de la care se scrie
	scanf("%c", &c);//null
	fgets(buff, MAX_STRING_SIZE, stdin);
	int index = 0, pos = 0;
	unsigned int len = 0;
	for (int i = strlen(buff) - 1; i > 0; i--) {
		if (buff[i] == '"') {
			pos = i;
			break;
		}
	}
	int i = 1;
	for (i = 1; i < pos; i++)
		str[i - 1] = buff[i];
	str[i] = '\0';
	pos++;
	len = pos - 1;
	for (unsigned int i = pos; i < strlen(buff); i++)
		aux[index++] = buff[i];
	aux[index] = '\0';
	nr_bytes = atoi(aux);
	free(aux);
	free(buff);
	if (nr_bytes > len)
		nr_bytes = len;
	unsigned int spatiu_total = 0;
	for (unsigned int i = 0; i < info->alloc_blocks; i++) {
		if (adresa == a[i].adr) { //daca incep de la inceputul blocului
			for (unsigned int j = i; j < info->alloc_blocks; j++)
				spatiu_total += a[j].spatiu;
			if (spatiu_total < nr_bytes) { //daca nu am suficient spatiu
				printf("Segmentation fault (core dumped)\n");
				memory_dump(h, info, a);
				free(str);
				destroy_heap(h, info, a);
				exit(0);
			} else {
				help_my_write(a, i, str, cnt, len);
				ok = 1;
				break;
			}
		}
	}
	if (ok == 0) {
		printf("Segmentation fault (core dumped)\n");
		memory_dump(h, info, a);
		destroy_heap(h, info, a);
		exit(0);
	}
	free(str);
}

void my_read(heap *h, info_mem *info, array *a)
{
	unsigned int adr = 0, ok = 0, nrb = 0, adr_curr = 0, auxnrb = 0, cnt = 0;
	scanf("%x%d", &adr, &nrb);
	for (unsigned int i = 0; i < info->alloc_blocks; i++)
		if (a[i].adr == adr) {
			if (nrb > a[i].spatiu) {
				cnt = i;
				adr_curr = a[i].adr;
				auxnrb = nrb - a[i].spatiu;
				while (auxnrb > a[cnt].spatiu && cnt < info->alloc_blocks - 1) {
					adr_curr += a[cnt].spatiu;
					if (adr_curr != a[cnt + 1].adr) {
						printf("Segmentation fault (core dumped)\n");
						memory_dump(h, info, a);
						destroy_heap(h, info, a);
						exit(0);
					}
					auxnrb -= a[cnt].spatiu;
					cnt++;
				}
			}
			while (nrb) {
				char *s = (char *)calloc((nrb + 1), sizeof(char));
				DIE(!s, "malloc failed");
				size_t size_to_copy = 0;
				if (nrb > a[i].spatiu)
					size_to_copy = a[i].spatiu;
				else
					size_to_copy = nrb;
				if (a[i].data)
					memmove(s, a[i].data, size_to_copy);
				for (unsigned int j = 0; j < size_to_copy; j++)
					printf("%c", s[j]);
				nrb -= size_to_copy;
				free(s);
				if (nrb == 0) {
					printf("\n");
					ok = 1;
					break;
				} else if (nrb > 0) {
					i++;
				}
			}
			break;
		}
	if (ok == 0) {
		printf("Segmentation fault (core dumped)\n");
		memory_dump(h, info, a);
		destroy_heap(h, info, a);
		exit(0);
	}
}

void destroy_heap(heap *h, info_mem *info, array *a)
{
	for (unsigned int i = 0; i < h->nr_liste; i++)
		dll_free(&h->list[i]);
	free(h->list);
	free(h);
	for (unsigned int i = 0; i < info->alloc_blocks; i++) {
		if (a[i].data)
			free(a[i].data);
		}
	free(a);
	free(info);
	exit(0);
}
