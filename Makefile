TARGET      = kp19pp
SOURCEFILES = commandline_options.cpp common.cpp exception.cpp generate_cpp.cpp generate_vimscript.cpp indent.cpp main.cpp scanner.cpp scanner_lexer.cpp target.cpp
CC          = g++
CFLAGS      = -std=c++11 -c
RFLAGS      = -O3
DFLAGS      = -g -O0
INCLUDES    = -I /usr/include -I C:/cppinclude/boost_1_54_0

.PHONY: all
.PHONY: debug
.PHONY: release
.PHONY: run

all: release

release:
	$(CC) $(CFLAGS) $(RFLAGS) $(INCLUDES) $(SOURCEFILES)
	$(CC) -o $(TARGET) $(SOURCEFILES:.cpp=.o) $(LIBS)

debug:
	$(CC) $(CFLAGS) $(DFLAGS) $(INCLUDES) $(SOURCEFILES)
	$(CC) -o $(TARGET) $(SOURCEFILES:.cpp=.o) $(LIBS)

run: release
	./$(TARGET)

