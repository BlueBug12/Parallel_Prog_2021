CXX = g++

pi.out: pi.o
	$(CXX) $^ -o $@ -lpthread

pi.o: pi.cpp
	$(CXX) -c $< -o $@ -lpthread

.PHONY: clean
	rm -rf pi.out *.o