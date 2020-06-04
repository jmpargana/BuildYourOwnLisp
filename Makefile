CC := gcc --std=c99
FLAGS := -Wall

LINK = -lreadline -lm
LIB = lib/mpc.c

SRCDIR := src
BUILDDIR := build
TARGET := lispy


SRC := $(shell find $(SRCDIR) -type f -name *.c)
OBJ := $(patsubst $(SRCDIR)/%, $(BUILDDIR)/%, $(SRC:.c=.o))

INC := -I include


$(TARGET): $(OBJ)
	$(CC) $(FLAGS) $^ -o $(TARGET) $(LIB) $(INC) $(LINK)


$(BUILDDIR)/%.o: $(SRCDIR)/%.c
	$(CC) $(FLAGS) $(INC) -c $< -o $@


clean:
	rm -rf build/*
	rm lispy

