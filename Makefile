CC := gcc --std=c99
FLAGS := -Wall

LINK = -ledit -lm
LIB = lib/mpc.c

SRCDIR := src
BUILDDIR := build
TARGET := repl


SRC := $(shell find $(SRCDIR) -type f -name *.c)
OBJ := $(patsubst $(SRCDIR)/%, $(BUILDDIR)/%, $(SRC:.c=.o))

INC := -I include


$(TARGET): $(OBJ)
	$(CC) $^ -o $(TARGET) $(LIB) $(INC) $(LINK)


$(BUILDDIR)/%.o: $(SRCDIR)/%.c
	$(CC) $(FLAGS) $(INC) -c $< -o $@

# %: %.o
# 	${CC} ${FLAGS} $< ${LIB} ${LINK} -o $@


# %.o: %.c
# 	${CC} -c $<


# repl: repl.c mpc.c
# 	${CC} ${FLAGS} $< ${LIB} ${LINK} -o $@


clean:
	rm -rf build/*
	rm repl

