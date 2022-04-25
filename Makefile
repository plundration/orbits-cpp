build:
	g++ -g -o main `find . -name '*.cc'` -lraylib -lyaml-cpp

run:
	./main

all: build
	./main

release:
	g++ -O3 -o main_release `find . -name '*.cc'` -lraylib -lyaml-cpp
	
run_release: release
	./main_release