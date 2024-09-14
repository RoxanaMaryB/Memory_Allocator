**Nume: Baranga Roxana Mary**
**Grupă: 311CA**

## Segregated Free Lists Tema 1

### Descriere:

* Programul este un alocator de memorie virtual ce permite alocarea memoriei, scrierea, citirea si eliberarea blocurilor de memorie.

## Comenzi posibile:

### INIT_HEAP <adresa_start_heap> <nr_liste> <nr_bytes_per_lista> <tip>
        
```markdown
- initializeaza un heap de la <adresa_start_heap>, cu <nr_liste> liste de cate <nr_bytes_per_listă> bytes.
- <tip> va fi intotdeauna 0 pentru aceasta implementare
```

### MALLOC <nr_bytes>

```markdown
- aloca <nr_bytes> memorie, scoate acesti bytes din heap-ul de memorie libera si ii pune intr-un vector de structuri
- daca se aloca mai putin decat un bloc intreg, se fragmenteaza, iar memoria nealocata este pusa in lista corespunzatoare ei
```

### FREE <adresa_>

```markdown
- elibereaza memoria de la adresa <adresa_> si o pune inapoi in heap, in lista coresponzatoare dimesiunii sale
- daca adresa nu a fost alocata, se afiseaza mesajul "Invalid free"
```

### WRITE <adresa> <date> <nr_bytes>

```markdown
- scrie <nr_bytes> de la adresa <adresa> cu datele <date>
- daca adresa nu a fost alocata sau exista o adresa nealocata pe parcursul celor <nr_bytes>, se afiseaza mesajul "Segmentation fault (core dumped)" si se face dump al intregii memorii (DUMP_MEMORY)
- informatia din <date> este delimitata de ghilimele care nu vor fi luate in considerare
```

### READ <adresa> <nr_bytes>

```markdown
- citeste <nr_bytes> de la adresa <adresa> si le afiseaza
- daca adresa nu a fost alocata sau exista o adresa nealocata pe parcursul celor <nr_bytes>, se afiseaza mesajul "Segmentation fault (core dumped)" si se face dump al intregii memorii (DUMP_MEMORY)
```
    
### DUMP_MEMORY

```markdown
- afiseaza toata memoria alocata si nealocata in ordine crescatoare a adreselor si dimensiunilor
```

### DESTROY_HEAP
```markdown
- elibereaza toata memoria folosita de program (memoria reala) si inchide programul
```

* Explicatii structuri de date folosite:

```markdown
- structura de tip nod (struct dll_node_t) este generica, retine doar datele de tip void * din nodul respectiv, nodul anterior si nodul urmator
- structura de tip lista (struct dll_t) este generica, retine pointer catre primul nod (head), marimea datelor, cati bytes are fiecare nod din lista si cate noduri sunt in lista
- structura de tip heap (struct heap) retinr un vector de liste si informatii esentiale pentru heap: adresa de inceput, numarul de liste, numarul de bytes per lista, tipul heap-ului si cate noduri sunt in heap
- structura de tip array (struct array) este generica si este folosita pentru structura de date ce retine memoria alocata; retine adresa de inceput a blocului alocat, spatiul alocat, adresa de la care se incepe scrierea si data de tip void *
- structura de tip info_mem (struct info_mem) retine informatii care trebuie afisate la memory dump
```

* Explicatii fisiere sursa:

```markdown
- ex.c: fisierul main, unde se citesc comenzile si se apeleaza functiile corespunzatoare
- func.c: fisierul unde sunt implementate functiile specifice listelor dublu inlantuite
- utils.c: fisierul unde sunt implementate functiile specifice alocarii si eliberarii memoriei, scrierii si citirii datelor
- def.h: fisierul unde sunt definite structurile de date si antetele functiilor
```

### Comentarii asupra temei:

* Crezi că ai fi putut realiza o implementare mai bună?
    - Da, cred ca as fi putut realiza o implementare mai buna. Imi propun sa implementez si partea de free recursiv in viitor.
* Ce ai invățat din realizarea acestei teme?
    - Am invatat cum sa aloc si sa eliberez memorie, cum sa scriu si sa citesc date si cum sa folosesc structuri de date generice.
