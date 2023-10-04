#pragma once

#include <vector>
#include <string>
#include <map>


struct NFA {
    struct Edge {
        size_t to;
        std::string symbol;

        bool operator<(const Edge&) const;
        bool operator==(const Edge&) const;
    };

    struct Vertex {
        bool is_terminal;
        std::vector<Edge> edges;
    };

    size_t _size;
    size_t start_vertex;
    std::vector<size_t> terminal_vertices;
    std::vector<Vertex> graph;

    /*std::vector<VertexType> vertex_to_name;
    std::map<VertexType, size_t> name_to_vertex;*/

    constexpr static const double MAX_SEARCH_MULTIPLIER = 10;

    void build_transitive_closure_on_empty_edges();
    void add_terminal_states_by_empty_edges();
    void add_edges_through_empty_edges();
    void remove_empty_edges();
    void remove_vertex(size_t);

    NFA();
    explicit NFA(size_t size);
    void set_start(size_t vertex);
    void set_end(size_t vertex);
    void add_edge(size_t from, size_t to, const std::string& symbol);
    void add_bidirectional_edge(size_t from, size_t to, const std::string& symbol);
    size_t add_vertices(size_t count);
    void remove_all_end_vertices();
    size_t merge(const NFA& other);
    std::string to_string() const;
    std::string to_graphviz() const;
    bool accepts(const std::string& str);
    void build_empty_edges_closure();
    void remove_unreachable_vertices();
    void remove_multiple_edges();
    void normalize();
    size_t size() const;
    void clear();
};
