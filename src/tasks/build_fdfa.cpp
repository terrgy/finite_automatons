#include <iostream>
#include <string>
#include <sstream>
#include "DFA.h"

std::ostream& operator<<(std::ostream& out, const DFA& dfa) {
    out << dfa.to_string();
    return out;
}

std::istream& operator>>(std::istream& in, DFA& dfa) {
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

    std::vector< std::tuple<size_t, size_t, char> > edges;
    std::getline(in, line);
    while (!in.eof() && !line.empty()) {
        std::stringstream ss(line);
        size_t from, to;
        char symbol;
        ss >> from >> to >> symbol;
        max_vertex = std::max(max_vertex, std::max(from, to));
        edges.emplace_back(from, to, symbol);
        std::getline(in, line);
    }

    dfa.clear();
    dfa.add_vertices(max_vertex + 1);
    dfa.set_start(start_vertex);
    for (auto vertex : terminal_vertices) {
        dfa.set_end(vertex);
    }
    for (auto& [from, to, symbol] : edges) {
        dfa.add_edge(from, to, symbol);
    }

    return in;
}

int main() {
    DFA dfa;
    std::cin >> dfa;
    dfa.build_FDFA();
    std::cout << dfa << '\n';
    return 0;
}
