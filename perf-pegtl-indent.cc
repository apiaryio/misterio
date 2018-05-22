#include "perf.h"
#include "pegtl-indent-grammar.h"


int main() {

    std::string input = INPUT;

    memory_input<> in(input.data(), input.data() + input.size());

    ParsingState state;

    for (size_t i = 0 ; i <= LOOP_COUNT ; ++i) {
      bool result = parse<file, blueprint_actions>(in, state);
      auto rest = in.end() - in.current();
    }
}
