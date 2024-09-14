CC=gcc
CFLAGS=-Wall -Wextra -std=c99

TARGETS = sfl

build: $(TARGETS)

sfl: ex.c
	$(CC) $(CFLAGS) -g -o sfl func.c utils.c ex.c



pack:
	zip -FSr 311CA_BarangaRoxanaMary_Tema1.zip README.md Makefile *.c *.h

clean:
	rm -f $(TARGETS)

.PHONY: pack clean