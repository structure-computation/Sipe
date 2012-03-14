all: cmp

cmp: src/Sipe/Predef.cpp
	metil_comp -Wall -Isrc -g3 -O3 -ne -o sipe src/Sipe/sipe.cpp 

src/Sipe/Predef.cpp: src/Sipe/Predef.sipe src/txt_to_cpp.cpp
	metil_comp -Isrc src/txt_to_cpp.cpp src/Sipe/Predef.cpp src/Sipe/Predef.sipe predef

tst: cmp
	echo "'to\\\'to'" | ./sipe -e tests/test.sipe

install:
	mkdir -p ~/.kde4/share/apps/katepart/syntax
	ln -s sipe.xml ~/.kde4/share/apps/katepart/syntax

.PHONY: cmp
