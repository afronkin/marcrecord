PROG_NAME=test

OBJS=marcrec.o test.o
MODULES=
VPATH=../src

CXX=CC
CXXFLAGS=-O2
LIBS=

.PHONY: all clean
.SUFFIXES: .cpp .o

all: $(PROG_NAME)
	@echo $@ done.

$(PROG_NAME): $(OBJS) $(MODULES)
	$(CXX) $(CXXFLAGS) *.o $(LIBS) -o $@
	@echo $@ done.

clean:
	rm -f $(PROG_NAME) *.o
	@echo $@ done.

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

module.%:
	make -f Makefile.$*
