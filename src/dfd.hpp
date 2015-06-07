#ifndef DFD_HPP
#define DFD_HPP

#include <string>
#include <set>
#include <memory>
#include <utility>
#include <functional>
#include <iostream>

class iserializable {
public:
    virtual void serialize(std::ostream& out) const = 0;
    virtual bool deserialize(std::istream& in) = 0;
    virtual ~iserializable() {}
};

template<class T, class U> class DFD
{
public:
    class node;
    class edge;
    class bad_serialize {
    public:
        typename std::ostream::streampos where;
        bad_serialize(std::ostream::streampos where): where(where) {}
    };

    class bad_node_serialize: public bad_serialize {
    public:
        T* node;
        bad_node_serialize(std::ostream::streampos where, T* node): bad_serialize(where), node(node) {}
    };
    class bad_edge_serialize: public bad_serialize {
    public:
        U* edge;
        bad_edge_serialize(std::ostream::streampos where, U* edge): bad_serialize(where), edge(edge) {}
    };
    class bad_deserialize {
    public:
        std::ostream::streampos where;
        bad_deserialize(std::istream::streampos where): where(where) {}
    };
    class bad_node_deserialize: public bad_deserialize {
    public:
        T* node;
        bad_node_deserialize(std::istream::streampos where, T* node): bad_deserialize(where), node(node) {}
    };
    class bad_edge_deserialize: public bad_deserialize {
    public:
        U* edge;
        bad_edge_deserialize(std::istream::streampos where, U* edge): bad_deserialize(where), edge(edge) {}
    };
    using iterator = typename std::set<std::shared_ptr<DFD::node>>::iterator;
    using const_iterator = typename std::set<std::shared_ptr<DFD::node>>::const_iterator;

    DFD(){}
    node& createNode(T* value) {
        std::shared_ptr<node> _node = std::make_shared<node>(value);
        nodes.insert(_node);
        return *_node;
    }

    edge& linkNodes(node& from, node& to, U* value);
    void serialize(std::ostream& out) const {
        std::set<DFD::node*> serialized_nodes;
        for(auto&& i: nodes) i->serialize(out, serialized_nodes);
    }
    void deserialize(std::istream& in, std::function<T*(node*)> createNode, std::function<U*()> createEdge) {
        while(!in.eof()) {
            std::shared_ptr<node> _node = std::make_shared<node>(nullptr);
            _node->value = createNode(_node.get());
            bool result = _node->deserialize(in);
            if(result) nodes.insert(_node);
            else {
                std::shared_ptr<DFD::edge> _edge = std::make_shared<edge>(nullptr);
                _edge->value = createEdge();
                result = _edge->deserialize(in);
                if(result == false) return; // TODO error
            }
        }
    }

    iterator begin() { return nodes.begin(); }
    const_iterator cbegin() const { return nodes.cbegin(); }
    iterator end() { return nodes.end(); }
    const_iterator cend() const { return nodes.cend(); }
    bool dfs() const {
        std::set<const DFD::node*> checked_nodes;
        std::set<const DFD::node*> visited_nodes;
        for(auto&& i: nodes) if(i->dfs(checked_nodes, visited_nodes)) return true;
        return false;
    }
private:
    std::set<std::shared_ptr<DFD::node>> nodes;
};

template<class T, class U> class DFD<T, U>::node
{
    std::set<std::shared_ptr<DFD::edge>> out_edges;
    std::set<DFD::edge*> in_edges;
public:
    T* value;
    node(T* value): value(value) {}
    std::shared_ptr<DFD::edge> link(DFD::node& other, U* value) {
        std::shared_ptr<DFD::edge> edge = std::make_shared<DFD::edge>(value);
        edge->link(this, &other);
        out_edges.insert(edge);
        other.addInputEdge(edge);
        return edge;
    }
private:
    void addInputEdge(std::shared_ptr<DFD::edge> edge) {
        in_edges.insert(edge.get());
    }
public:
    void serialize(std::ostream& out, std::set<node*>& nodes) {
        typename std::set<node*>::key_type k = this;
        if(nodes.find(k) != nodes.end()) return;
        value->serialize(out);
        nodes.insert(k);
        for(auto&& i: out_edges) i->serialize(out, nodes);
    }
    bool deserialize(std::istream& in) {
        return value->deserialize(in);
    }
    bool dfs(std::set<const node*>& checked_nodes, std::set<const node*>& visited_nodes) const {
        if(checked_nodes.find(this) != checked_nodes.end()) return false;
        if(visited_nodes.find(this) != visited_nodes.end()) return true;
        visited_nodes.insert(this);
        for(auto&& i: out_edges) if(i->end().dfs(checked_nodes, visited_nodes)) return true;
        visited_nodes.erase(this);
        checked_nodes.insert(this);
        return false;
    }
};

template<class T, class U> class DFD<T, U>::edge
{
    DFD::node* left;
    DFD::node* right;
public:
    U* value;
    edge(U* v): value(v) {}
    void link(DFD::node* from, DFD::node* to) {
        left = from;
        right = to;
    }
    DFD::node& start() { return *left; }
    const DFD::node& start() const { return *left; }
    DFD::node& end() { return *right; }
    const DFD::node& end() const { return *right; }
    void serialize(std::ostream& out, std::set<DFD::node*>& nodes) const {
        left->serialize(out, nodes);
        right->serialize(out, nodes);
        value->serialize(out);
    }
    bool deserialize(std::istream& in) {
        return value->deserialize(in);
    }
};

template<class T, class U> typename DFD<T, U>::edge& DFD<T, U>::linkNodes(DFD<T, U>::node& from, DFD<T, U>::node& to, U *value) {
    return *from.link(to, value);
}

#endif // DFD_HPP
