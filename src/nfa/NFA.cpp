#include <iostream>
#include <string>
#include <sstream>
#include <tuple>
#include <deque>
#include <cstring>
#include "NFA.h"

bool NFA::Edge::operator<(const NFA::Edge& edge) const {
    return tie(to, symbol) < tie(edge.to, symbol);
}

bool NFA::Edge::operator==(const NFA::Edge& edge) const {
    return (to == edge.to) && (symbol == edge.symbol);
}

NFA::NFA() : _size(1), graph(1), start_vertex(0) {
    set_end(0);
}
NFA::NFA(size_t size) : _size(size), graph(size) {}

void NFA::set_start(size_t vertex) {
    start_vertex = vertex;
}

void NFA::set_end(size_t vertex) {
    if (!graph[vertex].is_terminal) {
        graph[vertex].is_terminal = true;
        terminal_vertices.push_back(vertex);
    }
}

void NFA::add_edge(size_t from, size_t to, const std::string& symbol) {
    graph[from].edges.emplace_back(to, symbol);
}

void NFA::add_bidirectional_edge(size_t from, size_t to, const std::string& symbol) {
    add_edge(from, to, symbol);
    add_edge(to, from, symbol);
}

size_t NFA::add_vertices(size_t count) {
    _size += count;
    graph.resize(_size);
    return _size - count;
}

void NFA::remove_all_end_vertices() {
    while (!terminal_vertices.empty()) {
        graph[terminal_vertices.back()].is_terminal = false;
        terminal_vertices.pop_back();
    }
}

size_t NFA::merge(const NFA& other) {
    size_t shift = size();
    add_vertices(other.size());
    for (int i = 0; i < other.size(); ++i) {
        for (auto& edge : other.graph[i].edges) {
            graph[i + shift].edges.emplace_back(edge.to + shift, edge.symbol);
        }
    }
    return shift;
}

std::string NFA::to_string() const {
    std::stringstream ss;
    ss << start_vertex << "\n\n";
    for (size_t i : terminal_vertices) {
        ss << i << '\n';
    }
    ss << '\n';
    for (size_t i = 0; i < size(); ++i) {
        for (auto& edge : graph[i].edges) {
            ss << i << ' ' << edge.to << ' ' << edge.symbol << '\n';
        }
    }
    return ss.str();
}

bool NFA::accepts(const std::string& str) {
    size_t max_depth = std::max(static_cast<size_t>(str.size() * MAX_SEARCH_MULTIPLIER),
                             static_cast<size_t>(MAX_SEARCH_MULTIPLIER));
    std::vector< std::pair<int, size_t> > states, next_states;
    states.emplace_back(start_vertex, 0);
    for (size_t depth = 0; depth < max_depth; ++depth) {
        if (states.empty()) {
            break;
        }

        while (!states.empty()) {
            auto [vertex, pos] = states.back();
            states.pop_back();

            if ((pos == str.size()) && graph[vertex].is_terminal) {
                return true;
            }

            for (auto& edge : graph[vertex].edges) {
                if ((pos < str.size()) && (edge.symbol == str.substr(pos, 1))) {
                    next_states.emplace_back(edge.to, pos + 1);
                } else if (edge.symbol.empty()) {
                    next_states.emplace_back(edge.to, pos);
                }
            }
        }

        std::swap(states, next_states);
    }
    return false;
}

void NFA::build_transitive_closure_on_empty_edges() {
    for (size_t vertex = 0; vertex < size(); ++vertex) {
        std::vector<char> used(size());
        std::deque<size_t> bfs;
        bfs.push_back(vertex);
        used[vertex] = 1;
        while (!bfs.empty()) {
            size_t cur_vertex = bfs.front();
            bfs.pop_front();

            for (auto& edge : graph[cur_vertex].edges) {
                if (!edge.symbol.empty() || used[edge.to]) {
                    continue;
                }
                bfs.push_back(edge.to);
                used[edge.to] = 1;
            }
        }

        for (size_t i = 0; i < size(); ++i) {
            if ((i == vertex) || !used[i]) {
                continue;
            }
            add_edge(vertex, i, "");
        }
    }
}

