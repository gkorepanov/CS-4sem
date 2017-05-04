.PHONY: clean run

TARGET = ./integral
FLAGS = -Wall -Wextra -Wextra -Werror -Wpedantic -g -pthread -O3
LIBS = -lm -pthread
OBJS = integral.o
DEPS = $(OBJS:.o=.d)


$(TARGET): $(OBJS)
	gcc -o $@ $^ $(LIBS)

-include $(DEPS)

%.o: %.c
	gcc $(FLAGS) -c -o $@ $<
	gcc $(FLAGS) -MM -o $(patsubst %.o, %.d, $@) $<

clean:
	@rm $(OBJS) $(TARGET)
	@find . -name "*.d" | xargs rm

run:
	$(TARGET) 1 2 1

debug: FLAGS += -DDEBUG
debug: $(TARGET)