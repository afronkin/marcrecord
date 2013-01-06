marcrecord
==========

MARC record library for C++ allow read and write records in ISO2709 and MARCXML
containers and manipulate MARC record content (such as fields, subfields,
record leader etc).

This library development was insipred by marc4j library for Java, available
at http://marc4j.tigris.org, so there is similar conceptions used in some
parts.

Main features:
- support of different contaners, such as ISO2709, MARCXML, text (write-only);
- support of UNIMARC-specific embedded fields;
- support of different encodings and encoding conversion;
- ability to read even incorrect records in many cases;
- focus on speed of batch records processing.

Look at usage examples in "src/test.cxx".
