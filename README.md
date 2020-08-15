# Multithread server example

Simple client multi-thread server example with some C++14 type of bells and whistles. This server simply gets a message, prints out, and sends back a received.

Goal is to extend this to a chat server
which registers users server-side in memory and lets them send short messages back and forth using some json structure. Maybe I'll use protos
or try to make it HTTP not sure. 

## Run it

``` bash
make
```

should produce output

```
clang++ -Wall -g -c util.cc
clang++ -Wall -g -c server_example.cc -pthread
clang++ -Wall -g -c client_example.cc
clang++ -Wall -g -o server -pthread server_example.o util.o
clang++ -Wall -g -o client client_example.o util.o
rm -f *.o
```

Then just start the server:

```
./server --port=12000
```

And in another tab:
```
./requests.sh
```

Alternatively you can start the server in the background using **&** 
but make sure you kill it either by using top or jobs.



