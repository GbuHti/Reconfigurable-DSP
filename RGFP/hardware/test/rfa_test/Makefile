SYSTEMC_HOME := /home/wdz/riscv-vp/riscv-vp/vp/dependencies/systemc-dist
SYSTEMC_INCLUDE := $(SYSTEMC_HOME)/include 
SYSTEMC_LIBDIR := $(SYSTEMC_HOME)/lib

INCDIR = -I../../core -I../../RFU
INCDIR += -I$(SYSTEMC_INCLUDE)
SRCs += ../../RFU/arith_PE.cpp
SRCs += ../../RFU/combiner.cpp
SRCs += ../../RFU/crossbar.cpp
SRCs += ../../RFU/distributor.cpp
SRCs += ../../RFU/loader.cpp
SRCs += ../../RFU/RFA.cpp
SRCs += ../../RFU/storer.cpp
SRCs += ../../core/device_proxy.cpp
SRCs += ../../core/memory.cpp
SRCs += main.cpp 

OBJs = $(SRCs:%.cpp=%.o )

CXX = g++
CXX_FLAGS = $(INCDIR) -L$(SYSTEMC_LIBDIR) -lsystemc -lboost_iostreams -lboost_program_options -g

$(warning $(SRCs))
$(warning $(OBJs))

all:test

test:$(OBJs)
	$(CXX) $(OBJs) $(CXX_FLAGS) -o test

$(OBJs):%.o:%.cpp
	$(CXX) -c $(CXX_FLAGS) -o $@ $<

clean:
	-rm $(OBJs) test

