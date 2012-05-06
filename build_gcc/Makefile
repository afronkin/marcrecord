SRC_DIR=../src
OBJS=marc_reader.o marc_writer.o marcrecord.o marcrecord_field.o marcrecord_subfield.o \
  marcrecord_tools.o marcxml_reader.o marcxml_writer.o test.o xmlparse.o xmlrole.o xmltok.o
PROGRAM=test

CXX=g++
CXXFLAGS=-O2 -W -Wall -Wextra -ansi -pedantic -Wpointer-arith -Wwrite-strings -Wno-long-long \
  -I$(SRC_DIR) -I$(SRC_DIR)/expat -DXML_STATIC
CC=gcc
CFLAGS=-O2 -Wall -ansi -pedantic -Wpointer-arith -Wmissing-prototypes -Wstrict-prototypes \
  -Wold-style-definition -Wwrite-strings -Wno-long-long -I$(SRC_DIR)/expat \
  -DHAVE_EXPAT_CONFIG_H -DXML_STATIC
LDFLAGS=
LIBS=-lm

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

%.o: $(SRC_DIR)/expat/%.c
	$(CC) $(CFLAGS) -c $< -o $@
