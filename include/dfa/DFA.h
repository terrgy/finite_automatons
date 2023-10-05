#pragma once

#include <vector>
#include <string>
#include <map>
#include "Automaton.h"
#include "NFA.h"

struct DFAVertex {
    using GraphType = std::map<char, size_t>;

    class basic_iterator;

    class DFAEdge : EdgesInterface<char> {
    private:
        friend class basic_iterator;
        GraphType::const_iterator _it;
    public:
        explicit DFAEdge(GraphType::const_iterator it);

    public:
        size_t to() const override;
        const char& symbol() const override;
    };

    class basic_iterator {
    public:
        using difference_type = ptrdiff_t;
        using value_type = DFAEdge;
        using reference = value_type&;
        using pointer = value_type*;
        using iterator_category = std::bidirectional_iterator_tag;

    private:
        GraphType::const_iterator _it;
        mutable DFAEdge _edge;

        void _remake_edge();

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

    using value_type = DFAEdge;
    using iterator = basic_iterator;
    using const_iterator = iterator;
    using reverse_iterator = std::reverse_iterator<iterator>;
    using const_reverse_iterator = reverse_iterator;


    bool _is_terminal;
    GraphType _edges;

    DFAVertex& edges();
    const DFAVertex& edges() const;
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
    void add_edge(size_t to, const char& symbol);
    void clear_edges();
    void remove_multiple_edges();
    bool move_exists(char symbol) const;
    size_t move(char symbol) const;
};

struct DFA : Automaton<DFAVertex, char> {
    std::set<char> _alphabet;

    DFA();
    explicit DFA(size_t size);
    explicit DFA(const NFA& nfa);
    void set_alphabet(const std::set<char>&);
    bool accepts(const std::string& str) override;
    void normalize();
    size_t build_from_NFA_make_vertex(const NFA&, const std::set<size_t>&,
            std::map< std::set<size_t>, size_t>&);
    void build_from_NFA(const NFA& nfa);
    void build_FDFA();
    void rebuild_to_complement();
};
