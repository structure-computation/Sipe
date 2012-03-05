all:
	metil_comp -Isrc -g3 src/Sipe/sipe.cpp -di -ds -ws tests/http.sipe.py
#  --exec-using "valgrind --leak-check=full"
# metil_comp --exec-using "valgrind --leak-check=full" -Isrc -g3 src/Sipe/sipe.cpp tests/http.sipe
