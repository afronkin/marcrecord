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

.PHONY: all clean verify
.SUFFIXES: .cxx .o

all: $(BIN_TEST)

clean:
	$(RM) $(BIN_TEST) $(OBJS_TEST) $(OBJS_MARCRECORD)
	$(RM) -R tmp

verify:
	mkdir -p tmp
	(cd tmp ; ../$(BIN_TEST) | more)

$(BIN_TEST): $(OBJS_TEST) $(OBJS_MARCRECORD)
	$(LINK) $(LDFLAGS) -o $@ $^ $(LIBS)

$(OBJS_DIR_TEST)/test.o: $(SRC_DIR_TEST)/test.cxx
	$(CXX) $(CXXFLAGS_TEST) -c -o $@ $<

$(OBJS_DIR_MARCRECORD)/marc_reader.o: $(SRC_DIR_MARCRECORD)/marc_reader.cxx
	$(CXX) $(CXXFLAGS_MARCRECORD) -c -o $@ $<

$(OBJS_DIR_MARCRECORD)/marc_writer.o: $(SRC_DIR_MARCRECORD)/marc_writer.cxx
	$(CXX) $(CXXFLAGS_MARCRECORD) -c -o $@ $<

$(OBJS_DIR_MARCRECORD)/marcrecord.o: $(SRC_DIR_MARCRECORD)/marcrecord.cxx
	$(CXX) $(CXXFLAGS_MARCRECORD) -c -o $@ $<

$(OBJS_DIR_MARCRECORD)/marcrecord_field.o: $(SRC_DIR_MARCRECORD)/marcrecord_field.cxx
	$(CXX) $(CXXFLAGS_MARCRECORD) -c -o $@ $<

$(OBJS_DIR_MARCRECORD)/marcrecord_subfield.o: $(SRC_DIR_MARCRECORD)/marcrecord_subfield.cxx
	$(CXX) $(CXXFLAGS_MARCRECORD) -c -o $@ $<

$(OBJS_DIR_MARCRECORD)/marcrecord_tools.o: $(SRC_DIR_MARCRECORD)/marcrecord_tools.cxx
	$(CXX) $(CXXFLAGS_MARCRECORD) -c -o $@ $<

$(OBJS_DIR_MARCRECORD)/marctext_writer.o: $(SRC_DIR_MARCRECORD)/marctext_writer.cxx
	$(CXX) $(CXXFLAGS_MARCRECORD) -c -o $@ $<

$(OBJS_DIR_MARCRECORD)/marcxml_reader.o: $(SRC_DIR_MARCRECORD)/marcxml_reader.cxx
	$(CXX) $(CXXFLAGS_MARCRECORD) -c -o $@ $<

$(OBJS_DIR_MARCRECORD)/marcxml_writer.o: $(SRC_DIR_MARCRECORD)/marcxml_writer.cxx
	$(CXX) $(CXXFLAGS_MARCRECORD) -c -o $@ $<

$(OBJS_DIR_MARCRECORD)/unimarcxml_writer.o: $(SRC_DIR_MARCRECORD)/unimarcxml_writer.cxx
	$(CXX) $(CXXFLAGS_MARCRECORD) -c -o $@ $<
