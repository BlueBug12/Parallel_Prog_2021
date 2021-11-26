CXX := mpicxx

TARGET := matmul
HOST_FILE := hosts
FLAG=-O3

all: $(TARGET)

$(TARGET): main.cc matmul.cc
	g++ $(FLAG) $^ -o $(TARGET) -lmpi


.PHONY: clean test run

run:
	mpirun -np 4 --hostfile $(HOST_FILE) $(TARGET) < /HW4/data/data1_1
	mpirun -np 4 --hostfile $(HOST_FILE) $(TARGET) < /HW4/data/data1_2
	mpirun -np 4 --hostfile $(HOST_FILE) $(TARGET) < /HW4/data/data1_3
	mpirun -np 4 --hostfile $(HOST_FILE) $(TARGET) < /HW4/data/data1_4
	mpirun -np 4 --hostfile $(HOST_FILE) $(TARGET) < /HW4/data/data1_5
	mpirun -np 4 --hostfile $(HOST_FILE) $(TARGET) < /HW4/data/data1_6
	mpirun -np 4 --hostfile $(HOST_FILE) $(TARGET) < /HW4/data/data2_1
	mpirun -np 4 --hostfile $(HOST_FILE) $(TARGET) < /HW4/data/data2_2
	mpirun -np 4 --hostfile $(HOST_FILE) $(TARGET) < /HW4/data/data2_3
	mpirun -np 4 --hostfile $(HOST_FILE) $(TARGET) < /HW4/data/data2_4
	mpirun -np 4 --hostfile $(HOST_FILE) $(TARGET) < /HW4/data/data2_5
	mpirun -np 4 --hostfile $(HOST_FILE) $(TARGET) < /HW4/data/data2_6

test:
	mpirun -np 4 --hostfile $(HOST_FILE) $(TARGET) < test

clean:
	rm -rf $(TARGET)
	
