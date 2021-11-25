CXX := mpicxx

TARGET := matmul
HOST_FILE := hosts

SUFFIX=-I/usr/lib/x86_64-linux-gnu/openmpi/include/openmpi -I/usr/lib/x86_64-linux-gnu/openmpi/include -pthread -L/usr/lib/x86_64-linux-gnu/openmpi/lib -lmpi_cxx -lmpi
FLAG=-O3 -m64

all: $(TARGET)

$(TARGET): main.cc matmul.cc
	g++ $(FLAG) main.cc matmul.cc -o $(TARGET) $(SUFFIX)

#all: $(TARGET)

#$(TARGET): main.cc matmul.cc
#	$(CXX) $< -o $(TARGET)

.PHONY: clean test run

test:
	mpirun -np 4 --hostfile $(HOST_FILE) $(TARGET) < test

clean:
	rm -rf $(TARGET)
	
