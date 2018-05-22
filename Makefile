all: build test

BIN = pegtl-recursive pegtl-indent spirit-recursive
PERF = perf-pegtl-recursive perf-pegtl-indent perf-spirit-recursive

TEST = test-tree
	
build: $(BIN) $(TEST) $(PERF)

test: build
	./pegtl-recursive
	./pegtl-indent
	./spirit-recursive
	./test-tree

perf: build
	bash -c 'time ./perf-pegtl-recursive'
	bash -c 'time ./perf-pegtl-indent'
	bash -c 'time ./perf-spirit-recursive'

pegtl-recursive: pegtl-recursive.cc node.h pegtl-recursive-grammar.h

pegtl-indent: pegtl-indent.cc node.h pegtl-indent-grammar.h

spirit-recursive: spirit-recursive.cc node.h spirit-recursive-grammar.h

test-tree: test-tree.cc node.h

# performance tests

perf-pegtl-recursive: perf-pegtl-recursive.cc node.h pegtl-recursive-grammar.h perf.h

perf-pegtl-indent: perf-pegtl-indent.cc node.h pegtl-indent-grammar.h perf.h

perf-spirit-recursive: perf-spirit-recursive.cc node.h spirit-recursive-grammar.h perf.h

#all: praser.cc
#    make praser
#    ./praser

CXXFLAGS=-std=c++14 -g -I PEGTL/include/ -I Catch2/

clean:
	rm $(BIN) $(PERF) $(TEST)
