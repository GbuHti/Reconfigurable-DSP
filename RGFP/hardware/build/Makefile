SYSTEMC_HOME := ~/riscv-vp/riscv-vp/vp/dependencies/systemc-dist
SYSTEMC_INCLUDE := $(SYSTEMC_HOME)/include 
SYSTEMC_LIBDIR := $(SYSTEMC_HOME)/lib

INCDIR = ../helloworld
#SRCs += ../core/instr.cpp 
SRCs += $(wildcard ../helloworld/*.cpp)
OBJs = $(notdir $(SRCs:%.cpp=%.o ))

CXX = g++
CXX_FLAGS = -I$(INCDIR) -I$(SYSTEMC_INCLUDE) -L$(SYSTEMC_LIBDIR) -lsystemc -g

#CXX = ~/test/riscv-gnu-toolchain-dist-rv32imac-ilp32/bin
$(warning $(SRCs))
$(warning $(OBJs))

all:soc

soc:$(OBJs)
	$(CXX) $(OBJs) $(CXX_FLAGS) -o soc  

$(OBJs):$(SRCs)
	$(CXX) -c $< $(CXX_FLAGS) -o $@

