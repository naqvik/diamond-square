INCLUDE = $(HOME)/git/doctest/doctest
CXX = g++-9
CXXFLAGS = -Wall -I $(INCLUDE) -std=c++17 -Wsuggest-override -g

ds-test: ds-test.cpp ../src/ds.h
	$(CXX) $(CXXFLAGS) -o ds-test ds-test.cpp ../src/ds.h

check: ds-test
	./$<

clean:
	rm -f *~
	rm -f ds-test
