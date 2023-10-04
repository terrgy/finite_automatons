#include <iostream>
#include <string>
#include <sstream>
#include <tuple>
#include <deque>
#include "NFA.h"


NFA::NFA() : size(1), graph(1), start_vertex(0) {
    set_end(0);
}
NFA::NFA(int size) : size(size), graph(size) {}

void NFA::set_start(int vertex) {
    start_vertex = vertex;
}

void NFA::set_end(int vertex) {
    if (!graph[vertex].is_end) {
        graph[vertex].is_end = true;
        end_vertices.push_back(vertex);
    }
}

void NFA::add_edge(int from, int to, const std::string& symbol) {
    graph[from].edges.emplace_back(to, symbol);
}

void NFA::add_bidirectional_edge(int from, int to, const std::string& symbol) {
    add_edge(from, to, symbol);
    add_edge(to, from, symbol);
}

int NFA::add_vertices(int count) {
    size += count;
    graph.resize(size);
    return size - count;
}

void NFA::remove_all_end_vertices() {
    while (!end_vertices.empty()) {
        graph[end_vertices.back()].is_end = false;
        end_vertices.pop_back();
    }
}

int NFA::merge(const NFA& other) {
    int shift = size;
    add_vertices(other.size);
    for (int i = 0; i < other.size; ++i) {
        for (auto& edge : other.graph[i].edges) {
            graph[i + shift].edges.emplace_back(edge.to + shift, edge.symbol);
        }
    }
    return shift;
}

std::string NFA::to_string() {
    std::stringstream ss;
    ss << start_vertex << "\n\n";
    for (int i : end_vertices) {
        ss << i << '\n';
    }
    ss << '\n';
    for (int i = 0; i < size; ++i) {
        for (auto& edge : graph[i].edges) {
            ss << i << ' ' << edge.to << ' ' << edge.symbol << '\n';
        }
    }
    return ss.str();
}

bool NFA::accepts(const std::string& str) {
    int max_depth = std::max(static_cast<int>(str.size() * MAX_SEARCH_MULTIPLIER),
                             static_cast<int>(MAX_SEARCH_MULTIPLIER));
    std::vector< std::pair<int, int> > states, next_states;
    states.emplace_back(start_vertex, 0);
    for (int depth = 0; depth < max_depth; ++depth) {
        if (states.empty()) {
            break;
        }

        while (!states.empty()) {
            auto [vertex, pos] = states.back();
            states.pop_back();

            if ((pos == str.size()) && graph[vertex].is_end) {
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