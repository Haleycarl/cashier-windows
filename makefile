# TO COMPILE THIS PROGRAM YOU NEED GCC AND MAKE
OUTFILE=main # Output file
INFILE=main.cpp # Input Files
l=-l:pdcurses.a -lsqlite3# Libraries
L=-L sqlite -L  include/wincon
I=-I sqlite -I include -I include/wincon
LIBS=-lncurses -lsqlite3
CC=g++ # Compiler
linux:
	$(CC) $(LIBS) $(INFILE) -o $(OUTFILE)
win:
	$(CC) $(INFILE) $I $L $l -o $(OUTFILE)
run:
	./$(OUTFILE) 2> info.log

clean:
	rm main sqlite3
