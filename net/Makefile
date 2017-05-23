.PHONY: all clean

TARGET = ./distributor ./calculator
OBJS = server.o calculator.o
DEPS = $(OBJS:.o=.d)
FLAGS = -Wall -Wextra -Wextra -Werror -Wpedantic -g -O3
ifeq ($(shell uname),Linux)
	FLAGS += -pthread
endif


all: $(TARGET)

distributor: distributor.o
	gcc $(FLAGS) -o $@ $<

calculator: calculator.o
	gcc $(FLAGS) -o $@ $<

-include $(DEPS)

%.o: %.c
	gcc $(FLAGS) -c -o $@ $<
	gcc $(FLAGS) -MM -o $(patsubst %.o, %.d, $@) $<

clean:
	-@rm $(OBJS) $(TARGET) -rf
	-@find . -name "*.o" | xargs rm -rf
	-@find . -name "*.d" | xargs rm -rf
