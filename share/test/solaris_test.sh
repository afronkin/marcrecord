#!/bin/sh

../../build_solaris/test rusmarc_2.iso | iconv -f cp1251 -t utf-8 | less
# ../../build_solaris/test rusmarc_2.iso
