#define CATCH_CONFIG_MAIN 
#include "single_include/catch.hpp"

//#include "tao/pegtl/contrib/tracer.hpp"

#include "pegtl-indent-grammar.h"

using R = std::pair<bool, size_t>;

//std::ostream& operator<< (std::ostream& stream, const R& result) {
//    stream << result.first;
//    stream << result.second;
//    return stream;
//}

TEST_CASE("recognize list item") {
    REQUIRE(verify<listItem>("a") == R(false, 1));
    REQUIRE(verify<listItem>("*a") == R(false, 2));

    REQUIRE(verify<listItem>("* a") == R(true, 0));
    REQUIRE(verify<listItem>("*   a") == R(true, 0));
    REQUIRE(verify<listItem>("*   xxx  ") == R(true, 0));

    REQUIRE(verify<listItem>("+   a\n") == R(true, 1));
    REQUIRE(verify<listItem>("+\n") == R(true, 1));
}



struct ostHelper {
    Tree* tree;
    size_t indent;
};

std::ostream& operator<<(std::ostream& o, ostHelper& h) {
    o << h.tree->get().content << std::endl;
    if (!h.tree->hasChildren()) {
        return o;
    }

    ostHelper x { nullptr, h.indent + 1 };

    for (auto const& c : *h.tree) {
        x.tree = c.get();
        std::string pad(x.indent, '.');
        o << pad << x;
    }


    return o;
}

std::ostream& operator<<(std::ostream& o, Tree& tree) {
    ostHelper h { &tree, 0 };
    o << h << std::endl;
    return o;
}



TEST_CASE("indented text", "[parse]") {
    std::string input = "a\n  be\n    c \n  d\n  e\nf";

    ParsingState state;

    memory_input<> in(input.data(), input.data() + input.size());
    bool result = parse<file, blueprint_actions>(in, state);

    REQUIRE(state.result.size() == 6);

    //std::cout << state.result << std::endl;
}

TEST_CASE("different bullets list item", "[parse]") {
    std::string input = "* a\n+ b\n- c";

    ParsingState state;

    memory_input<> in(input.data(), input.data() + input.size());
    bool result = parse<file, blueprint_actions>(in, state);

    REQUIRE(state.result.size() == 3);

    //std::cout << state.result << std::endl;
}

TEST_CASE("basic tree up", "[parse]") {
    std::string input = "* a\n  + b\n  - c\n      - d";

    ParsingState state;

    memory_input<> in(input.data(), input.data() + input.size());
    bool result = parse<file, blueprint_actions>(in, state);

    REQUIRE(state.result.size() == 1);

    auto it = state.result.begin();
    REQUIRE((*it)->size() == 2);

    it = (*it)->begin();
    REQUIRE((*it)->size() == 0);

    it++;
    REQUIRE((*it)->size() == 1);

    //std::cout << state.result << std::endl;
}

TEST_CASE("tree up and down", "[parse]") {
    std::string input = 
R"(* a
  + b
    - c
      - d
      - e
    + f
+ g)";

    ParsingState state;

    memory_input<> in(input.data(), input.data() + input.size());
    bool result = parse<file, blueprint_actions>(in, state);

    REQUIRE(state.result.size() == 2); // a,g
    REQUIRE((*state.result.begin())->size() == 1); // b
    auto cItem = (*(*(*state.result.begin())->begin())->begin()).get();
    REQUIRE(cItem->size() == 2);
    REQUIRE(cItem->get().content == "c");
    REQUIRE(cItem->get().indent == 4);

    //std::cout << state.result << std::endl;
}

TEST_CASE("not enough space to be recognized as indent", "[parse]") {
    std::string input = 
R"(* a
 + b
  - c
   - d
    - e
     + f
      + g)";

    ParsingState state;

    memory_input<> in(input.data(), input.data() + input.size());
    bool result = parse<file, blueprint_actions>(in, state);

    REQUIRE(state.result.size() == 7); // a,g
    //REQUIRE((*state.result.begin())->size() == 1); // b
    //auto cItem = (*(*(*state.result.begin())->begin())->begin()).get();
    //REQUIRE(cItem->size() == 2);
    //REQUIRE(cItem->get().content == "c");
    //REQUIRE(cItem->get().indent == 4);

    //std::cout << state.result << std::endl;
}
