SRC = $(wildcard *.cpp)
OBJ = $(SRC:%.cpp=%.o)
$(warning OBJ: $(OBJ))
EXE = $(notdir $(PWD)).x
FLAGS = -I$(SYSTEMC_INCLUDE) -L$(SYSTEMC_LIBDIR) -lsystemc

$(EXE):$(OBJ)
	g++ $(FLAGS) $(OBJ) -o $@ 

$(OBJ):%.o:%.cpp
	g++ -c $(FLAGS) $< -o $@

clean:
	rm -rf $(OBJ) $(EXE)
