#include <set>
#include <vector>
#include <deque>
#include <map>
#include "DFA.h"

DFAVertex::DFAEdge::DFAEdge(GraphType::const_iterator it) : _it(it) {}

size_t DFAVertex::DFAEdge::to() const {
    return _it->second;
}

const char& DFAVertex::DFAEdge::symbol() const {
    return _it->first;
}

void DFAVertex::basic_iterator::_remake_edge() {
    _edge = DFAEdge(_it);
}

DFAVertex::basic_iterator::basic_iterator(GraphType::const_iterator it) : _it(it), _edge(_it) {}

const DFAVertex::basic_iterator::value_type& DFAVertex::basic_iterator::operator*() const {
    return _edge;
}
const DFAVertex::basic_iterator::value_type* DFAVertex::basic_iterator::operator->() const {
    return &_edge;
}

DFAVertex::basic_iterator& DFAVertex::basic_iterator::operator++() {
    ++_it;
    _remake_edge();
    return *this;
}

DFAVertex::basic_iterator DFAVertex::basic_iterator::operator++(int) {
    auto copy = *this;
    ++(*this);
    return copy;
}

DFAVertex::basic_iterator& DFAVertex::basic_iterator::operator--() {
    --_it;
    _remake_edge();
    return *this;
}

DFAVertex::basic_iterator DFAVertex::basic_iterator::operator--(int) {
    auto copy = *this;
    --(*this);
    return copy;
}

bool DFAVertex::basic_iterator::operator==(const DFAVertex::basic_iterator& other) const {
    return _it == other._it;
};

DFAVertex& DFAVertex::edges() {
    return *this;
}

const DFAVertex& DFAVertex::edges() const {
    return *this;
}

DFAVertex::iterator DFAVertex::begin() {
    return iterator(_edges.begin());
}

DFAVertex::iterator DFAVertex::end() {
    return iterator(_edges.end());
}

DFAVertex::const_iterator DFAVertex::begin() const {
    return basic_iterator(_edges.begin());
}

DFAVertex::const_iterator DFAVertex::end() const {
    return iterator(_edges.end());
}

DFAVertex::reverse_iterator DFAVertex::rbegin() {
    return reverse_iterator(end());
}

DFAVertex::reverse_iterator DFAVertex::rend() {
    return reverse_iterator(begin());
}

DFAVertex::const_reverse_iterator DFAVertex::rbegin() const {
    return reverse_iterator(end());
}

DFAVertex::const_reverse_iterator DFAVertex::rend() const {
    return reverse_iterator(begin());
}

bool DFAVertex::is_terminal() const {
    return _is_terminal;
}

void DFAVertex::set_terminal() {
    set_terminal_value(true);
}

void DFAVertex::reset_terminal() {
    set_terminal_value(false);
}

void DFAVertex::set_terminal_value(bool value) {
    _is_terminal = value;
}

void DFAVertex::add_edge(size_t to, const char& symbol) {
    _edges[symbol] = to;
}

void DFAVertex::clear_edges() {
    _edges.clear();
}

void DFAVertex::remove_multiple_edges() {}

bool DFAVertex::move_exists(char symbol) const {
    return _edges.contains(symbol);
}

size_t DFAVertex::move(char symbol) const {
    return _edges.at(symbol);
}

DFA::DFA() : Automaton<DFAVertex, char>() {}
DFA::DFA(size_t size) : Automaton<DFAVertex, char>(size) {}
DFA::DFA(const NFA& nfa) : Automaton<DFAVertex, char>() {
    set_alphabet(nfa.get_alphabet());
    build_from_NFA(nfa);
}

bool DFA::accepts(const std::string& str) {
    size_t cur_vertex = _start_vertex;
    for (char c : str) {
        if (!_graph[cur_vertex].move_exists(c)) {
            return false;
        }
        cur_vertex = _graph[cur_vertex].move(c);
    }
    return _graph[cur_vertex].is_terminal();
}

void DFA::normalize() {
    remove_unreachable_from_start_vertices();
    remove_vertices_unreachable_to_terminal();
}

size_t DFA::build_from_NFA_make_vertex(const NFA& nfa, const std::set<size_t>& st,
                                       std::map< std::set<size_t>, size_t>& set_to_vertex) {
    auto new_vertex = add_vertices(1);
    for (auto vertex : st) {
        if (nfa._graph[vertex].is_terminal()) {
            set_end(new_vertex);
            break;
        }
    }
    set_to_vertex[st] = new_vertex;
    return new_vertex;
}

void DFA::set_alphabet(const std::set<char>& alphabet) {
    _alphabet = alphabet;
}

void DFA::build_from_NFA(const NFA& nfa) {
    clear();
    std::map< std::set<size_t>, size_t> st_to_vertex;
    std::deque< std::pair<std::set<size_t>, size_t> > bfs;
    auto start_dfa_vertex = build_from_NFA_make_vertex(nfa, {nfa._start_vertex}, st_to_vertex);
    set_start(start_dfa_vertex);
    bfs.emplace_back(std::set<size_t>{nfa._start_vertex}, start_dfa_vertex);

    while (!bfs.empty()) {
        auto [cur_set, cur_dfa_vertex] = bfs.front();
        bfs.pop_front();

        for (char symbol : _alphabet) {
            std::set<size_t> new_set;
            for (auto vertex : cur_set) {
                for (auto& edge : nfa._graph[vertex].edges()) {
                    if (edge.symbol().empty() || (edge.symbol().front() != symbol)) {
                        continue;
                    }
                    new_set.insert(edge.to());
                }
            }
            size_t new_dfa_vertex;
            if (!st_to_vertex.contains(new_set)) {
                new_dfa_vertex = build_from_NFA_make_vertex(nfa, new_set, st_to_vertex);
                bfs.emplace_back(new_set, new_dfa_vertex);
            } else {
                new_dfa_vertex = st_to_vertex[new_set];
            }

            add_edge(cur_dfa_vertex, new_dfa_vertex, symbol);
        }
    }
    normalize();
}

void DFA::build_FDFA() {
    remove_vertices_unreachable_to_terminal();
    size_t sink_vertex = add_vertices(1);
    for (size_t vertex = 0; vertex < size(); ++vertex) {
        for (char symbol : _alphabet) {
            if (!_graph[vertex].move_exists(symbol)) {
                add_edge(vertex, sink_vertex, symbol);
            }
        }
    }
}

void DFA::rebuild_to_complement() {
    std::vector<char> terminal_flag(size());
    for (auto vertex : _terminal_vertices) {
        terminal_flag[vertex] = 1;
    }
    remove_all_end_vertices();
    for (size_t vertex = 0; vertex < size(); ++vertex) {
        if (!terminal_flag[vertex]) {
            set_end(vertex);
        }
    }
}

NFA DFA::to_NFA() {
    NFA nfa(size());
    nfa.set_start(_start_vertex);
    for (auto vertex : _terminal_vertices) {
        nfa.set_end(vertex);
    }

    for (size_t vertex = 0; vertex < size(); ++vertex) {
        for (auto& edge : _graph[vertex].edges()) {
            nfa.add_edge(vertex, edge.to(), std::string(1, edge.symbol()));
        }
    }
    return nfa;
}