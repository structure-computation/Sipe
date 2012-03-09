MC = metil_comp -Wall -Isrc -g3 src/Sipe/sipe.cpp 

all:
	${MC} -di -ws -e tests/http.sipe.py

exe:
	${MC} -e tests/http.sipe.py

#  -di -ds -ws
val:
	${MC} --valgrind -ws tests/http.sipe.py
#  --exec-using "valgrind --leak-check=full"
# metil_comp --exec-using "valgrind --leak-check=full" -Isrc -g3 src/Sipe/sipe.cpp tests/http.sipe
