PROG_NAME=test

OBJS=marcrec.o marcrec_field.o marcrec_subfield.o test.o
MODULES=
VPATH=../src

CXX=CC
CXXFLAGS=-g -O2
LDFLAGS=
LIBS=

.PHONY: all clean
.SUFFIXES: .cxx .o

all: $(PROG_NAME)
	@echo $@ done.

$(PROG_NAME): $(OBJS) $(MODULES)
	$(CXX) *.o $(LDFLAGS) $(LIBS) -o $@
	@echo $@ done.

clean:
	rm -f $(PROG_NAME) *.o
	@echo $@ done.

%.o: %.cxx
	$(CXX) $(CXXFLAGS) -c $< -o $@

module.%:
	make -f Makefile.$*
