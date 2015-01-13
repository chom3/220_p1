PROJECT = prj1
COURSE = cs220f14
TARGET = steg

SRC_FILES = \
  Makefile \
  README \
  steg.c \
  steg.h \
  main.c

OBJ_FILES = \
  steg.o \
  main.o

CC = gcc
CFLAGS = -g -Wall -std=c99

$(TARGET):  	$(OBJ_FILES)
		$(CC) $(OBJ_FILES) -o $@

$(PROJECT).tar.gz \
dist:		$(SRC_FILES)
		tar -cvf $(PROJECT).tar $(SRC_FILES)
		gzip $(PROJECT).tar

clean:
		rm -f $(TARGET) *.o *~

steg.o:		steg.c steg.h
main.o:		main.c steg.h
