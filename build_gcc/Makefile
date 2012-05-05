SRC_DIR=../src
OBJS=marc_reader.o marc_writer.o marcrecord.o marcrecord_field.o marcrecord_subfield.o \
  marcrecord_tools.o marcxml_reader.o marcxml_writer.o test.o
PROGRAM=test

CXX=g++
CXXFLAGS=-O2 -W -Wall -Wextra -ansi -pedantic -Wpointer-arith -Wwrite-strings -Wno-long-long \
  -DMARCRECORD_MARCXML -I$(SRC_DIR)
LDFLAGS=
LIBS=-lm -lexpat

.PHONY: all clean verify
.SUFFIXES: .cxx .o

all: $(PROGRAM)
	@echo $@ done.

$(PROGRAM): $(OBJS)
	$(CXX) $(CXXFLAGS) $(OBJS) $(LDFLAGS) $(LIBS) -o $@
	@echo $@ done.

clean:
	rm -f $(PROGRAM) $(OBJS) *.iso *.xml
	@echo $@ done.

verify:
	./$(PROGRAM) | more
	@echo $@ done.

%.o: $(SRC_DIR)/%.cxx
	$(CXX) $(CXXFLAGS) -c $< -o $@
