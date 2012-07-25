SRC_DIR=../src
OBJS=marc_reader.o marc_writer.o marcrecord.o marcrecord_field.o marcrecord_subfield.o \
  marcrecord_tools.o marcxml_reader.o marcxml_writer.o test.o xmlparse.o xmlrole.o xmltok.o
PROGRAM=test

CXX=CC
CXXFLAGS=-O2 -I$(SRC_DIR) -I$(SRC_DIR)/expat
CC=cc
CFLAGS=-O2 -I$(SRC_DIR)/expat -DHAVE_EXPAT_CONFIG_H
LDFLAGS=
LIBS=-lm

.PHONY: all clean verify
.SUFFIXES: .cxx .c .o

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
