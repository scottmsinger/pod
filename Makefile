
# DEBUG
CPPFLAGS = -DYYDEBUG=1 -DDEBUG -UNDEBUG -O0 -ggdb3 -fPIC

# OPT
#CPPFLAGS = -UYYDEBUG -UDEBUG -DNDEBUG -O3 -g -fPIC


lib_objects = TipPod_version.o TipPodBlockPodValue.o TipPod.o TipPodValue.o TipPodNode.o TipPodUtils.o \
              lexer.o parser.o 

objects = $(lib_objects) main.o


all: parser libTipPod.a

clean_all: clean nocore
	make parser libTipPod.a

parser.h parser.cpp: parser.y lexer.h
	bison --report=all -Wall -d -o parser.cpp parser.y

lexer.h lexer.cpp: lexer.l
	flex -F -o lexer.cpp lexer.l
	sed -i 's/\t/    /g' lexer.cpp lexer.h

$(objects): lexer.h parser.h
%.o: %.cpp %.h 
	$(CXX) $(CPPFLAGS) -c $< -o $@

parser: $(objects)
	$(CXX) $(CPPFLAGS) -o $@  $^

TipPod_version.cpp:
	echo 'const char *TipPod_VERSIONTAG = "TipPod_VERSIONTAG SVN TEST_BUILD";' > TipPod_version.cpp

libTipPod.a: $(lib_objects)
	ar rcs $@ $^

.PHONY: clean
clean:
	rm -vf parser.h parser.cpp lexer.cpp lexer.h parser parser.output
	rm -vf $(objects) libTipPod.a

.PHONY: nocore
nocore:
	rm -vf core.[0-9]*
