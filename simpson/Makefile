.PHONY: clean run

TARGET = ./integral
FLAGS = -Wall -Wextra -Wextra -Werror -Wpedantic -g
OBJS = integral.o
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
	$(TARGET) test.txt

debug: FLAGS += -DDEBUG
debug: $(TARGET)