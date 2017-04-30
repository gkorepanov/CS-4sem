.PHONY: clean run

TARGET = ./main
FLAGS = -Wall -Wextra -Wextra -Werror -Wpedantic -g
LFLAGS = -fprofile-arcs -ftest-coverage
OBJS = list.o main.o
DEPS = $(OBJS:.o=.d)


$(TARGET): $(OBJS)
	gcc $(LFLAGS) -o $@ $^

-include $(DEPS)

%.o: %.c
	gcc $(FLAGS) $(LFLAGS) -c -o $@ $<
	gcc $(FLAGS) $(LFLAGS) -MM -o $(patsubst %.o, %.d, $@) $<

clean:
	-@rm $(OBJS) $(TARGET)
	@find . -name "*.d"    | xargs rm
	@find . -name "*.gcov" | xargs rm
	@find . -name "*.gcno" | xargs rm
	@find . -name "*.gcda" | xargs rm

run:
	$(TARGET)
	gcov -a list.c

debug: FLAGS += -DDEBUG
debug: $(TARGET)