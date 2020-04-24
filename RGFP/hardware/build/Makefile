SYSTEMC_HOME := /home/wdz/riscv-vp/riscv-vp/vp/dependencies/systemc-dist
SYSTEMC_INCLUDE := $(SYSTEMC_HOME)/include 
SYSTEMC_LIBDIR := $(SYSTEMC_HOME)/lib

SrcDir1 = ../core
SrcDir2 = ../RFU
INCDIR = -I../core -I../RFU
INCDIR += -I$(SYSTEMC_INCLUDE)
#SRCs += ../core/instr.cpp 
SRCs1 += $(wildcard $(SrcDir1)/*.cpp)
SRCs2 += $(wildcard $(SrcDir2)/*.cpp)

OBJs1 = $(notdir $(SRCs1:%.cpp=%.o ))
OBJs2 = $(notdir $(SRCs2:%.cpp=%.o ))
OBJs = $(OBJs1)
OBJs += $(OBJs2)

CXX = g++
CXX_FLAGS = $(INCDIR) -L$(SYSTEMC_LIBDIR) -lsystemc -lboost_iostreams -lboost_program_options -g

#CXX = ~/test/riscv-gnu-toolchain-dist-rv32imac-ilp32/bin
$(warning $(SRCs))
$(warning $(OBJs))

all:soc

soc:$(OBJs)
	$(CXX) $(OBJs) $(CXX_FLAGS) -o soc  

$(OBJs1):%.o:$(SrcDir1)/%.cpp
	$(CXX) -c $(CXX_FLAGS) -o $@ $<

$(OBJs2):%.o:$(SrcDir2)/%.cpp
	$(CXX) -c $(CXX_FLAGS) -o $@ $<

clean:
	-rm $(OBJs) soc

