SYSTEMC_HOME := /home/wdz/riscv-vp/riscv-vp/vp/dependencies/systemc-dist
SYSTEMC_INCLUDE := $(SYSTEMC_HOME)/include 
SYSTEMC_LIBDIR := $(SYSTEMC_HOME)/lib

SrcDir = ../core
INCDIR = -I../core
INCDIR += -I$(SYSTEMC_INCLUDE)
#SRCs += ../core/instr.cpp 
SRCs += $(wildcard $(SrcDir)/*.cpp)
OBJs = $(notdir $(SRCs:%.cpp=%.o ))

CXX = g++
CXX_FLAGS = $(INCDIR) -L$(SYSTEMC_LIBDIR) -lsystemc -lboost_iostreams -lboost_program_options -g

#CXX = ~/test/riscv-gnu-toolchain-dist-rv32imac-ilp32/bin
$(warning $(SRCs))
$(warning $(OBJs))

all:soc

soc:$(OBJs)
	$(CXX) $(OBJs) $(CXX_FLAGS) -o soc  

$(OBJs):%.o:$(SrcDir)/%.cpp
	$(CXX) -c $(CXX_FLAGS) -o $@ $<

clean:
	-rm $(OBJs) soc