void NFA::add_terminal_states_by_empty_edges() {
    for (size_t vertex = 0; vertex < size(); ++vertex) {
        if (graph[vertex].is_terminal) {
            continue;
        }
        for (auto& edge : graph[vertex].edges) {
            if (!edge.symbol.empty() || !graph[edge.to].is_terminal) {
                continue;
            }
            graph[vertex].is_terminal = true;
            terminal_vertices.push_back(vertex);
            break;
        }
    }
}

void NFA::add_edges_through_empty_edges() {
    for (size_t vertex = 0; vertex < size(); ++vertex) {
        std::vector<Edge> new_edges;
        for (auto& edge : graph[vertex].edges) {
            if (!edge.symbol.empty()) {
                continue;
            }
            for (auto& edge_new : graph[edge.to].edges) {
                if (edge_new.symbol.empty()) {
                    continue;
                }
                new_edges.emplace_back(edge_new.to, edge_new.symbol);
            }
        }
        for (auto& edge : new_edges) {
            add_edge(vertex, edge.to, edge.symbol);
        }
    }
}

void NFA::remove_empty_edges() {
    for (size_t vertex = 0; vertex < size(); ++vertex) {
        std::vector<Edge> new_edges;
        for (auto& edge : graph[vertex].edges) {
            if (edge.symbol.empty()) {
                continue;
            }
            new_edges.push_back(edge);
        }
        std::swap(new_edges, graph[vertex].edges);
    }
}

void NFA::remove_vertex(size_t vertex) {
    for (auto& other : graph) {
        std::vector<Edge> new_edges;
        for (auto& edge : other.edges) {
            if (edge.to != vertex) {
                if (edge.to > vertex) {
                    --edge.to;
                }
                new_edges.push_back(edge);
            }
        }
        std::swap(other.edges, new_edges);
    }

    if (graph[vertex].is_terminal) {
        terminal_vertices.erase(std::find(
                terminal_vertices.begin(), terminal_vertices.end(), vertex));
    }
    if (start_vertex == vertex) {
        start_vertex = size();
    }
    for (auto& other : terminal_vertices) {
        if (other > vertex) {
            --other;
        }
    }
    graph.erase(graph.begin() + vertex);
    --_size;
}

void NFA::remove_unreachable_vertices() {
    std::vector<char> used(size());
    std::deque<size_t> bfs;
    bfs.push_back(start_vertex);
    used[start_vertex] = 1;

    while (!bfs.empty()) {
        size_t cur_v = bfs.front();
        bfs.pop_front();

        for (auto& edge : graph[cur_v].edges) {
            if (used[edge.to]) {
                continue;
            }
            used[edge.to] = 1;
            bfs.push_back(edge.to);
        }
    }

    for (int vertex = size() - 1; vertex > -1; --vertex) {
        if (!used[vertex]) {
            remove_vertex(vertex);
        }
    }
}

void NFA::remove_multiple_edges() {
    for (auto& vertex : graph) {
        std::sort(vertex.edges.begin(), vertex.edges.end());
        vertex.edges.resize(std::distance(vertex.edges.begin(),
                                          std::unique(vertex.edges.begin(), vertex.edges.end())
                                          ));
    }
}

void NFA::normalize() {
    remove_unreachable_vertices();
    remove_multiple_edges();
}

void NFA::build_empty_edges_closure() {
    build_transitive_closure_on_empty_edges();
    add_terminal_states_by_empty_edges();
    add_edges_through_empty_edges();
    remove_empty_edges();
    normalize();
}

size_t NFA::size() const {
    return _size;
}

void NFA::clear() {
    _size = 0;
    start_vertex = 0;
    terminal_vertices.clear();
    graph.clear();
}

std::string NFA::to_graphviz() const {
    std::stringstream ss;
    ss << "digraph NFA {\n";
    ss << "node [style=\"filled\"];\n";
    for (size_t vertex = 0; vertex < size(); ++vertex) {
        for (auto& edge : graph[vertex].edges) {
            ss << "\"" << vertex << "\" -> \"" << edge.to << "\" [label=\"" << edge.symbol << "\"];\n";
        }
    }
    ss << "\"" << start_vertex << "\" [fillcolor=\"" << (graph[start_vertex].is_terminal ? "yellow" : "green") << "\"];\n";
    for (auto vertex : terminal_vertices) {
        if (vertex == start_vertex) {
            continue;
        }
        ss << "\"" << vertex << "\" [fillcolor=\"red\"];\n";
    }
    ss << "}\n";
    return ss.str();
}

