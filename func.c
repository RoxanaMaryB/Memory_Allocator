#include "def.h"
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
 * Functie care trebuie apelata pentru alocarea si initializarea unei liste.
 * (Setare valori initiale pentru campurile specifice structurii LinkedList).
 */
doubly_linked_t *dll_create(unsigned int nr_bytes,
							unsigned int data_size)
{
	doubly_linked_t *l = malloc(sizeof(doubly_linked_t));
	DIE(!l, "malloc failed");
	l->head = NULL;
	l->size = 0; // nr de noduri din lista (setat cu k) doar la inceput
	l->data_size = data_size;
	l->nr_noduri = 0;       // nr noduri curente din lista
	l->nr_bytes = nr_bytes; // nr de bytes pe care il are fiecare nod
	return l;
}

/*
 * Functia intoarce un pointer la nodul de pe pozitia n din lista.
 * Pozitiile din lista sunt indexate incepand cu 0 (i.e. primul nod din lista se
 * afla pe pozitia n=0). Daca n >= nr_noduri, atunci se intoarce nodul de pe
 * pozitia rezultata daca am "cicla" (posibil de mai multe ori) pe lista si am
 * trece de la ultimul nod, inapoi la primul si am continua de acolo. Cum putem
 * afla pozitia dorita fara sa simulam intreaga parcurgere?
 * Atentie: n>=0 (nu trebuie tratat cazul in care n este negativ).
 */
dll_node_t *dll_get_nth_node(doubly_linked_t *list, unsigned int n)
{
	dll_node_t *curr;
	if (!list || !list->head)
		return NULL;
	curr = list->head;
	while (n >= list->nr_noduri)
		n -= list->nr_noduri;
	for (unsigned int i = 0; i < n; i++)
		curr = curr->next;
	return curr;
}

/*
 * Pe baza datelor trimise prin pointerul new_data, se creeaza un nou nod care e
 * adaugat pe pozitia n a listei reprezentata de pointerul list. Pozitiile din
 * lista sunt indexate incepand cu 0 (i.e. primul nod din lista se afla pe
 * pozitia n=0). Cand indexam pozitiile nu "ciclam" pe lista circulara ca la
 * get, ci consideram nodurile in ordinea de la head la ultimul (adica acel nod
 * care pointeaza la head ca nod urmator in lista). Daca n >= nr_noduri, atunci
 * adaugam nodul nou la finalul listei.
 * Atentie: n>=0 (nu trebuie tratat cazul in care n este negativ).
 */
void dll_add_nth_node(doubly_linked_t *list, unsigned int n,
					  const void *new_data)
{
	dll_node_t *prev, *curr;
	dll_node_t *new;

	if (!list) { // lista nu exista
		return;
	}

	if (n > list->nr_noduri) { // daca vreau sa adaug pe o pozitie
		n = list->nr_noduri; // mai mare decat nr de elem
	}

	curr = list->head;
	prev = NULL;
	while (n > 0) {
		prev = curr;
		curr = curr->next;
		n--;
	}

	new = malloc(sizeof(*new));
	new->data = malloc(list->data_size);
	memcpy(new->data, new_data, list->data_size);

	new->next = curr;
	new->prev = prev;
	if (!prev) { // daca vreau sa il adaug pe prima pozitie
		list->head = new;
	} else {
		prev->next = new;
	}

	if (curr)
		curr->prev = new;

	list->nr_noduri++;
}

/*
 * Elimina nodul de pe pozitia n din lista al carei pointer este trimis ca
 * parametru. Pozitiile din lista se indexeaza de la 0 (i.e. primul nod din
 * lista se afla pe pozitia n=0). Functia intoarce un pointer spre acest nod
 * proaspat eliminat din lista. Daca n >= nr_noduri - 1, se elimina nodul de la
 * finalul listei. Este responsabilitatea apelantului sa elibereze memoria
 * acestui nod.
 * Atentie: n>=0 (nu trebuie tratat cazul in care n este negativ).
 */
dll_node_t *dll_remove_nth_node(doubly_linked_t *list, unsigned int n)
{
	dll_node_t *prev, *curr;

	if (!list || !list->head)
		return NULL;

	if (n >= list->nr_noduri) { // daca vreau sa elimin ultimul element
		n = list->nr_noduri - 1;
	}

	curr = list->head;
	prev = NULL;
	while (n > 0) {
		prev = curr;
		curr = curr->next;
		n--;
	}

	if (!prev) { // daca vreau sa elimin primul element
		list->head = curr->next;
		if (curr->next) // daca lista are > 1 elem
			curr->next->prev = NULL;
	} else {
		prev->next = curr->next;
		if (curr->next) // daca lista are > 1 elem
			curr->next->prev = prev;
	}

	list->nr_noduri--;
	return curr;
}

/*
 * Functia intoarce numarul de noduri din lista al carei pointer este trimis ca
 * parametru.
 */
unsigned int dll_get_size(doubly_linked_t *list)
{
	return list->nr_noduri;
}

/*
 * Procedura elibereaza memoria folosita de toate nodurile din lista, iar la
 * sfarsit, elibereaza memoria folosita de structura lista.
 */
void dll_free(doubly_linked_t **pp_list)
{
	dll_node_t *curr = (*pp_list)->head;
	for (unsigned int i = 0; i < (*pp_list)->nr_noduri; i++) {
		dll_node_t *aux = curr->next;
		free(curr->data);
		free(curr);
		curr = aux;
	}
	(*pp_list)->nr_noduri = 0;
	(*pp_list)->head = 0;
	(*pp_list)->data_size = 0;
	(*pp_list)->nr_bytes = 0;
	(*pp_list)->size = 0;
	free(*pp_list);
}

/*
 * Atentie! Aceasta functie poate fi apelata doar pe liste ale caror noduri STIM
 * ca stocheaza int-uri. Functia afiseaza toate valorile int stocate in nodurile
 * din lista separate printr-un spatiu, incepand de la primul nod din lista.
 */
void dll_print_addr(doubly_linked_t *list)
{
	if (!list || !list->head) {
		printf("no list\n");
		return;
	}

	dll_node_t *curr = list->head;

	for (unsigned int i = 0; i < list->nr_noduri; i++) {
		printf("%#x", *((unsigned int *)(curr->data)));
		if (i != list->nr_noduri - 1)
			printf(" ");
		else
			printf("\n");
		curr = curr->next;
	}
}
