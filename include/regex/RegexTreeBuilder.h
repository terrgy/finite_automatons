#pragma once

#include <string>
#include <vector>
#include <functional>
#include "NFA.h"

struct RegExTreeBuilder {
    struct Vertex {
        std::string operation;
        std::vector<int> edges;
        std::function<NFA(int, std::vector<Vertex>&)> builder;

        Vertex(std::string  operation, std::function<NFA(int, std::vector<Vertex>&)> builder);
        Vertex(std::string  operation, std::vector<int> edges, std::function<NFA(int, std::vector<Vertex>&)> builder);
    };

    struct RegExParseOperation {
        std::string target;
        std::function<void(int, std::vector<Vertex>&, std::string&)> handler;
    };

    static const std::vector<RegExParseOperation> operations;
    std::string regex;
    std::vector<Vertex> graph;

    RegExTreeBuilder(std::string& regex);

    static void RemoveUnnecessaryParenthesis(int pos, std::string& regex) {
        int left_pos = pos;
        int right_pos = pos + 1;
        while (regex[right_pos] != '#') {
            ++right_pos;
        }
        int token_end_pos = right_pos;
        while ((left_pos != 0) && (regex[left_pos - 1] == '(') && (right_pos + 1 < regex.size()) && (regex[right_pos + 1] == ')')) {
            --left_pos;
            ++right_pos;
        }
        if (left_pos == pos) {
            return;
        }
        regex.replace(left_pos, right_pos - left_pos + 1, regex.substr(pos, token_end_pos - pos + 1));
    }

    static void KleeneStarHandler(int pos, std::vector<RegExTreeBuilder::Vertex>& graph, std::string& regex) {
        int left_pos, right_pos;
        right_pos = pos + 2;
        left_pos = pos - 1;
        while (regex[left_pos] != '#') {
            --left_pos;
        }
        int vertex_index = std::stoi(regex.substr(left_pos + 1, pos - left_pos - 1));
        int new_vertex_index = graph.size();
        graph.emplace_back("*", std::vector<int>{vertex_index}, RegExTreeBuilder::KleeneStarBuilder);
        regex.replace(left_pos, right_pos  - left_pos, "#" + std::to_string(new_vertex_index) + "#");
        RegExTreeBuilder::RemoveUnnecessaryParenthesis(left_pos, regex);
    }

    static void KleenePlusHandler(int pos, std::vector<Vertex>& graph, std::string& regex) {
        int left_pos, right_pos;
        right_pos = pos + 2;
        left_pos = pos - 1;
        while (regex[left_pos] != '#') {
            --left_pos;
        }
        int vertex_index = std::stoi(regex.substr(left_pos + 1, pos - left_pos - 1));
        int new_vertex_index = graph.size();
        graph.emplace_back("%", std::vector<int>{vertex_index}, RegExTreeBuilder::KleenePlusBuilder);
        regex.replace(left_pos, right_pos  - left_pos, "#" + std::to_string(new_vertex_index) + "#");
        RegExTreeBuilder::RemoveUnnecessaryParenthesis(left_pos, regex);
    }

    static void PowerHandler(int pos, std::vector<Vertex>& graph, std::string& regex) {
        int left_pos, right_pos;
        right_pos = pos + 2;
        left_pos = pos - 1;
        while (regex[left_pos] != '#') {
            --left_pos;
        }
        int power_begin_pos = right_pos;
        while ((right_pos < regex.size()) && isdigit(regex[right_pos])) {
            ++right_pos;
        }

        int vertex_index = std::stoi(regex.substr(left_pos + 1, pos - left_pos - 1));
        int new_vertex_index = graph.size();
        graph.emplace_back("^" + regex.substr(power_begin_pos, right_pos - power_begin_pos),
                           std::vector<int>{vertex_index}, RegExTreeBuilder::PowerBuilder);
        regex.replace(left_pos, right_pos  - left_pos, "#" + std::to_string(new_vertex_index) + "#");
        RegExTreeBuilder::RemoveUnnecessaryParenthesis(left_pos, regex);
    }

    static void MultiplyHandler(int pos, std::vector<Vertex>& graph, std::string& regex) {
        int left_pos = pos - 1;
        while (regex[left_pos] != '#') {
            --left_pos;
        }
        int pos2 = pos + 1;
        int right_pos = pos2 + 1;
        while (regex[right_pos] != '#') {
            ++right_pos;
        }
        int vertex1_index = std::stoi(regex.substr(left_pos + 1, pos - left_pos - 1));
        int vertex2_index = std::stoi(regex.substr(pos2 + 1, right_pos - pos2 - 1));
        int new_vertex_index = graph.size();
        graph.emplace_back(".", std::vector<int>{vertex1_index, vertex2_index}, RegExTreeBuilder::MultiplyBuilder);
        regex.replace(left_pos, right_pos  - left_pos + 1, "#" + std::to_string(new_vertex_index) + "#");
        RegExTreeBuilder::RemoveUnnecessaryParenthesis(left_pos, regex);
    }

