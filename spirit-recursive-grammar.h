#ifndef _SPIRIT_RECURSIVE_GRAMMAR_H_
#define _SPIRIT_RECURSIVE_GRAMMAR_H_

// NOTE: this grammar is extracted from
// https://github.com/misuo/spirit-markdown.git

#include <list>
#include <memory>

#include <boost/spirit/include/qi.hpp>

#include <boost/spirit/include/phoenix_core.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>
#include <boost/spirit/include/phoenix_object.hpp>
#include <boost/spirit/include/phoenix_fusion.hpp>
#include <boost/spirit/include/phoenix_stl.hpp>

#include <boost/phoenix/statement/if.hpp>
#include <boost/phoenix/statement/sequence.hpp>
#include <boost/phoenix/stl/container.hpp>
#include <boost/phoenix/object.hpp>
#include <boost/phoenix/bind.hpp>
#include <boost/phoenix/stl/algorithm.hpp>
#include <boost/phoenix/core/argument.hpp>

#include "node.h"

using namespace boost::spirit;

struct TreeData {
    std::string content;
};

using Tree = node<TreeData>;

template <typename Iterator>
struct listGrammar : qi::grammar<Iterator, Tree::Type*()> {

    listGrammar() : listGrammar::base_type(start) {

        using boost::phoenix::ref;
        using boost::phoenix::push_back;
        using boost::phoenix::empty;
        using boost::phoenix::clear;
        using boost::phoenix::construct;
        using boost::phoenix::new_;
        using boost::phoenix::bind;
        using boost::phoenix::for_each;

        static const std::string nl("\n");

        blankLine = *qi::blank >> eol;
        indent %= qi::string("\t") | repeat(4)[qi::char_(' ')];
        nonIndentSpace %= repeat(0, 3)[qi::char_(' ')];
        //indentedBlankLine = lit(_r1) >> blankLine;

        bullet = 
            nonIndentSpace
            >> qi::char_("+*-")
            >> +qi::blank
            ;

        listItemContents =
            !blankLine
            >> +(qi::char_ - eol)[ push_back(_val, _1) ]
            >> eol [ _a = boost::phoenix::ref(nl) ]
#if 0 // continuation paragraph
          >> *(
                -lit(_r1) 
                >> !(indent | bullet | blankLine)
                >> +(qi::char_ - eol) /*[
          //        if_(!empty(_a)) [
          //            push_back(_val, _a),
          //            clear(_a)
          //            ],
          //        push_back(_val, _1) ]*/
                >> eol [ _a = boost::phoenix::ref(nl) ]
              )
#endif
             ;
        listItemContents.name("listItemContents");

        listItemContinuation =
            omit[
                //*( lit(_r1) >> blankLine )
                //>> 
                lit(_r1) [ _a = _r1 ]
                >> indent [ _a += _1 ]
            ]
            >> subList(_a)
            ;

        listItemContinuation.name("listItemContinuation");

        listItem = 
            bullet
            >> listItemContents(_r1)[ _val = new_<Tree>(_1) ]
            >> *(listItemContinuation(_r1))
            ;

        listItem.name("listItem");

        subList = 
            eps[ _a = true ]
            >> listItem(_r1) [ push_back(_b, _1) ]
              % (/*-indentedBlankLine(_r1)[ _a = false ] >> */ lit(_r1))
            >> attr(_a)[ 
                if_(!empty(_b)) [
                  _val = new_<Tree>(_b),
                  clear(_b)
                ]
            ]
          ;

        subList.name("subList");
 
        start = 
            subList(std::string()) [ _val = _1 ]
            ;

        start.name("start");

        //debug(listItemContents);
        //debug(listItemContinuation);
        //debug(listItem);
        //debug(subList);
        //debug(start);
    }

    qi::rule<Iterator, Tree::Type*()> start;

    qi::rule<Iterator, Tree::Type*(std::string), locals<bool, std::list<Tree*> >> subList;
    qi::rule<Iterator, Tree::Type*(std::string)> listItem;

    qi::rule<Iterator, std::string(std::string), locals<std::string>> listItemContents;
    qi::rule<Iterator, void(std::string), locals<std::string>> listItemContinuation;

    qi::rule<Iterator> bullet;

    //qi::rule<Iterator, void(std::string)> indentedBlankLine;
    qi::rule<Iterator, std::string()> nonIndentSpace;
    qi::rule<Iterator> blankLine;
    qi::rule<Iterator, std::string()> indent;
};



#endif // #ifdef _SPIRIT_RECURSIVE_GRAMMAR_H_
