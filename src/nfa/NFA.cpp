#include <iostream>
#include <string>
#include <sstream>
#include <tuple>
#include <deque>
#include <cstring>
#include <set>
#include "NFA.h"


NFAEdge::NFAEdge() : _to(0) {};
NFAEdge::NFAEdge(size_t to, std::string symbol) : _to(to), _symbol(std::move(symbol)) {}

bool NFAEdge::operator<(const NFAEdge& other) const {
    return tie(_to, _symbol) < tie(other._to, other._symbol);
}

bool NFAEdge::operator==(const NFAEdge& other) const {
    return (_to == other._to) && (_symbol == other._symbol);
}

size_t NFAEdge::to() const {
    return _to;
}

const std::string& NFAEdge::symbol() const {
    return _symbol;
}

NFAVertex::basic_iterator::basic_iterator(GraphType::const_iterator it) : _it(it) {}

const NFAVertex::basic_iterator::value_type& NFAVertex::basic_iterator::operator*() const {
    return *_it;
}
const NFAVertex::basic_iterator::value_type* NFAVertex::basic_iterator::operator->() const {
    return _it.operator->();
}

NFAVertex::basic_iterator& NFAVertex::basic_iterator::operator++() {
    ++_it;
    return *this;
}

NFAVertex::basic_iterator NFAVertex::basic_iterator::operator++(int) {
    auto copy = *this;
    ++(*this);
    return copy;
}

NFAVertex::basic_iterator& NFAVertex::basic_iterator::operator--() {
    --_it;
    return *this;
}

NFAVertex::basic_iterator NFAVertex::basic_iterator::operator--(int) {
    auto copy = *this;
    --(*this);
    return copy;
}

bool NFAVertex::basic_iterator::operator==(const NFAVertex::basic_iterator& other) const {
    return _it == other._it;
};

NFAVertex& NFAVertex::edges() {
    return *this;
}

const NFAVertex& NFAVertex::edges() const {
    return *this;
}

NFAVertex::iterator NFAVertex::begin() {
    return iterator(_edges.begin());
}

NFAVertex::iterator NFAVertex::end() {
    return iterator(_edges.end());
}

NFAVertex::const_iterator NFAVertex::begin() const {
    return basic_iterator(_edges.begin());
}

NFAVertex::const_iterator NFAVertex::end() const {
    return iterator(_edges.end());
}

NFAVertex::reverse_iterator NFAVertex::rbegin() {
    return reverse_iterator(end());
}

NFAVertex::reverse_iterator NFAVertex::rend() {
    return reverse_iterator(begin());
}

NFAVertex::const_reverse_iterator NFAVertex::rbegin() const {
    return reverse_iterator(end());
}

NFAVertex::const_reverse_iterator NFAVertex::rend() const {
    return reverse_iterator(begin());
}

bool NFAVertex::is_terminal() const {
    return _is_terminal;
}

void NFAVertex::set_terminal() {
    set_terminal_value(true);
}

void NFAVertex::reset_terminal() {
    set_terminal_value(false);
}

void NFAVertex::set_terminal_value(bool value) {
    _is_terminal = value;
}

void NFAVertex::add_edge(size_t to, const std::string& symbol) {
    _edges.emplace_back(to, symbol);
}

void NFAVertex::clear_edges() {
    _edges.clear();
}

void NFAVertex::remove_multiple_edges() {
    std::sort(_edges.begin(), _edges.end());
    _edges.resize(std::distance(_edges.begin(),
                                      std::unique(_edges.begin(), _edges.end())));
}


NFA::NFA() : Automaton<NFAVertex, std::string>() {}
NFA::NFA(size_t size) : Automaton<NFAVertex, std::string>(size) {}

bool NFA::accepts(const std::string& str) {
    size_t max_depth = std::max(static_cast<size_t>(str.size() * MAX_SEARCH_MULTIPLIER),
                             static_cast<size_t>(MAX_SEARCH_MULTIPLIER));
    std::vector< std::pair<int, size_t> > states, next_states;
    states.emplace_back(_start_vertex, 0);
    for (size_t depth = 0; depth < max_depth; ++depth) {
        if (states.empty()) {
            break;
        }

        while (!states.empty()) {
            auto [vertex, pos] = states.back();
            states.pop_back();

            if ((pos == str.size()) && _graph[vertex].is_terminal()) {
                return true;
            }

            for (auto& edge : _graph[vertex].edges()) {
                if ((pos < str.size()) && (edge.symbol() == str.substr(pos, 1))) {
                    next_states.emplace_back(edge.to(), pos + 1);
                } else if (edge.symbol().empty()) {
                    next_states.emplace_back(edge.to(), pos);
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

            for (auto& edge : _graph[cur_vertex].edges()) {
                if (!edge.symbol().empty() || used[edge.to()]) {
                    continue;
                }
                bfs.push_back(edge.to());
                used[edge.to()] = 1;
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
        if (_graph[vertex].is_terminal()) {
            continue;
        }
        for (auto& edge : _graph[vertex].edges()) {
            if (!edge.symbol().empty() || !_graph[edge.to()].is_terminal()) {
                continue;
            }
            set_end(vertex);
            break;
        }
    }
}

void NFA::add_edges_through_empty_edges() {
    for (auto& vertex : _graph) {
        NFAVertex new_vertex;
        new_vertex.set_terminal_value(vertex.is_terminal());
        for (auto& edge : vertex.edges()) {
            new_vertex.add_edge(edge.to(), edge.symbol());
            if (!edge.symbol().empty()) {
                continue;
            }
            for (auto& edge_new : _graph[edge.to()].edges()) {
                if (edge_new.symbol().empty()) {
                    continue;
                }
                new_vertex.add_edge(edge_new.to(), edge_new.symbol());
            }
        }
        std::swap(vertex, new_vertex);
    }
}

void NFA::remove_empty_edges() {
    for (auto& vertex : _graph) {
        NFAVertex new_vertex;
        new_vertex.set_terminal_value(vertex.is_terminal());
        for (auto& edge : vertex.edges()) {
            if (edge.symbol().empty()) {
                continue;
            }
            new_vertex.add_edge(edge.to(), edge.symbol());
        }
        std::swap(vertex, new_vertex);
    }
}

void NFA::remove_multiple_edges() {
    for (auto& vertex : _graph) {
        vertex.remove_multiple_edges();
    }
}

void NFA::normalize() {
    remove_unreachable_from_start_vertices();
    remove_vertices_unreachable_to_terminal();
    remove_multiple_edges();
}

void NFA::build_empty_edges_closure() {
    build_transitive_closure_on_empty_edges();
    add_terminal_states_by_empty_edges();
    add_edges_through_empty_edges();
    remove_empty_edges();
    normalize();
}

std::set<char> NFA::get_alphabet() const {
    std::set<char> alphabet;
    for (auto& vertex : _graph) {
        for (auto& edge : vertex.edges()) {
            if (edge.symbol().empty()) {
                continue;
            }
            alphabet.insert(edge.symbol().front());
        }
    }
    return alphabet;
}