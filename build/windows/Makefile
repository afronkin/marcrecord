SRC_DIR=..\..\src
SRC_DIR_TEST=$(SRC_DIR)
SRC_DIR_EXPAT=$(SRC_DIR)\expat
SRC_DIR_MARCRECORD=$(SRC_DIR)\marcrecord
SRC_DIR_WIN_ICONV=$(SRC_DIR)\win-iconv

OBJS_DIR=objs
OBJS_DIR_TEST=$(OBJS_DIR)
OBJS_DIR_EXPAT=$(OBJS_DIR)\expat
OBJS_DIR_MARCRECORD=$(OBJS_DIR)\marcrecord
OBJS_DIR_WIN_ICONV=$(OBJS_DIR)\win-iconv

OBJS_TEST=\
  $(OBJS_DIR_TEST)\test.obj
OBJS_EXPAT=\
  $(OBJS_DIR_EXPAT)\xmlparse.obj \
  $(OBJS_DIR_EXPAT)\xmlrole.obj \
  $(OBJS_DIR_EXPAT)\xmltok.obj
OBJS_MARCRECORD=\
  $(OBJS_DIR_MARCRECORD)\marc_reader.obj \
  $(OBJS_DIR_MARCRECORD)\marc_writer.obj \
  $(OBJS_DIR_MARCRECORD)\marciso_reader.obj \
  $(OBJS_DIR_MARCRECORD)\marciso_writer.obj \
  $(OBJS_DIR_MARCRECORD)\marcrecord.obj \
  $(OBJS_DIR_MARCRECORD)\marcrecord_field.obj \
  $(OBJS_DIR_MARCRECORD)\marcrecord_subfield.obj \
  $(OBJS_DIR_MARCRECORD)\marcrecord_tools.obj \
  $(OBJS_DIR_MARCRECORD)\marctext_writer.obj \
  $(OBJS_DIR_MARCRECORD)\marcxml_reader.obj \
  $(OBJS_DIR_MARCRECORD)\marcxml_writer.obj \
  $(OBJS_DIR_MARCRECORD)\unimarcxml_writer.obj
OBJS_WIN_ICONV=\
  $(OBJS_DIR_WIN_ICONV)\win_iconv.obj

BIN_DIR=bin
BIN_TEST=$(BIN_DIR)\test.exe

PLATFORM_SDK=C:\Program Files\Microsoft SDKs\Windows\v7.1

CXXFLAGS=/nologo /MT /O2 /EHsc /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" \
  /FD /I "$(PLATFORM_SDK)\Include"
CXXFLAGS_TEST=$(CXXFLAGS) /I "$(SRC_DIR_MARCRECORD)" /I "$(SRC_DIR_EXPAT)" \
  /I "$(SRC_DIR_WIN_ICONV)" /D "XML_STATIC"
CXXFLAGS_MARCRECORD=$(CXXFLAGS) /I "$(SRC_DIR_MARCRECORD)" /I "$(SRC_DIR_EXPAT)" \
  /I "$(SRC_DIR_WIN_ICONV)" /D "XML_STATIC"

CFLAGS=/nologo /MT /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" \
  /FD /I "$(PLATFORM_SDK)\Include"
CFLAGS_EXPAT=$(CFLAGS) /I "$(SRC_DIR_EXPAT)" /D "HAVE_EXPAT_CONFIG_H" /D "XML_STATIC"

LINK=link
LDFLAGS=/nologo /machine:I386
LIBS=

all: depend $(BIN_TEST)

clean:
	IF EXIST *.idb del /q *.idb
	IF EXIST test rmdir /s /q test
	IF EXIST $(BIN_DIR) rmdir /s /q $(BIN_DIR)
	IF EXIST $(OBJS_DIR) rmdir /s /q $(OBJS_DIR)
	IF EXIST $(OBJS_DIR_EXPAT) rmdir /s /q $(OBJS_DIR_EXPAT)
	IF EXIST $(OBJS_DIR_MARCRECORD) rmdir /s /q $(OBJS_DIR_MARCRECORD)
	IF EXIST $(OBJS_DIR_WIN_ICONV) rmdir /s /q $(OBJS_DIR_WIN_ICONV)

depend:
	mkdir $(BIN_DIR) $(OBJS_DIR) $(OBJS_DIR_EXPAT) $(OBJS_DIR_MARCRECORD) $(OBJS_DIR_WIN_ICONV)

