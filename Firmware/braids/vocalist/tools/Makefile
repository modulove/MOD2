OBJS = reciter.o sam.o render.o debug.o processframes.o createtransitions.o rendertabs.o parser.o
PARSER_OBJS = $(addprefix braids/vocalist/tools/src/, $(OBJS))

CC = g++

CFLAGS =  -Wall -g

all: braids/vocalist/wordlist.cc

braids/vocalist/wordlist.cc: braids/vocalist/tools/parser
	ruby ./braids/vocalist/tools/src/generator.rb braids/vocalist/tools/words

braids/vocalist/tools/parser: $(PARSER_OBJS)
	$(CC) -o braids/vocalist/tools/parser $(PARSER_OBJS)

braids/vocalist/tools/src/%.o: braids/vocalist/tools/src/%.cc
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm braids/vocalist/tools/parser braids/vocalist/tools/src/*.o braids/vocalist/wordlist.* || true
