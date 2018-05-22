#define CATCH_CONFIG_MAIN 
#include "single_include/catch.hpp"

#include "tao/pegtl/contrib/tracer.hpp"
#include "pegtl-recursive-grammar.h"

template <typename Rule, template <typename ...> typename Action = nothing>
std::pair<bool, size_t> verify(const std::string& input) {
    memory_input<> in(input.data(), input.data() + input.size());

    auto result = parse<Rule, Action>(in);
    auto rest = in.end() - in.current();

    //std::cout << result << ", " << rest << std::endl;

    return std::make_pair(result, rest);
}

struct TreeData {
    std::string content;
    size_t indent;
};

using Tree = node<TreeData>;

using R = std::pair<bool, size_t>;


TEST_CASE("recognize blankLine") {
    REQUIRE(verify<blankLine>("") == R(true, 0));
    REQUIRE(verify<blankLine>("     ") == R(true, 0));
    REQUIRE(verify<blankLine>("   x  ") == R(false, 6));
}


TEST_CASE("indent") {
    REQUIRE(verify<indent>("\t") == R(true, 0));
    REQUIRE(verify<indent>("\txx") == R(true, 2));
    REQUIRE(verify<indent>("    ") == R(true, 0));
    REQUIRE(verify<indent>("     ") == R(true, 1));
    REQUIRE(verify<indent>("         ") == R(true, 5));
}


TEST_CASE("nonindented") {
    REQUIRE(verify<nonIndentedSpace>("") == R(true, 0));
    REQUIRE(verify<nonIndentedSpace>(" ") == R(true, 0));
    REQUIRE(verify<nonIndentedSpace>("  ") == R(true, 0));
    REQUIRE(verify<nonIndentedSpace>("   ") == R(true, 0));
    REQUIRE(verify<nonIndentedSpace>("    ") == R(false, 4));
}


TEST_CASE("dynamicIndentMatch") {
    //REQUIRE(verify<dynamicIndentMatch, blueprint_actions>("") == R(true, 0));
    std::string input("  ");

    memory_input<> in(input.data(), input.data() + input.size());

    dynamicIndentState s;
    s.indent.push("  ");

    auto result = parse<dynamicIndentMatch, blueprint_actions>(in, s);
    auto rest = in.end() - in.current();

    REQUIRE(result);
    REQUIRE(rest == 0);
}



TEST_CASE("bullet") {
    REQUIRE(verify<bullet>("") == R(false, 0));
    REQUIRE(verify<bullet>("* ") == R(true, 0));
    REQUIRE(verify<bullet>("*") == R(false, 1));
    REQUIRE(verify<bullet>(" *x") == R(false, 3));
    REQUIRE(verify<bullet>("*   xxx") == R(true, 3));
    REQUIRE(verify<bullet>(" * a") == R(true, 1));
    REQUIRE(verify<bullet>("  * a") == R(true, 1));
    REQUIRE(verify<bullet>("   * a") == R(true, 1));
    REQUIRE(verify<bullet>("    * a") == R(false, 7));
}


TEST_CASE("listItem") {
    //REQUIRE(verify<listItem>("") == R(false, 0));
    //REQUIRE(verify<listItem>("*") == R(false, 1));
    //REQUIRE(verify<listItem>("* ") == R(true, 0));
    //REQUIRE(verify<listItem>("* a") == R(true, 0));
    //REQUIRE(verify<listItem>("* a\n") == R(true, 0));
    //REQUIRE(verify<listItem>("   * a\n") == R(true, 0));
    //REQUIRE(verify<listItem>("* a\nb") == R(true, 1));
}

TEST_CASE("itemlist") {
    std::string input("* a\n* b\n* c");

    memory_input<> in(input.data(), input.data() + input.size());

    dynamicIndentState s;

    auto result = parse<itemList, blueprint_actions /*, tracer*/>(in, s);
    auto rest = in.end() - in.current();

    REQUIRE(result);
    REQUIRE(rest == 0);
}

