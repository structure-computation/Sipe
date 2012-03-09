MC = metil_comp -Wall -Isrc -g3 src/Sipe/sipe.cpp 

all: src/Sipe/Predef.cpp
	${MC} tests/http.sipe.py

exe: src/Sipe/Predef.cpp
	${MC} -e tests/http.sipe.py

#  -di -ds -ws
val:
	${MC} --valgrind -ws tests/http.sipe.py

src/Sipe/Predef.cpp: src/Sipe/Predef.sipe src/txt_to_cpp.cpp
	metil_comp -Isrc src/txt_to_cpp.cpp src/Sipe/Predef.cpp src/Sipe/Predef.sipe predef
