SRC_DIR=../../src
SRC_DIR_TEST=$(SRC_DIR)
SRC_DIR_MARCRECORD=$(SRC_DIR)/marcrecord

OBJS_DIR=objs
OBJS_DIR_TEST=$(OBJS_DIR)
OBJS_DIR_MARCRECORD=$(OBJS_DIR)/marcrecord

OBJS_TEST=\
  $(OBJS_DIR_TEST)/test.o
OBJS_MARCRECORD=\
  $(OBJS_DIR_MARCRECORD)/marc_reader.o \
  $(OBJS_DIR_MARCRECORD)/marc_writer.o \
  $(OBJS_DIR_MARCRECORD)/marcrecord.o \
  $(OBJS_DIR_MARCRECORD)/marcrecord_field.o \
  $(OBJS_DIR_MARCRECORD)/marcrecord_subfield.o \
  $(OBJS_DIR_MARCRECORD)/marcrecord_tools.o \
  $(OBJS_DIR_MARCRECORD)/marctext_writer.o \
  $(OBJS_DIR_MARCRECORD)/marcxml_reader.o \
  $(OBJS_DIR_MARCRECORD)/marcxml_writer.o \
  $(OBJS_DIR_MARCRECORD)/unimarcxml_writer.o

BIN_DIR=bin
BIN_TEST=$(BIN_DIR)/test

CXX=g++
CXXFLAGS=-O2 -W -Wall -Wextra -ansi -pedantic -Wpointer-arith -Wwrite-strings -Wno-long-long
CXXFLAGS_TEST=$(CXXFLAGS) -I$(SRC_DIR) -I$(SRC_DIR_MARCRECORD)
CXXFLAGS_MARCRECORD=$(CXXFLAGS) -I$(SRC_DIR_MARCRECORD)

LINK=g++
LDFLAGS=
LIBS=-lm -lexpat -liconv

.PHONY: all clean depend verify
.SUFFIXES: .cxx .o

all: depend $(BIN_TEST)

clean:
	$(RM) -R test $(BIN_DIR) $(OBJS_DIR) $(OBJS_DIR_MARCRECORD)

depend:
	mkdir -p $(BIN_DIR) $(OBJS_DIR) $(OBJS_DIR_MARCRECORD)

verify:
	mkdir -p test
	(cd test ; ../$(BIN_TEST) | more)

$(BIN_TEST): $(OBJS_TEST) $(OBJS_MARCRECORD)
	$(LINK) $(LDFLAGS) -o $@ $^ $(LIBS)

$(OBJS_DIR_TEST)/%.o: $(SRC_DIR_TEST)/%.cxx
	$(CXX) $(CXXFLAGS_TEST) -c -o $@ $<

$(OBJS_DIR_MARCRECORD)/%.o: $(SRC_DIR_MARCRECORD)/%.cxx
	$(CXX) $(CXXFLAGS_MARCRECORD) -c $< -o $@