TEST_CASE("subList indented") {
    std::string input("    * a\n    * b\n      * c");

    memory_input<> in(input.data(), input.data() + input.size());

    dynamicIndentState s;
    s.indent.push("    ");

    auto result = parse<seq<dynamicIndentMatch, subList>, blueprint_actions /*, tracer */>(in, s);
    auto rest = in.end() - in.current();

    REQUIRE(result);
    REQUIRE(rest == 0);
}

TEST_CASE("subList indented + dedent") {
    std::string input("    * a\n    * b\n* c");

    memory_input<> in(input.data(), input.data() + input.size());

    dynamicIndentState s;
    s.indent.push("    ");

    auto result = parse<seq<dynamicIndentMatch, subList>, blueprint_actions /*, tracer */>(in, s);
    auto rest = in.end() - in.current();

    REQUIRE(result);
    REQUIRE(rest == 3); // dedented sublist - so we close previous
}


TEST_CASE("oneMoreIndent 1st level") {
    std::string input("    * a");

    memory_input<> in(input.data(), input.data() + input.size());

    dynamicIndentState s;

    auto result = parse<oneMoreIndent, blueprint_actions/*, tracer */>(in, s);
    auto rest = in.end() - in.current();

    REQUIRE(result);
    REQUIRE(rest == 3); 
    REQUIRE(s.indent.top().size() == 4); 
    REQUIRE(s.indent.top() == "    "); 
}

TEST_CASE("oneMoreIndent 2nd level") {
    std::string input("\t\t* a");

    memory_input<> in(input.data(), input.data() + input.size());

    dynamicIndentState s;
    s.indent.push("\t");

    auto result = parse<oneMoreIndent, blueprint_actions/*, tracer */>(in, s);
    auto rest = in.end() - in.current();

    REQUIRE(result);
    REQUIRE(rest == 3); 
    REQUIRE(s.indent.size() == 2); 
    REQUIRE(s.indent.top() == "\t\t"); 
}

TEST_CASE("oneMoreIndent overflow") {
    std::string input("\t\t* a");

    memory_input<> in(input.data(), input.data() + input.size());

    dynamicIndentState s;

    auto result = parse<oneMoreIndent, blueprint_actions /*, tracer */>(in, s);
    auto rest = in.end() - in.current();

    REQUIRE(result);
    REQUIRE(rest == 4); 
    REQUIRE(s.indent.size() == 1); 
    REQUIRE(s.indent.top() == "\t"); 
}


TEST_CASE("listItemContinuation") {
    std::string input("\t* a\n\t* b");

    memory_input<> in(input.data(), input.data() + input.size());

    dynamicIndentState s;

    auto result = parse<listItemContinuation, blueprint_actions/* ,tracer*/ >(in, s);
    auto rest = in.end() - in.current();

    REQUIRE(result);
    REQUIRE(rest == 0);
}

TEST_CASE("item list - upstair") {
    std::string input("* a\n    * b\n        * c");

    memory_input<> in(input.data(), input.data() + input.size());

    dynamicIndentState s;

    auto result = parse<itemList, blueprint_actions/* ,tracer*/>(in, s);
    auto rest = in.end() - in.current();

    REQUIRE(result);
    REQUIRE(rest == 0);
}

TEST_CASE("item list - up and down") {
    std::string input("* a\n    * b\n        * c\n            * d\n    * e");

    memory_input<> in(input.data(), input.data() + input.size());

    dynamicIndentState s;

    auto result = parse<itemList, blueprint_actions/* ,tracer*/>(in, s);
    auto rest = in.end() - in.current();

    REQUIRE(result);
    REQUIRE(rest == 0);
}

TEST_CASE("item list - one by one") {
    std::string input("* a\n * b\n  * c\n   * d\n    * e\n     * f");

    memory_input<> in(input.data(), input.data() + input.size());

    dynamicIndentState s;

    auto result = parse<itemList, blueprint_actions/* ,tracer*/>(in, s);
    auto rest = in.end() - in.current();

    REQUIRE(result);
    REQUIRE(rest == 0);
}



