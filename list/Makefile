.PHONY: clean test

TARGET = ./bin/test
FLAGS = -std=c99 -Wall -Wextra -Wextra -Werror -Wpedantic -g
LFLAGS = -fprofile-arcs -ftest-coverage

SRCS = list/list.c list/list_iterator.c test.c
OBJS = $(SRCS:.c=.o)
DEPS = $(OBJS:.o=.d)


$(TARGET): $(OBJS)
	@mkdir -p bin
	gcc $(LFLAGS) -o $@ $^

-include $(DEPS)

%.o: %.c
	gcc $(FLAGS) $(LFLAGS) -c -o $@ $<
	gcc $(FLAGS) $(LFLAGS) -MM -o $(patsubst %.o, %.d, $@) $<

clean:
	-@rm -rf $(OBJS) $(TARGET)
	-@find . -name "*.d"    | xargs rm -rf
	-@find . -name "*.gcov" | xargs rm -rf
	-@find . -name "*.gcno" | xargs rm -rf
	-@find . -name "*.gcda" | xargs rm -rf
	-@rm -rf bin

test: $(TARGET) 
	-@find . -name "*.gcov" | xargs rm -rf
	-@find . -name "*.gcda" | xargs rm -rf
	$(TARGET)
	gcov -a ./list/list.c ./list/list_iterator.c

debug: FLAGS += -DDEBUG
debug: $(TARGET)

