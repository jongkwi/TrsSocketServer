# onlic framing unit test (Rocky 9):  make && ./framing_test
CXX = g++
all: framing_test
framing_test: framing_test.cpp ../source/onlic/Framing.cpp ../include/onlic/Framing.h
	$(CXX) -g -O0 -Wall -I../include/onlic -o framing_test framing_test.cpp ../source/onlic/Framing.cpp
vectors:
	python3 framing_sim.py
clean:
	rm -f framing_test
.PHONY: all vectors clean
