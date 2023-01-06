CXX_FLAGS:= -Wall -Wextra

all:
	@g++ $(CXX_FLAGS) main.c list.cpp list_dump.cpp -o proga.out
