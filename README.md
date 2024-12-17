go to directory DigitalTwin


Compiling library
$ cd lib/
$ rm -r build && mkdir build && cd build && cmake .. && make


$ cd ../..

Compilaing testcases
$ cd tests/
$ rm -r build && mkdir build && cd build && cmake .. && make


Running testcases
$./test_lib

$ cd ../..

Compiling app
$ cd app/
$ rm -r build && mkdir build && cd build && cmake .. && make

Running app
$ ./main_app
