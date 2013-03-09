SRC_DIR=..\src
OBJS = marc_reader.obj marc_writer.obj marcrecord.obj marcrecord_field.obj \
  marcrecord_subfield.obj marcrecord_tools.obj marctext_writer.obj \
  marcxml_reader.obj marcxml_writer.obj xmlparse.obj xmlrole.obj xmltok.obj \
  win_iconv.obj test.obj
PROGRAM = test.exe

PLATFORM_SDK=C:\Program Files\Microsoft SDKs\Windows\v7.1

CXXFLAGS=/nologo /MT /O2 /EHsc /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" \
  /FD /I "$(PLATFORM_SDK)\Include" /I "..\src" /I "..\src\win-iconv" \
  /D "XML_STATIC"
CFLAGS_EXPAT=/nologo /MT /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" \
  /FD /I "$(PLATFORM_SDK)\Include" /I "..\src\expat" /D "HAVE_EXPAT_CONFIG_H"
CFLAGS_ICONV=/nologo /MT /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" \
  /FD /I "$(PLATFORM_SDK)\Include" /I "..\src\win-iconv"
LDFLAGS=/nologo /machine:I386
LIBS=

all: $(PROGRAM)

$(PROGRAM): $(OBJS)
	link $** $(LIBS) $(LDFLAGS) /out:$@

clean:
	del *.exe *.idb *.obj *.iso *.txt *.xml

verify:
	$(PROGRAM)

marc_reader.obj: $(SRC_DIR)\marc_reader.cxx
	cl /c $(CXXFLAGS) $**

marc_writer.obj: $(SRC_DIR)\marc_writer.cxx
	cl /c $(CXXFLAGS) $**

marcrecord.obj: $(SRC_DIR)\marcrecord.cxx
	cl /c $(CXXFLAGS) $**

marcrecord_field.obj: $(SRC_DIR)\marcrecord_field.cxx
	cl /c $(CXXFLAGS) $**

marcrecord_subfield.obj: $(SRC_DIR)\marcrecord_subfield.cxx
	cl /c $(CXXFLAGS) $**

marcrecord_tools.obj: $(SRC_DIR)\marcrecord_tools.cxx
	cl /c $(CXXFLAGS) $**

marctext_writer.obj: $(SRC_DIR)\marctext_writer.cxx
	cl /c $(CXXFLAGS) $**

marcxml_reader.obj: $(SRC_DIR)\marcxml_reader.cxx
	cl /c $(CXXFLAGS) $**

marcxml_writer.obj: $(SRC_DIR)\marcxml_writer.cxx
	cl /c $(CXXFLAGS) $**

xmlparse.obj: $(SRC_DIR)\expat\xmlparse.c
	cl /c $(CFLAGS_EXPAT) $**

xmlrole.obj: $(SRC_DIR)\expat\xmlrole.c
	cl /c $(CFLAGS_EXPAT) $**

xmltok.obj: $(SRC_DIR)\expat\xmltok.c
	cl /c $(CFLAGS_EXPAT) $**

win_iconv.obj: $(SRC_DIR)\win-iconv\win_iconv.c
	cl /c $(CFLAGS_IFCONV) $**

test.obj: $(SRC_DIR)\test.cxx
	cl /c $(CXXFLAGS) $**
