#ifndef _PEGTL_RECURSIVE_GRAMMAR_H_
#define _PEGTL_RECURSIVE_GRAMMAR_H_

#include "tao/pegtl.hpp"

#include "node.h"
#include <stack>

using namespace tao::pegtl;

struct blankLine : until<eolf, blank> {};
struct indent : sor< one<'\t'>, rep<4, one<' '>>> {};
struct nonIndentedSpace : rep_max<3, one<' '>> {};

struct dynamicIndentState {
    std::stack<std::string> indent;
};

struct dynamicIndentMatch; // see below

struct indentedBlankLine : seq<dynamicIndentMatch, blankLine> {};

struct bullet : seq<nonIndentedSpace, one<'+','*','-'>, plus<blank>> {};

struct listItemContent : until<eolf> {};

struct listItemContinuation;

struct listItem : seq<bullet, listItemContent, star<listItemContinuation>> {};

struct subList : list<listItem, dynamicIndentMatch> {};

struct oneMoreIndent : seq<dynamicIndentMatch, indent> {};
struct listItemContinuation : seq<oneMoreIndent, subList> {};

struct itemList : subList {};

template <typename Rule>
struct blueprint_actions : nothing<Rule>{
};

template<>
struct blueprint_actions<oneMoreIndent> {
    template< typename Input >
    static void apply(const Input& in, dynamicIndentState& state) {
        //std::cout << "PI: >" << state.indent << "<" << std::endl;
        state.indent.push(in.string());
        //std::cout << "CI: >" << state.indent << "<" << std::endl;
    }
};

template<>
struct blueprint_actions<subList> {
    template< typename Input >
    static void apply(const Input& in, dynamicIndentState& state) {
        state.indent.pop();
    }
};

template<>
struct blueprint_actions<itemList> {
    template< typename Input >
    static void apply(const Input& in, dynamicIndentState& state) {
        //state.indent = "";
    }
};

/**
 * FIXME: decide if it is better use Action or Match implementation
 *
 * Action (in general) is not prefered solution for matching rule (though threre is machanism to do it)
 *
 * From PEGTL manual:
 *  Note that actions returning bool are an advanced use case that should be used with caution. 
 *  They prevent some internal optimisations, in particular when used with apply0(). 
 *  They can also have weird effects on the semantics of a parsing run, 
 *  for example at< rule > can succeed for the same input for which rule fails 
 *  when there is a bool-action attached to rule that returns false 
 *  (remembering that actions are disabled within an at<> combinator).
 *
 *  Rule:match() seems to be better solution for, but it have some disadvantages.
 *
 *  Main problem is, you need to write own match and it mean, you cannot use same particulat check for rule by inheritance. e.g.
 *
 * ```
 *  struct dynamicIndentMatch : star<indent> {
 *     // match impl
 *     ... match()...
 *  }
 * ```
 *
 * will not check for rule `star<indent>` but this happen while Action::apply0 is invoked
 * there is workaround to invoke it internaly for rule you wold like to inherit from.
 * Rule:match() is static function so you can invoke match inside your own matching 
 *
 */

struct dynamicIndentMatch /* : star<indent> */ {
    template< 
        tao::pegtl::apply_mode A,
        tao::pegtl::rewind_mode M,
        template< typename... > class Action,
        template< typename... > class Control,
        typename Input >
        static bool match( Input& in,
                const dynamicIndentState& state)
        {

            //std::cout << "MI: >" << state.indent << "<" << std::endl;
            //std::cout << "in(" << in.size() << "): >" << std::string(in.current(), in.end()) << "<" << std::endl;
            const std::string empty;
            const std::string& indent = state.indent.empty()
                ? empty
                : state.indent.top();


            if( in.size(indent.size() ) >= indent.size() ) {
                if( std::memcmp( in.current(), indent.data(), indent.size() ) == 0 ) {
                    in.bump( indent.size() );
                    return true;
                }
            }
            return false;
        }
};


/*
template<>
struct blueprint_actions<dynamicIndentMatch> {
    template< typename Input >
    static bool apply(const Input& in, dynamicIndentState& state) {
        if (in.size() == state.indent.size()) {
            return memcmp(in.begin(), state.indent.data(), in.size()) == 0;
        }
        return false;
    }
};
*/

#endif // #ifndef _PEGTL_RECURSIVE_GRAMMAR_H_
