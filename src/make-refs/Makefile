LIB=../libsrc/libdocmgr.a
PROG=make-refs
CXXFLAGS=-g -I../libsrc
LDFLAGS=-L../libsrc
LIBS=-ldocmgr -lpq

SRCS=make-refs.cpp

OBJS=$(addprefix obj/,$(SRCS:.cpp=.o))

all: obj $(PROG)

obj:
	if [ ! -d obj ]; then mkdir obj ; fi

make-refs: $(OBJS)
	$(CXX) -g $(LDFLAGS) -o $@ $^ $(LIBS)

depend:
	makedepend -Y -pobj/ -- $(CXXFLAGS) -- $(SRCS) 2> /dev/null

clean:
	rm -f make-refs $(OBJS)

obj/%.o: %.cpp
	$(COMPILE.cpp) -o $@ $<


# DO NOT DELETE THIS LINE -- make depend depends on it.

obj/make-refs.o: ../libsrc/DocMgr.hh
