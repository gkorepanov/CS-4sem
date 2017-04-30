.PHONY: clean run

TARGET = ./main
FLAGS = -Wall -Wextra -Wextra -Werror -Wpedantic -g
OBJS = list.o main.o
DEPS = $(OBJS:.o=.d)


$(TARGET): $(OBJS)
	gcc -o $@ $^

-include $(DEPS)

%.o: %.c
	gcc $(FLAGS) -c -o $@ $<
	gcc $(FLAGS) -MM -o $(patsubst %.o, %.d, $@) $<

clean:
	@rm $(OBJS) $(TARGET)
	@find . -name "*.d" | xargs rm

run:
	$(TARGET)

debug: FLAGS += -DDEBUG
debug: $(TARGET)