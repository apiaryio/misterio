#define CATCH_CONFIG_MAIN 
#include "single_include/catch.hpp"

#include "node.h"

TEST_CASE( "tree", "[tree]" ){

    struct X {
        std::string content;
    };

    GIVEN("tree") {
        node<X> tree;

        REQUIRE(tree.isRoot());
        REQUIRE(!tree.hasChildren());
        REQUIRE(tree.getParent() == &tree);

        WHEN("add child") {
          auto child = tree.addChild({"first"});

          THEN("tree has children") {
              REQUIRE(tree.hasChildren());
              REQUIRE(tree.size() == 1);
          }

          THEN("child has no children") {
              REQUIRE(!child->hasChildren());
          }

          THEN("child is not root") {
              REQUIRE(!child->isRoot());
          }

          GIVEN("iterator from tree begin()") {
              auto it = tree.begin();

              THEN("begin is equal to children") {
                  REQUIRE(it->get() == child);
              }

              WHEN("move iterator forward") {
                  ++it;

                  THEN("iterator equals to tree.end()") {
                      REQUIRE(it == tree.end());
                  }
              }
          }

          WHEN("add sub child") {
              child->addChild({"second.first"});

              THEN("child has one child") {
                  REQUIRE(child->size() == 1);
              }


              THEN("size of tree is unchanged") {
                  REQUIRE(tree.size() == 1);
              }
          }

          WHEN("add sibling of child via parent") {
              child->getParent()->addChild({"second"});


              THEN("tree has two children") {
                  REQUIRE(tree.size() == 2);
              }

              THEN("last member of tree is 'second')") {
                  REQUIRE((*(--tree.end()))->get().content == "second");
              }
          }

          WHEN("add another sibling via parent") {
              auto sibling = child->getParent()->addChild();
              sibling->set({"tres"});
              //child->getParent()->addChild({"tres"});
              THEN("child has no children") {
                  REQUIRE(!child->hasChildren());
              }

              THEN("tree has three children") {
                  //REQUIRE(tree.size() == 3);
              }

              THEN("last member of tree is 'tres')") {
                  REQUIRE((*(--tree.end()))->get().content == "tres");
              }
          }
       }
    }
}

