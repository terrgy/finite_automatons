#include <iostream>
#include <string>
#include <sstream>
#include "NFA.h"
#include "RegexTreeBuilder.h"

std::ostream& operator<<(std::ostream& out, const NFA& nfa) {
    out << nfa.to_string();
    return out;
}

std::istream& operator>>(std::istream& in, NFA& nfa) {
    nfa.clear();
    in >> nfa.start_vertex;
    in.get();
    std::string line;
    std::getline(in, line);
    std::getline(in, line);
    while (!line.empty()) {
        nfa.terminal_vertices.push_back(std::stoi(line));
        std::getline(in, line);
    }
    std::getline(in, line);
    while (!in.eof() || !line.empty()) {
        std::stringstream ss(line);
        int from, to;
        std::string symbol;
        ss >> from >> to >> symbol;
        nfa.add_edge(from, to, symbol);
    }
    return in;
}

int main() {
    std::cout << "Hello, world!\n";
    return 0;
}
