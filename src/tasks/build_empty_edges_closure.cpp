#include <iostream>
#include <string>
#include <sstream>
#include "NFA.h"

std::ostream& operator<<(std::ostream& out, const NFA& nfa) {
    out << nfa.to_string();
    return out;
}

std::istream& operator>>(std::istream& in, NFA& nfa) {
    size_t start_vertex;
    in >> start_vertex;
    in.get();
    size_t max_vertex = start_vertex;

    std::string line;
    std::getline(in, line);
    std::getline(in, line);
    std::vector<size_t> terminal_vertices;
    while (!line.empty()) {
        size_t vertex = std::stoi(line);
        terminal_vertices.push_back(vertex);
        max_vertex = std::max(max_vertex, vertex);
        std::getline(in, line);
    }

    std::vector< std::tuple<size_t, size_t, std::string> > edges;
    std::getline(in, line);
    while (!in.eof() && !line.empty()) {
        std::stringstream ss(line);
        size_t from, to;
        std::string symbol;
        ss >> from >> to >> symbol;
        max_vertex = std::max(max_vertex, std::max(from, to));
        edges.emplace_back(from, to, symbol);
        std::getline(in, line);
    }

    nfa.clear();
    nfa.add_vertices(max_vertex + 1);
    nfa.set_start(start_vertex);
    for (auto vertex : terminal_vertices) {
        nfa.set_end(vertex);
    }
    for (auto& [from, to, symbol] : edges) {
        nfa.add_edge(from, to, symbol);
    }

    return in;
}

int main() {
    NFA nfa;
    std::cin >> nfa;
    nfa.build_empty_edges_closure();
    std::cout << nfa << '\n';
    return 0;
}
