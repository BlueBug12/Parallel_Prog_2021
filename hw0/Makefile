CXX = g++

pi.out: pi.o
	$(CXX) $^ -o $@

pi.o: pi.cpp
	$(CXX) -c $< -o $@

.PHONY: clean
	rm -rf pi.out *.o
