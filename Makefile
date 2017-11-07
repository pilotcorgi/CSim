# Notes
# In general, all commonly available linux distributions will use libstdc++ by default.
#
# If you want to compile c++11 code, include the following command options in your make file.
# -std=c++11 enables c++11 features
#

INC= -I ./CircuitRelated -I ./Solvers
LIB= -std=c++11
CFLAG= -c -O3
GCC=g++

all:SimNode.o SimTransistor.o Simulator.o Solver.o AccurateSolver.o WeakPreciseSolver.o SolverFactory.o main.o
	$(GCC) $(LIB) -o CSim -g SimNode.o SimTransistor.o Simulator.o Solver.o AccurateSolver.o WeakPreciseSolver.o SolverFactory.o main.o
Simulator.o:
	$(GCC) $(LIB) $(INC) $(CFLAG) Simulator.cpp
SimTransistor.o:
	$(GCC) $(LIB) $(INC) $(CFLAG) ./CircuitRelated/SimTransistor.cpp
SimNode.o:
	$(GCC) $(LIB) $(INC) $(CFLAG) ./CircuitRelated/SimNode.cpp
Solver.o:
	$(GCC) $(LIB) $(INC) $(CFLAG) ./Solvers/Solver.cpp
AccurateSolver.o:
	$(GCC) $(LIB) $(INC) $(CFLAG) ./Solvers/AccurateSolver.cpp
WeakPreciseSolver.o:
	$(GCC) $(LIB) $(INC) $(CFLAG) ./Solvers/WeakPreciseSolver.cpp
SolverFactory.o:
	$(GCC) $(LIB) $(INC) $(CFLAG) ./Solvers/SolverFactory.cpp
main.o:
	$(GCC) $(LIB) $(INC) $(CFLAG) main.cpp
clean:
	rm -f *.o CSim
