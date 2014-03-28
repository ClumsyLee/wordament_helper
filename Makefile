CC = g++
CFLAG = -c -std=c++11 -Wall -Wextra -I "/Users/lisihan969/programing/C++_programing/wordament/src"

bin/solver: obj/main.o obj/dictionary.o obj/wordament_ai.o | bin
	$(CC) -o $@ $^

bin/dictionary_test: obj/dictionary_test.o obj/dictionary.o | bin
	$(CC) -o $@ $^

obj/main.o: src/wordament_ai/main.cpp src/wordament_ai/wordament_ai.h | obj
	$(CC) $(CFLAG) -o $@ $<

obj/dictionary_test.o: src/wordament_ai/testing/dictionary_test.cpp src/wordament_ai/dictionary.h | obj
	$(CC) $(CFLAG) -o $@ $<

obj/dictionary.o: src/wordament_ai/dictionary.cpp src/wordament_ai/dictionary.h | obj
	$(CC) $(CFLAG) -o $@ $<

obj/wordament_ai.o: src/wordament_ai/wordament_ai.cpp src/wordament_ai/wordament_ai.h src/wordament_ai/dictionary.h | obj
	$(CC) $(CFLAG) -o $@ $<

bin:
	mkdir bin

obj:
	mkdir obj

clean:
	-rm obj/*.o
