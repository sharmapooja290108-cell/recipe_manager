CC = clang
CFLAGS = -Wall -Wextra -O2 -std=c11 -Iinclude
TARGET = ../bin/recipe_manager_c
SRCS = src/main.c src/recipe.c src/storage.c
OBJS = $(SRCS:.c=.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	@mkdir -p ../bin
	$(CC) $(CFLAGS) -o $@ $^

src/%.o: src/%.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)

run: $(TARGET)
	@$(TARGET)

.PHONY: all clean run
