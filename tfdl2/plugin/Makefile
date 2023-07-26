DIR_INC = -I$(PWD)/../include/tfdl/ -I$(PWD)/../include/frogHeart/
LIB_PATH = -L$(PWD)/../tools/lib/

CFLAGS = -O2 -fPIC $(DIR_INC) $(LIB_PATH) -pthread -std=c++11 -pie -shared
LIBS = -lstdc++ -lm -llib_tfdl -lopenblas

DIR_SRC = $(PWD)
DIR_OBJ = $(PWD)
DIRS:=$(shell find $(DIR_SRC) -maxdepth 0 -type d)
SRC = $(foreach dir, $(DIRS), $(wildcard $(dir)/*.cpp))
OBJ = $(patsubst %.cpp, %.o, $(SRC))

TARGET = plugin.so

$(TARGET):$(OBJ)
	$(CC) -o $@ $(OBJ) $(LIBS) $(CFLAGS) 
	
.cpp.o:
	$(CC) -c $(CFLAGS) $< -o $@
	
clean:
	rm -f *.o $(TARGET)
