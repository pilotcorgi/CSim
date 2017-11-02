# Notes
# In general, all commonly available linux distributions will use libstdc++ by default.
#
# If you want to compile c++11 code, include the following command options in your make file.
# -std=c++11 enables c++11 features
#

LIB= -std=c++11
CFLAG= -c -O3
GCC=g++

all:SimNode.o SimTransistor.o Simulator.o main.o
	$(GCC) $(LIB) -o CSim -g SimNode.o SimTransistor.o Simulator.o main.o
Simulator.o:SimTransistor.cpp SimTransistor.hpp SimNode.cpp SimNode.hpp Simulator.cpp Simulator.hpp
	$(GCC) $(LIB) $(CFLAG) Simulator.cpp
SimTransistor.o:SimTransistor.cpp SimTransistor.hpp SimNode.cpp SimNode.hpp
	$(GCC) $(LIB) $(CFLAG) SimTransistor.cpp
SimNode.o:SimTransistor.cpp SimTransistor.hpp SimNode.cpp SimNode.hpp
	$(GCC) $(LIB) $(CFLAG) SimNode.cpp
main.o:SimTransistor.cpp SimTransistor.hpp SimNode.cpp SimNode.hpp Simulator.cpp Simulator.hpp
	$(GCC) $(LIB) $(CFLAG) main.cpp
clean:
	rm -f *.o CSim
