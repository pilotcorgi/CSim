# Notes
# In general, all commonly available linux distributions will use libstdc++ by default.
#
# If you want to compile c++11 code, include the following command options in your make file.
# -std=c++11 enables c++11 features
#

LIB= -std=c++11
CFLAG= -c -O3
GCC=g++

all:SimNode.o SimTransistor.o Simulator.o Solver.o AccurateSolver.o WeakPreciseSolver.o SolverFactory.o main.o
	$(GCC) $(LIB) -o CSim -g SimNode.o SimTransistor.o Simulator.o Solver.o AccurateSolver.o WeakPreciseSolver.o SolverFactory.o main.o
Simulator.o:
	$(GCC) $(LIB) $(CFLAG) Simulator.cpp
SimTransistor.o:
	$(GCC) $(LIB) $(CFLAG) SimTransistor.cpp
SimNode.o:
	$(GCC) $(LIB) $(CFLAG) SimNode.cpp
Solver.o:
	$(GCC) $(LIB) $(CFLAG) Solver.cpp
AccurateSolver.o:
	$(GCC) $(LIB) $(CFLAG) AccurateSolver.cpp
WeakPreciseSolver.o:
	$(GCC) $(LIB) $(CFLAG) WeakPreciseSolver.cpp
SolverFactory.o:
	$(GCC) $(LIB) $(CFLAG) SolverFactory.cpp
main.o:
	$(GCC) $(LIB) $(CFLAG) main.cpp
clean:
	rm -f *.o CSim
