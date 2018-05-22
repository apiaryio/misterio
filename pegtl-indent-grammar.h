#ifndef _PEGTL_INDENT_GRAMMAR_H_
#define _PEGTL_INDENT_GRAMMAR_H_

#include "tao/pegtl.hpp"
#include "node.h"

#include <stack>
using namespace tao::pegtl;

struct TreeData {
    std::string content;
    size_t indent;
};

using Tree = node<TreeData>;

struct ParsingState {
    ParsingState() : indent(0), result(Tree()), current(&result) {
    }

    size_t getLastIndent() {
        return indentStack.empty()
            ? 0
            : indentStack.top();
    }

    int getIndentLevel() {
        //size_t prev = getLastIndent(s.indentStack);

        size_t prev = current->get().indent;
        int pos = indent - prev;
        int level = 0;

        if (pos >= 2) {
            level = 1;
            indentStack.push(indent);
        }
        else if (indent < prev) {
            while (!indentStack.empty() && indent < prev) {
                indentStack.pop();
                prev = getLastIndent();
                level -= 1;
            }
        }

        return level;
    }

    size_t indent;
    std::string text;
    std::stack<size_t> indentStack;
    Tree result;
    Tree* current;
};

struct indent : seq<bol, star<blank>> {};

struct WS : plus<blank> {};

struct bullet : one<'*', '+', '-'> {};

struct rest : plus<not_at<eol>, any> {};

struct text : seq<rest> {};

struct listItem : sor<
                    seq<bullet, WS, opt<rest>>,
                    seq<bullet, at<eolf>>
                  > {};

struct content : sor<
                   listItem,
                   text
                 > {};

struct line : seq<indent, opt<content>, eolf> {};

struct file : until<eof, line> {};

template <typename Rule>
struct blueprint_actions : nothing<Rule>{
};

template<>
struct blueprint_actions<indent> {
    template< typename Input >
    static void apply(const Input& in, ParsingState& state) {
        state.indent = in.size();
    }
};

template<>
struct blueprint_actions<rest> {
    template< typename Input >
    static void apply(const Input& in, ParsingState& state) {
        state.text = in.string();
    }
};

template<>
struct blueprint_actions<text> {
    template< typename Input >
    static void apply(const Input& in, ParsingState& state) {
        state.current = state.current->getParent()->addChild();
    }
};


template<>
struct blueprint_actions<listItem> {
    template< typename Input >
    static void apply(const Input& in, ParsingState& state) {
        int level = state.getIndentLevel();

        if (level == 1) {
            state.current = state.current->addChild();
        }
        else if (level <= 0) {

            while (level < 0) {
              state.current = state.current->getParent();
              level++;
            }
            state.current = state.current->getParent()->addChild();
        }

    }
};

template<>
struct blueprint_actions<line> {
    template< typename Input >
    static void apply(const Input& in, ParsingState& state) {
        state.current->set({state.text, state.indent});
    }
};



template <typename Rule>
std::pair<bool, size_t> verify(const std::string& input) {
    memory_input<> in(input.data(), input.data() + input.size());

    auto result = parse<Rule>(in);
    auto rest = in.end() - in.current();

    return std::make_pair(result, rest);
}


#endif // #ifndef _PEGTL_INDENT_GRAMMAR_H_
