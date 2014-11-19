
only = http.o url.o tools.o flags.o socket.o

objs = hb.o $(only)
target = dist/Debug/GNU-Linux-x86/hb

cc = gcc

$(target) : $(objs)
	-mkdir -p dist/Debug/GNU-Linux-x86/
	-rm -f dist/Debug/GNU-Linux-x86/httpbench
	cc -o $(target) $(objs)
	-ln -s hb dist/Debug/GNU-Linux-x86/httpbench
	-cp $(target) ./

hb.o : hb.c hb.h
http.o : http.c http.h
url.o : url.c url.h
tools.o : tools.c tools.h
flags.o : flags.c flags.h
socket.o : socket.c socket.h


testobjs = tests/httptest.o $(only)
build-tests : $(testobjs)
	cc -o tests/test tests/httptest.o $(only) -lcunit
tests/httptest.o : tests/httptest.c

test : build-tests
	tests/test

.PHONY : clean
clean :
	rm $(target) $(objs)
