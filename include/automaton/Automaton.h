#pragma once

#include <vector>
#include <string>
#include <deque>
#include <sstream>

template <typename SymbolType>
class EdgesInterface {
public:
    virtual size_t to() const = 0;
    virtual const SymbolType& symbol() const = 0;
};

template <typename VertexType, typename SymbolType>
struct Automaton {
    size_t _size;
    size_t _start_vertex;
    std::vector<size_t> _terminal_vertices;
    std::vector<VertexType> _graph;

    Automaton();
    explicit Automaton(size_t size);
    void set_start(size_t vertex);
    void set_end(size_t vertex);
    void add_edge(size_t from, size_t to, const SymbolType& symbol);
    void add_bidirectional_edge(size_t from, size_t to, const SymbolType& symbol);
    size_t add_vertices(size_t);
    void remove_all_end_vertices();
    size_t merge(const Automaton<VertexType, SymbolType>& other);
    void remove_vertex(size_t);
    void remove_unreachable_from_start_vertices();
    void remove_vertices_unreachable_to_terminal();
    size_t size() const;
    void clear();

    std::string to_string() const;
    std::string to_graphviz() const;

    virtual bool accepts(const std::string& str) = 0;
};

template <typename VertexType, typename SymbolType>
Automaton<VertexType, SymbolType>::Automaton() : _size(1), _start_vertex(0), _graph(1) {
    set_start(0);
    set_end(0);
}

template <typename VertexType, typename SymbolType>
Automaton<VertexType, SymbolType>::Automaton(size_t size) : _size(size), _start_vertex(0), _graph(size) {}

template <typename VertexType, typename SymbolType>
void Automaton<VertexType, SymbolType>::set_start(size_t vertex) {
    _start_vertex = vertex;
}

template <typename VertexType, typename SymbolType>
void Automaton<VertexType, SymbolType>::set_end(size_t vertex) {
    if (!_graph[vertex].is_terminal()) {
        _graph[vertex].set_terminal();
        _terminal_vertices.push_back(vertex);
    }
}

template <typename VertexType, typename SymbolType>
void Automaton<VertexType, SymbolType>::add_edge(size_t from, size_t to, const SymbolType& symbol) {
    _graph[from].add_edge(to, symbol);
}

template <typename VertexType, typename SymbolType>
void Automaton<VertexType, SymbolType>::add_bidirectional_edge(size_t from, size_t to, const SymbolType& symbol) {
    add_edge(from, to, symbol);
    add_edge(to, from, symbol);
}

template <typename VertexType, typename SymbolType>
size_t Automaton<VertexType, SymbolType>::add_vertices(size_t count) {
    _size += count;
    _graph.resize(_size);
    return _size - count;
}

template <typename VertexType, typename SymbolType>
void Automaton<VertexType, SymbolType>::remove_all_end_vertices() {
    while (!_terminal_vertices.empty()) {
        _graph[_terminal_vertices.back()].reset_terminal();
        _terminal_vertices.pop_back();
    }
}

template <typename VertexType, typename SymbolType>
size_t Automaton<VertexType, SymbolType>::merge(const Automaton<VertexType, SymbolType>& other) {
    size_t shift = size();
    add_vertices(other.size());
    for (int i = 0; i < other.size(); ++i) {
        for (auto& edge : other._graph[i].edges()) {
            add_edge(i + shift, edge.to() + shift, edge.symbol());
        }
    }
    return shift;
}

template <typename VertexType, typename SymbolType>
void Automaton<VertexType, SymbolType>::remove_vertex(size_t vertex) {
    for (auto& other : _graph) {
        VertexType new_vertex;
        new_vertex.set_terminal_value(other.is_terminal());
        for (auto& edge : other.edges()) {
            size_t to = edge.to();
            if (to != vertex) {
                if (to > vertex) {
                    --to;
                }
                new_vertex.add_edge(to, edge.symbol());
            }
        }
        other = std::move(new_vertex);
    }

    if (_graph[vertex].is_terminal()) {
        _terminal_vertices.erase(std::find(
                _terminal_vertices.begin(), _terminal_vertices.end(), vertex));
    }
    if (_start_vertex == vertex) {
        _start_vertex = size();
    } else if (_start_vertex > vertex) {
        --_start_vertex;
    }
    for (auto& other : _terminal_vertices) {
        if (other > vertex) {
            --other;
        }
    }
    _graph.erase(_graph.begin() + vertex);
    --_size;
}

