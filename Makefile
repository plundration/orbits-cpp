build:
	g++ -g -o main `find . -name '*.cc'` -lraylib -lyaml-cpp -lm -ldl -lpthread

run:
	./main

all: build
	./main

release:
	g++ -O3 -o main_release `find . -name '*.cc'` -lraylib -lyaml-cpp -lm -ldl -lpthread
	
run_release: release
	./main_release
