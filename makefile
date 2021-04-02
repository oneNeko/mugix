CXX ?= g++

DEBUG ?= 1
ifeq ($(DEBUG), 1)
    CXXFLAGS += -g
else
    CXXFLAGS += -O2
endif
CXXFLAGS += -std=c++11

server: *.cpp
	$(CXX) -o server.o  $^ $(CXXFLAGS) 

clean:
	rm  -r server.o