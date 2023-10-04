#pragma once

#include <vector>
#include <string>

struct NFA {
    struct Edge {
        int to;
        std::string symbol;
    };

    struct Vertex {
        bool is_end;
        std::vector<Edge> edges;
    };

    int size;
    int start_vertex;
    std::vector<int> end_vertices;
    std::vector<Vertex> graph;

    constexpr static const double MAX_SEARCH_MULTIPLIER = 10;

    NFA();
    NFA(int size);
    void set_start(int vertex);
    void set_end(int vertex);
    void add_edge(int from, int to, const std::string& symbol);
    void add_bidirectional_edge(int from, int to, const std::string& symbol);
    int add_vertices(int count);
    void remove_all_end_vertices();
    int merge(const NFA& other);
    std::string to_string();
    bool accepts(const std::string& str);
};