template <typename VertexType, typename SymbolType>
void Automaton<VertexType, SymbolType>::remove_unreachable_from_start_vertices() {
    std::vector<char> used(size());
    std::deque<size_t> bfs;
    bfs.push_back(_start_vertex);
    used[_start_vertex] = 1;

    while (!bfs.empty()) {
        size_t cur_v = bfs.front();
        bfs.pop_front();

        for (auto& edge : _graph[cur_v].edges()) {
            if (used[edge.to()]) {
                continue;
            }
            used[edge.to()] = 1;
            bfs.push_back(edge.to());
        }
    }

    for (int vertex = size() - 1; vertex > -1; --vertex) {
        if (!used[vertex]) {
            remove_vertex(vertex);
        }
    }
}

template <typename VertexType, typename SymbolType>
void Automaton<VertexType, SymbolType>::remove_vertices_unreachable_to_terminal() {
    std::vector<char> targets(size());
    for (size_t vertex = 0; vertex < size(); ++vertex) {
        if (_graph[vertex].is_terminal()) {
            continue;
        }
        std::vector<char> used(size());
        std::deque<size_t> bfs;
        bfs.push_back(vertex);
        bool is_reachable = false;

        while (!bfs.empty()) {
            size_t cur_v = bfs.front();
            bfs.pop_front();

            for (auto& edge : _graph[cur_v].edges()) {
                if (used[edge.to()]) {
                    continue;
                }
                if (_graph[edge.to()].is_terminal()) {
                    is_reachable = true;
                    break;
                }
                used[edge.to()] = 1;
                bfs.push_back(edge.to());
            }
            if (is_reachable) {
                break;
            }
        }
        if (!is_reachable) {
            targets[vertex] = 1;
        }
    }

    for (int vertex = size() - 1; vertex > -1; --vertex) {
        if (targets[vertex]) {
            remove_vertex(vertex);
        }
    }
}

template <typename VertexType, typename SymbolType>
size_t Automaton<VertexType, SymbolType>::size() const {
    return _size;
}

template <typename VertexType, typename SymbolType>
void Automaton<VertexType, SymbolType>::clear() {
    _size = 0;
    _start_vertex = 0;
    _terminal_vertices.clear();
    _graph.clear();
}

template <typename VertexType, typename SymbolType>
std::string Automaton<VertexType, SymbolType>::to_string() const {
    std::stringstream ss;
    ss << _start_vertex << "\n\n";
    for (size_t i : _terminal_vertices) {
        ss << i << '\n';
    }
    ss << '\n';
    for (size_t i = 0; i < size(); ++i) {
        for (const auto& edge : _graph[i].edges()) {
            ss << i << ' ' << edge.to() << ' ' << edge.symbol() << '\n';
        }
    }
    return ss.str();
}

template <typename VertexType, typename SymbolType>
std::string Automaton<VertexType, SymbolType>::to_graphviz() const {
    std::stringstream ss;
    ss << "digraph NFA {\n";
    ss << "node [style=\"filled\"];\n";
    for (size_t vertex = 0; vertex < size(); ++vertex) {
        for (auto& edge : _graph[vertex].edges()) {
            ss << "\"" << vertex << "\" -> \"" << edge.to() << "\" [label=\"" << edge.symbol() << "\"];\n";
        }
    }
    ss << "\"" << _start_vertex << "\" [fillcolor=\"" << (_graph[_start_vertex].is_terminal() ? "yellow" : "green") << "\"];\n";
    for (auto vertex : _terminal_vertices) {
        if (vertex == _start_vertex) {
            continue;
        }
        ss << "\"" << vertex << "\" [fillcolor=\"red\"];\n";
    }
    ss << "}\n";
    return ss.str();
}