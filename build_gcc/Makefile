SRC_DIR=../src
OBJS=marcrecord.o marcrecord_field.o marcrecord_iso2709.o \
  marcrecord_marcxml.o marcrecord_subfield.o marcrecord_tools.o test.o
PROGRAM=test

CXX=g++
CXXFLAGS=-g -Wall -Wextra -pedantic -Wmissing-prototypes -DMARCRECORD_MARCXML \
  -I$(SRC_DIR)
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
	rm -f $(PROGRAM) $(OBJS) *.iso
	@echo $@ done.

verify:
	./$(PROGRAM) ../share/test/rusmarc_2.iso | iconv -f cp1251 -t utf-8 | less
	@echo $@ done.

%.o: $(SRC_DIR)/%.cxx
	$(CXX) $(CXXFLAGS) -c $< -o $@
