#include "perf.h"
#include "spirit-recursive-grammar.h"


int main() {

    char const* eoi = INPUT + strlen(INPUT);
    using boost::spirit::qi::parse;
    listGrammar<const char*> g;

    for (size_t i = 0 ; i <= LOOP_COUNT ; ++i) {
        const char* b = INPUT;
        const char* e = eoi;
        parse(b, e, g);
    }
}