verify:
	IF NOT EXIST test mkdir test
	cd test & ..\$(BIN_TEST) | more

$(BIN_TEST): $(OBJS_TEST) $(OBJS_EXPAT) $(OBJS_MARCRECORD) $(OBJS_WIN_ICONV)
	$(LINK) $(LDFLAGS) /out:$@ $** $(LIBS)

$(OBJS_DIR_TEST)\test.obj: $(SRC_DIR_TEST)\test.cxx
	cl $(CXXFLAGS_TEST) /c /Fo$@ $**

$(OBJS_DIR_EXPAT)\xmlparse.obj: $(SRC_DIR_EXPAT)\xmlparse.c
	cl $(CFLAGS_EXPAT) /c /Fo$@ $**

$(OBJS_DIR_EXPAT)\xmlrole.obj: $(SRC_DIR_EXPAT)\xmlrole.c
	cl $(CFLAGS_EXPAT) /c /Fo$@ $**

$(OBJS_DIR_EXPAT)\xmltok.obj: $(SRC_DIR_EXPAT)\xmltok.c
	cl $(CFLAGS_EXPAT) /c /Fo$@ $**

$(OBJS_DIR_MARCRECORD)\marc_reader.obj: $(SRC_DIR_MARCRECORD)\marc_reader.cxx
	cl $(CXXFLAGS_MARCRECORD) /c /Fo$@ $**

$(OBJS_DIR_MARCRECORD)\marc_writer.obj: $(SRC_DIR_MARCRECORD)\marc_writer.cxx
	cl $(CXXFLAGS_MARCRECORD) /c /Fo$@ $**

$(OBJS_DIR_MARCRECORD)\marciso_reader.obj: $(SRC_DIR_MARCRECORD)\marciso_reader.cxx
	cl $(CXXFLAGS_MARCRECORD) /c /Fo$@ $**

$(OBJS_DIR_MARCRECORD)\marciso_writer.obj: $(SRC_DIR_MARCRECORD)\marciso_writer.cxx
	cl $(CXXFLAGS_MARCRECORD) /c /Fo$@ $**

$(OBJS_DIR_MARCRECORD)\marcrecord.obj: $(SRC_DIR_MARCRECORD)\marcrecord.cxx
	cl $(CXXFLAGS_MARCRECORD) /c /Fo$@ $**

$(OBJS_DIR_MARCRECORD)\marcrecord_field.obj: $(SRC_DIR_MARCRECORD)\marcrecord_field.cxx
	cl $(CXXFLAGS_MARCRECORD) /c /Fo$@ $**

$(OBJS_DIR_MARCRECORD)\marcrecord_subfield.obj: $(SRC_DIR_MARCRECORD)\marcrecord_subfield.cxx
	cl $(CXXFLAGS_MARCRECORD) /c /Fo$@ $**

$(OBJS_DIR_MARCRECORD)\marcrecord_tools.obj: $(SRC_DIR_MARCRECORD)\marcrecord_tools.cxx
	cl $(CXXFLAGS_MARCRECORD) /c /Fo$@ $**

$(OBJS_DIR_MARCRECORD)\marctext_writer.obj: $(SRC_DIR_MARCRECORD)\marctext_writer.cxx
	cl $(CXXFLAGS_MARCRECORD) /c /Fo$@ $**

$(OBJS_DIR_MARCRECORD)\marcxml_reader.obj: $(SRC_DIR_MARCRECORD)\marcxml_reader.cxx
	cl $(CXXFLAGS_MARCRECORD) /c /Fo$@ $**

$(OBJS_DIR_MARCRECORD)\marcxml_writer.obj: $(SRC_DIR_MARCRECORD)\marcxml_writer.cxx
	cl $(CXXFLAGS_MARCRECORD) /c /Fo$@ $**

$(OBJS_DIR_MARCRECORD)\unimarcxml_writer.obj: $(SRC_DIR_MARCRECORD)\unimarcxml_writer.cxx
	cl $(CXXFLAGS_MARCRECORD) /c /Fo$@ $**

$(OBJS_DIR_WIN_ICONV)\win_iconv.obj: $(SRC_DIR_WIN_ICONV)\win_iconv.c
	cl $(CFLAGS_WIN_ICONV) /c /Fo$@ $**
