all: cmp

cmp: src/Sipe/Predef.cpp
	metil_comp -Wall -Isrc -g3 -O3 -ne -o sipe src/Sipe/sipe.cpp

src/Sipe/Predef.cpp: src/Sipe/Predef.sipe src/txt_to_cpp.cpp
	metil_comp -Isrc src/txt_to_cpp.cpp src/Sipe/Predef.cpp src/Sipe/Predef.sipe predef

tst: cmp
	echo -e "//- cpp_flag tata\n//- cpp_flag toto\n" | ./sipe -e tests/test.sipe

tsh: cmp
	#echo -e -n "GET /toto HTTP/1.1\nContent-Length: 128\nContent-Length: 129\n\npuet" > rec
	cat rec
	./sipe -e tests/http.sipe rec

install:
	mkdir -p          ~/.kde4/share/apps/katepart/syntax
	ln    -s sipe.xml ~/.kde4/share/apps/katepart/syntax

clean:
	rm src/Sipe/compilations/*

.PHONY: cmp
