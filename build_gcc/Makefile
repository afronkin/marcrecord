SRC_DIR=../src
OBJS=marcrec.o marcrec_field.o marcrec_iso2709.o marcrec_subfield.o test.o
PROGRAM=test

CXX=g++
CXXFLAGS=-g -Wall -Wextra -pedantic -Wmissing-prototypes -I$(SRC_DIR)
LDFLAGS=
LIBS=-lm

.PHONY: all clean verify
.SUFFIXES: .cxx .o

all: $(PROGRAM)
	@echo $@ done.

$(PROGRAM): $(OBJS)
	$(CXX) $(OBJS) $(LDFLAGS) $(LIBS) -o $@
	@echo $@ done.

clean:
	rm -f $(PROGRAM) $(OBJS)
	@echo $@ done.

verify:
	./$(PROGRAM) ../share/test/rusmarc_2.iso | iconv -f cp1251 -t utf-8 | less
	@echo $@ done.

%.o: $(SRC_DIR)/%.cxx
	$(CXX) $(CFLAGS) -c $< -o $@
