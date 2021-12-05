
#bpt_test.o: bpt_test.cc
#	g++ bpt_test.cc -o bpt_test.o -g -O0 -std=c++17 -L./build/lib/ -lgtest -lgmock -lgtest_main -lgmock_main -I./build/_deps/src/googletest/googletest/include
#
#run: bpt_test.o
#	#g++ bpt_test.cc -o bpt_test -std=c++17  -lgtest
#	g++ bpt_test.o -o bpt_test -std=c++17 -L./build/lib/ -lgtest -lgmock -lgtest_main -lgmock_main
#

main: main.cc
	g++ -O0 -g main.cc -o main -std=c++17

clean:
	rm main *.o