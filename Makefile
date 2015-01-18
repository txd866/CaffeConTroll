PROG = deepnet
all: 
	clang++ -Wall -std=c++11 -I./lib/OpenBLAS/ -L./lib/OpenBLAS/ src/main.cpp -lopenblas -O3 -o $(PROG)

clean:
	rm $(PROG)
