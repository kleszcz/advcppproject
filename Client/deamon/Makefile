CXX=g++
CXXFLAGS+=-c -g -MMD -MP
LDFLAGS+= -lboost_thread -lboost_system -lboost_serialization
INCLUDE+=/usr/include/

NAME=deamon
SRC=$(wildcard *.cpp)
SRC+=$(wildcard ../*.cpp)
OBJ=$(SRC:.cpp=.o)
DEP=$(patsubst %.o,%.d,$(OBJ))
APP=$(NAME).out


all:  $(APP)
	./$(APP)
	
build: $(APP)

$(APP): $(OBJ)
	$(CXX) $(OBJ) -o $@ $(LDFLAGS)


%.o: %.cpp
	$(CXX) $(CXXFLAGS) $< -o $@
#       @sed -i -e '1s,\($*\)\.o[ :]*,\1.o $*.d: ,' $*.d
-include $(DEP)

clean:
	@rm -f $(OBJ) $(APP) $(DEP)

