#ifndef _PRASER_NODE_H_
#define _PRASER_NODE_H_

#include <list>
#include <memory>
#include <algorithm>

template <typename Data>
class node {

public:
    using Type = node<Data>;
    using Container = std::list<std::unique_ptr<Type>>;
    using ContainerIterator = typename Container::iterator;

private:
    Data data_;
    Type* parent_;
    Container children_;

public:

    node(Data&& data, Type* parent = nullptr) : data_(std::move(data)), parent_(parent) {}
    node(Type* parent = nullptr) : parent_(parent) {}

    node(std::list<Type*>& container) {
        std::transform(container.begin(), container.end()
                ,std::back_inserter(children_)
                ,[this](auto child){ child->parent_ = this; return std::unique_ptr<Type>(child); }
        );
    }

    node(const std::string& content) : data_({content}) {}

    using iterator = ContainerIterator;

    const Data& get() const noexcept { return data_; }
    void set(Data&& data) noexcept { data_ = std::move(data); }

    bool isRoot() const noexcept { return parent_ == nullptr; }
    bool hasChildren() const noexcept { return !children_.empty(); }

    Type* getParent() noexcept { return isRoot() ? this : parent_; }

    //iterator addChild(Data&& content) {
    Type* addChild(Data&& data) {
        return children_.emplace(children_.end(), new Type(std::move(data), this))->get();
    }

    Type* addChild() {
        return children_.emplace(children_.end(), new Type(this))->get();
    }

    iterator begin() { return children_.begin(); }
    iterator end() { return children_.end(); }

    size_t size() const { return children_.size(); }

};

#endif // #ifndef _PRASER_NODE_H_
