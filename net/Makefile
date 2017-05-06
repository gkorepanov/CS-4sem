.PHONY: clean run

TARGET = ./server ./client
FLAGS = -Wall -Wextra -Wextra -Werror -Wpedantic -g
DEPS = $(OBJS:.o=.d)


all: $(TARGET)

server: server.o
	gcc $(FLAGS) -o $@ $<

client: client.o
	gcc $(FLAGS) -o $@ $<

-include $(DEPS)

%.o: %.c
	gcc $(FLAGS) -c -o $@ $<
	gcc $(FLAGS) -MM -o $(patsubst %.o, %.d, $@) $<

clean:
	-@rm $(OBJS) $(TARGET)
	-@find . -name "*.d" | xargs rm -rf

run: $(TARGET) 
	echo NIY

debug: FLAGS += -DDEBUG
debug: $(TARGET)
