#define CATCH_CONFIG_MAIN 
#include "single_include/catch.hpp"

#include "spirit-recursive-grammar.h"

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

std::ostream& operator<<(std::ostream& o, Tree* tree) {
    //ostHelper h { &tree, 0 };
    if (!tree) {
      o << "null";
      return o;
    }

    o << (void *)tree << ">>" << tree->get().content << "<<";
    for(auto const& m : *tree) {
        o << m->get().content << ",";
    }
    return o;
}

std::ostream& operator<<(std::ostream& o, const std::list<Tree*>& tree) {
    //ostHelper h { &tree, 0 };
    //o << (void *)tree << ">>" << tree->get().content << "<<";
    for(auto const& m : tree) {
        o << m << ",";
    }
    //o << "." << tree->get().content << "<<";
    return o;
}


template <typename P>
bool test_parser(P const& p, char const* input) {
    using boost::spirit::qi::parse;
    char const* f(input);
    char const* l(f + strlen(f));
    return parse(f, l, p);
}

template <typename P, typename Attr>
bool test_parser(P const& p, char const* input, Attr const& expected) {
    using boost::spirit::qi::parse;
    char const* f(input);
    char const* l(f + strlen(f));
    Attr attr;
    return parse(f, l, p, attr) && f == l && attr == expected;
}

TEST_CASE("tree") {
    REQUIRE(test_parser(listGrammar<const char*>(), "* a\n    * b\n        * c\n            * d\n    * e"));
}

