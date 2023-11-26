CC = g++
CFLAGS  = -g -O0 -Wall -std=c++17

CFLAGS_RELEASE = -O3 -s -DNDEBUG -Wall -std=c++17

OBJS =  src/utils/numbers.cpp src/utils/strings.cpp src/parsers/value_type.cpp src/parsers/eu4_parser.cpp src/main.cpp
INCLUDE =


all:eu4_parser

eu4_parser:
	${CC} ${CFLAGS}  ${INCLUDE} -o build/eu4_parser.exe ${OBJS}
	./build/eu4_parser.exe

eu4_parser_release:
	${CC} ${CFLAGS_RELEASE}  ${INCLUDE} -o build/eu4_parser_release.exe ${OBJS}
	./build/eu4_parser_release.exe

clean:
	rm -f build/*.o build/*.exe