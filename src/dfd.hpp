#ifndef DFD_HPP
#define DFD_HPP

#include <string>
#include <map>
#include <memory>

template<class T, class U> class DFD
{
public:
    class node;
    class edge;

    DFD(){}
    node& createNode(const std::string name, const T& value);
    edge& linkNodes(const node& from, const node& to, const std::string& name, const U& value);
private:
    std::map<std::string, std::shared_ptr<DFD::node>> nodes;
    std::map<std::string, std::shared_ptr<DFD::edge>> edges;
};

template<class T, class U> class DFD<T, U>::node
{
    int id;
    std::string name;
    T value;
    std::multimap<std::string, std::shared_ptr<DFD::edge>> out_edges;
    std::multimap<std::string, std::weak_ptr<DFD::edge>> in_edges;
public:
    node(const std::string& name, const T& value): name(name), value(value)
    {}
    std::shared_ptr<DFD::edge> link(std::shared_ptr<DFD::node> other, const std::string& name, const U& value) {
        std::shared_ptr<DFD::edge> edge = std::make_shared(name, value);
        out_edges.insert({name, value});
        other->addInputEdge(edge);
        return edge;
    }
    void addInputEdge(std::shared_ptr<DFD::edge> edge) {
        std::weak_ptr<DFD::edge> in = edge;
        in_edges.insert({edge.getName(), in});
    }
};

template<class T, class U> class DFD<T, U>::edge
{
    std::string name;
    U value;
    DFD::node* left;
    std::weak_ptr<DFD::node> right;
public:
    edge(const std::string& name, const U& value): name(name), value(value)
    {}
    void link(DFD::node* from, std::shared_ptr<DFD::node> to) {
        left = from;
        right = to;
    }
    const std::string& getName() const {
        return name;
    }
};

#endif // DFD_HPP

