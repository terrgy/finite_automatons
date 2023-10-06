#pragma once

#include <vector>
#include <string>
#include <map>
#include <set>
#include "Automaton.h"

class NFAEdge : EdgesInterface<std::string> {
public:
    size_t _to;
    std::string _symbol;

    NFAEdge();
    NFAEdge(size_t to, std::string symbol);

    bool operator<(const NFAEdge&) const;
    bool operator==(const NFAEdge&) const;
    size_t to() const override;
    const std::string& symbol() const override;
};

struct NFAVertex {
    using GraphType = std::vector<NFAEdge>;

    class basic_iterator;

    class basic_iterator {
    public:
        using difference_type = ptrdiff_t;
        using value_type = NFAEdge;
        using reference = value_type&;
        using pointer = value_type*;
        using iterator_category = std::bidirectional_iterator_tag;

    private:
        GraphType::const_iterator _it;

    public:
        explicit basic_iterator(GraphType::const_iterator it);
        const value_type& operator*() const;
        const value_type* operator->() const;
        basic_iterator& operator++();
        basic_iterator operator++(int);
        basic_iterator& operator--();
        basic_iterator operator--(int);
        bool operator==(const basic_iterator& other) const;
    };

    using value_type = NFAEdge;
    using iterator = basic_iterator;
    using const_iterator = iterator;
    using reverse_iterator = std::reverse_iterator<iterator>;
    using const_reverse_iterator = reverse_iterator;

    bool _is_terminal;
    GraphType _edges;

    NFAVertex& edges();
    const NFAVertex& edges() const;
    iterator begin();
    iterator end();
    const_iterator begin() const;
    const_iterator end() const;
    reverse_iterator rbegin();
    reverse_iterator rend();
    const_reverse_iterator rbegin() const;
    const_reverse_iterator rend() const;
    bool is_terminal() const;
    void set_terminal();
    void reset_terminal();
    void set_terminal_value(bool);
    void add_edge(size_t to, const std::string& symbol);
    void clear_edges();
    void remove_multiple_edges();
};

struct NFA : Automaton<NFAVertex, std::string> {
    constexpr static const double MAX_SEARCH_MULTIPLIER = 10;

    void build_transitive_closure_on_empty_edges();
    void add_terminal_states_by_empty_edges();
    void add_edges_through_empty_edges();
    void remove_empty_edges();

    NFA();
    explicit NFA(size_t size);
    bool accepts(const std::string& str) override;
    void build_empty_edges_closure();
    void remove_multiple_edges();
    void normalize();
    std::set<char> get_alphabet() const;
    void to_regex_remove_multiple_edges();
    std::string to_regex_wrap(std::string);
    std::string to_regex_wrap_kleene(std::string);
    std::string to_regex_merge_strs(std::string, std::string, size_t);
    void to_regex_remove_vertex(size_t);
    std::string to_regex();
};
