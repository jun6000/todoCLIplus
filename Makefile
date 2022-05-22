CC = c++
CFLAGS = -g -Wall
TARGET = todo

all : $(TARGET)

$(TARGET) : main.o todoFn.o
	$(CC) $(CFLAGS) -o $(TARGET) main.o todoFn.o

main.o : main.cpp todoFn.h
	$(CC) $(CFLAGS) -c main.cpp

todoFn.o : todoFn.cpp todoFn.h
	$(CC) $(CFLAGS) -c todoFn.cpp

clean :
	$(RM) $(TARGET) main.o todoFn.o

