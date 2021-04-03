CXX ?= g++

DEBUG ?= 1
ifeq ($(DEBUG), 1)
    CXXFLAGS += -g
else
    CXXFLAGS += -O2
endif
CXXFLAGS += -std=c++11

server: *.cc ./http/*.cc 
	$(CXX) -o server.o  $^ $(CXXFLAGS) 

clean:
	rm  -r server.o