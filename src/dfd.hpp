#ifndef DFD_HPP
#define DFD_HPP

#include <string>
#include <map>
#include <memory>
#include <utility>

template<class T, class U> class DFD
{
public:
    class node;
    class edge;

    DFD(){}
    template<class V> node& createNode(const std::string name, V&& value) {
        std::shared_ptr<node> node = std::make_shared(name, std::forward<V>(value));
        nodes.insert({name, node});
        return *node;
    }

    edge& linkNodes(const node& from, const node& to, const std::string& name, const U& value);

private:
    std::multimap<std::string, std::shared_ptr<DFD::node>> nodes;
};

template<class T, class U> class DFD<T, U>::node
{
    friend class DFD<T, U>;
    std::string _name;
    T value;
    std::multimap<std::string, std::shared_ptr<DFD::edge>> out_edges;
    std::multimap<std::string, std::weak_ptr<DFD::edge>> in_edges;
private:
    template<class V> node(const std::string& name, V&& value): _name(name), value(std::forward<V>(value))
    {}
public:
    std::shared_ptr<DFD::edge> link(const DFD::node& other, const std::string& name, const U& value) {
        std::shared_ptr<DFD::edge> edge = std::make_shared(name, value);
        edge->link(this, other);
        out_edges.insert({name, value});
        other->addInputEdge(edge);
        return edge;
    }
    const std::string& name = _name;
    const T& getValue() const { return value; }
    T& getValue() { return value; }
private:
    void addInputEdge(std::shared_ptr<DFD::edge> edge) {
        std::weak_ptr<DFD::edge> in = edge;
        in_edges.insert({edge.getName(), in});
    }
};

template<class T, class U> class DFD<T, U>::edge
{
    friend class DFD<T, U>;
    std::string _name;
    U value;
    DFD::node* left;
    DFD::node* right;
private:
    template<class V> edge(const std::string& name, V&& value): _name(name), value(std::forward<V>(value))
    {}
public:
    void link(DFD::node* from, DFD::node* to) {
        left = from;
        right = to;
    }
    const std::string& name = _name;
    const U& getValue() const { return value; }
    U& getValue() { return value; }
    DFD::node& start() { return *left; }
    const DFD::node& start() const { return *left; }
    DFD::node& end() { return *right; }
    const DFD::node& end() const { return *right; }
};

template<class T, class U> typename DFD<T, U>::edge& DFD<T, U>::linkNodes(const DFD<T, U>::node& from, const DFD<T, U>::node& to, const std::string& name, const U& value) {
    return *from.link(to, name, value);
}

#endif // DFD_HPP
