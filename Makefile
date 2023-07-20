CC = g++
CFLAGS=-std=c++17 -Wall -Wextra -pedantic -O3 -static
LFLAGS = -lgmp

TARGET = g2
SRCS = main.cpp ECC.cpp
OBJS = $(SRCS:.cpp=.o)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(TARGET) $(LFLAGS)

%.o: %.cpp
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)
