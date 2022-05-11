CC=g++
CXXFLAGS=-O3 -std=c++11 -Wall -pedantic -D_GNU_SOURCE -I.

LDFLAGS=

MAKEDEPEND=${CC} -MM
PROGRAM=test-json

OBJS = ${PROGRAM}.o json/document.o json/value.o json/object.o json/array.o \
			 json/string.o json/number.o json/boolean.o json/null.o string/buffer.o

DEPS:= ${OBJS:%.o=%.d}

all: $(PROGRAM)

${PROGRAM}: ${OBJS}
	${CC} ${LDFLAGS} ${OBJS} ${LIBS} -o $@

clean:
	rm -f ${PROGRAM} ${OBJS} ${DEPS}

${OBJS} ${DEPS} ${PROGRAM} : Makefile

.PHONY : all clean

%.d : %.cpp
	${MAKEDEPEND} ${CXXFLAGS} $< -MT ${@:%.d=%.o} > $@

%.o : %.cpp
	${CC} ${CXXFLAGS} -c -o $@ $<

-include ${DEPS}
