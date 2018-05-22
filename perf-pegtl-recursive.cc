#include "perf.h"
#include "pegtl-recursive-grammar.h"


int main() {

    std::string input = INPUT;

    memory_input<> in(input.data(), input.data() + input.size());

    dynamicIndentState s;

    for (size_t i = 0 ; i <= LOOP_COUNT ; ++i) {
      auto result = parse<itemList, blueprint_actions>(in, s);
      auto rest = in.end() - in.current();
    }
}
