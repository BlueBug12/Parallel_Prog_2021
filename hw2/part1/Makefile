CXX = g++

pi.out: pi.o
	$(CXX) $^ -o $@ -lpthread -O3

pi.o: pi.cpp
	$(CXX) -c $< -o $@ -lpthread -O3

.PHONY: clean
	rm -rf pi.out *.o