    static void PlusHandler(int pos, std::vector<Vertex>& graph, std::string& regex) {
        int left_pos = pos - 1;
        while (regex[left_pos] != '#') {
            --left_pos;
        }
        int pos2 = pos + 2;
        int right_pos = pos2 + 1;
        while (regex[right_pos] != '#') {
            ++right_pos;
        }
        int vertex1_index = std::stoi(regex.substr(left_pos + 1, pos - left_pos - 1));
        int vertex2_index = std::stoi(regex.substr(pos2 + 1, right_pos - pos2 - 1));
        int new_vertex_index = graph.size();
        graph.emplace_back("+", std::vector<int>{vertex1_index, vertex2_index}, RegExTreeBuilder::PlusBuilder);
        regex.replace(left_pos, right_pos  - left_pos + 1, "#" + std::to_string(new_vertex_index) + "#");
        RegExTreeBuilder::RemoveUnnecessaryParenthesis(left_pos, regex);
    }

    static void AlphaHandler(int pos, std::vector<Vertex>& graph, std::string& regex) {
        int new_vertex_index = graph.size();
        graph.emplace_back(regex.substr(pos, 1), RegExTreeBuilder::AlphaNFABuilder);
        regex.replace(pos, 1, "#" + std::to_string(new_vertex_index) + "#");
        RegExTreeBuilder::RemoveUnnecessaryParenthesis(pos, regex);
    }

    static NFA AlphaNFABuilder(int pos, std::vector<Vertex>& graph) {
        NFA nfa(2);
        nfa.set_start(0);
        nfa.set_end(1);
        nfa.add_edge(0, 1, graph[pos].operation);
        return nfa;
    }

    static NFA KleeneStarBuilder(int pos, std::vector<Vertex>& graph) {
        NFA nfa = nfa_builder(graph[pos].edges.front(), graph);
        int new_index = nfa.add_vertices(1);
        for (auto end_vertex : nfa.terminal_vertices) {
            nfa.add_edge(end_vertex, new_index, "");
        }
        nfa.add_edge(new_index, nfa.start_vertex, "");
        nfa.remove_all_end_vertices();
        nfa.set_start(new_index);
        nfa.set_end(new_index);
        return nfa;
    }

    static NFA KleenePlusBuilder(int pos, std::vector<Vertex>& graph) {
        NFA nfa = nfa_builder(graph[pos].edges.front(), graph);
        for (auto end_vertex : nfa.terminal_vertices) {
            nfa.add_edge(end_vertex, nfa.start_vertex, "");
        }
        return nfa;
    }

    static NFA PowerBuilder(int pos, std::vector<Vertex>& graph) {
        NFA nfa(0);
        int power = std::stoi(graph[pos].operation.substr(1));
        if (power == 0) {
            nfa.add_vertices(1);
            nfa.set_start(0);
            nfa.set_end(0);
        } else {
            NFA nfa_op = nfa_builder(graph[pos].edges.front(), graph);
            nfa = nfa_op;
            for (int i = 1; i < power; ++i) {
                int shift = nfa.merge(nfa_op);
                for (int end_vertex : nfa.terminal_vertices) {
                    nfa.add_edge(end_vertex, shift + nfa_op.start_vertex, "");
                }
                nfa.remove_all_end_vertices();
                for (int end_vertex : nfa_op.terminal_vertices) {
                    nfa.set_end(shift + end_vertex);
                }
            }
        }
        return nfa;
    }

    static NFA PlusBuilder(int pos, std::vector<Vertex>& graph) {
        NFA nfa = nfa_builder(graph[pos].edges.front(), graph);
        NFA nfa2 = nfa_builder(graph[pos].edges.back(), graph);

        int shift = nfa.merge(nfa2);
        int new_start = nfa.add_vertices(1);
        nfa.add_edge(new_start, nfa.start_vertex, "");
        nfa.add_edge(new_start, nfa2.start_vertex + shift, "");
        nfa.set_start(new_start);
        for (int end_vertex : nfa2.terminal_vertices) {
            nfa.set_end(shift + end_vertex);
        }

        return nfa;
    }

    static NFA MultiplyBuilder(int pos, std::vector<Vertex>& graph) {
        NFA nfa = nfa_builder(graph[pos].edges.front(), graph);
        NFA nfa2 = nfa_builder(graph[pos].edges.back(), graph);

        int shift = nfa.merge(nfa2);
        for (int end_vertex : nfa.terminal_vertices) {
            nfa.add_edge(end_vertex, shift + nfa2.start_vertex, "");
        }
        nfa.remove_all_end_vertices();
        for (int end_vertex : nfa2.terminal_vertices) {
            nfa.set_end(shift + end_vertex);
        }

        return nfa;
    }

    static NFA nfa_builder(int pos, std::vector<Vertex>& graph) {
        return graph[pos].builder(pos, graph);
    }

    void build();
    int build_operations();
    NFA build_nfa();
};

