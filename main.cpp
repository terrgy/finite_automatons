#include <iostream>
#include <string>
#include <algorithm>
#include <utility>
#include <vector>
#include <functional>
#include <map>

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

    NFA(int size) : size(size), graph(size) {}

    void set_start(int vertex) {
        start_vertex = vertex;
    }

    void set_end(int vertex) {
        if (!graph[vertex].is_end) {
            graph[vertex].is_end = true;
            end_vertices.push_back(vertex);
        }
    }

    void add_edge(int from, int to, const std::string& symbol) {
        graph[from].edges.emplace_back(to, symbol);
    }

    void add_bidirectional_edge(int from, int to, const std::string& symbol) {
        add_edge(from, to, symbol);
        add_edge(to, from, symbol);
    }

    int add_vertices(int count) {
        size += count;
        graph.resize(size);
        return size - count;
    }

    void remove_all_end_vertices() {
        while (!end_vertices.empty()) {
            graph[end_vertices.back()].is_end = false;
            end_vertices.pop_back();
        }
    }

    int merge(const NFA& other) {
        int shift = size;
        add_vertices(other.size);
        for (int i = 0; i < other.size; ++i) {
            for (auto& edge : other.graph[i].edges) {
                graph[i + shift].edges.emplace_back(edge.to + shift, edge.symbol);
            }
        }
        return shift;
    }

    void print() {
        std::cout << start_vertex << "\n\n";
        for (int i : end_vertices) {
            std::cout << i << '\n';
        }
        std::cout << '\n';
        for (int i = 0; i < size; ++i) {
            for (auto& edge : graph[i].edges) {
                std::cout << i << ' ' << edge.to << ' ' << edge.symbol << '\n';
            }
        }
    }
};

struct RegExTreeBuilder {
    struct Vertex {
        std::string operation;
        std::vector<int> edges;
        std::function<NFA(int, std::vector<Vertex>&)> builder;

        Vertex(std::string  operation, std::function<NFA(int, std::vector<Vertex>&)> builder) :
            operation(std::move(operation)), builder(std::move(builder)) {}

        Vertex(std::string  operation, std::vector<int> edges, std::function<NFA(int, std::vector<Vertex>&)> builder) :
                operation(std::move(operation)), edges(std::move(edges)), builder(std::move(builder)) {}
    };

    struct RegExParseOperation {
        std::string target;
        std::function<void(int, std::vector<Vertex>&, std::string&)> handler;
    };

    static const std::vector<RegExParseOperation> operations;


    std::string regex;


    std::vector<Vertex> graph;

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

    static void KleeneStarHandler(int pos, std::vector<Vertex>& graph, std::string& regex) {
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


    RegExTreeBuilder(std::string& regex) : regex(regex) {}

    void build() {
        std::string result;
        for (char c : regex) {
            if (c == ' ') {
                continue;
            }
            result.push_back(c);
        }
        regex = result;
        while (true) {
            bool is_found = false;
            for (int i = 0; i < regex.size(); ++i) {
                if (isalpha(regex[i])) {
                    AlphaHandler(i, graph, regex);
                    is_found = true;
                    break;
                }
            }
            if (!is_found) {
                break;
            }
        }
        std::cout << "After removing charactes: " << regex << '\n';

        build_operations();
    }

    int build_operations() {
        while (true) {
            bool is_found = false;
            for (auto& op : operations) {
                auto pos = regex.find(op.target);
                if (pos == std::string::npos) {
                    continue;
                }
                is_found = true;
                op.handler(pos, graph, regex);
                std::cout << regex << '\n';
                break;
            }
            if (!is_found) {
                break;
            }
        }
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
        for (auto end_vertex : nfa.end_vertices) {
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
        for (auto end_vertex : nfa.end_vertices) {
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
                for (int end_vertex : nfa.end_vertices) {
                    nfa.add_edge(end_vertex, shift + nfa_op.start_vertex, "");
                }
                nfa.remove_all_end_vertices();
                for (int end_vertex : nfa_op.end_vertices) {
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
        for (int end_vertex : nfa2.end_vertices) {
            nfa.set_end(shift + end_vertex);
        }

        return nfa;
    }

    static NFA MultiplyBuilder(int pos, std::vector<Vertex>& graph) {
        NFA nfa = nfa_builder(graph[pos].edges.front(), graph);
        NFA nfa2 = nfa_builder(graph[pos].edges.back(), graph);

        int shift = nfa.merge(nfa2);
        for (int end_vertex : nfa.end_vertices) {
            nfa.add_edge(end_vertex, shift + nfa2.start_vertex, "");
        }
        nfa.remove_all_end_vertices();
        for (int end_vertex : nfa2.end_vertices) {
            nfa.set_end(shift + end_vertex);
        }

        return nfa;
    }

    NFA build_nfa() {
        return nfa_builder(graph.size() - 1, graph);
    }

    static NFA nfa_builder(int pos, std::vector<Vertex>& graph) {
        return graph[pos].builder(pos, graph);
    }
};

const std::vector<RegExTreeBuilder::RegExParseOperation> RegExTreeBuilder::operations = {
        RegExParseOperation{"#*", RegExTreeBuilder::KleeneStarHandler},
        RegExParseOperation{"#%", RegExTreeBuilder::KleenePlusHandler},
        RegExParseOperation{"#^", RegExTreeBuilder::PowerHandler},
        RegExParseOperation{"##", RegExTreeBuilder::MultiplyHandler},
        RegExParseOperation{"#+#", RegExTreeBuilder::PlusHandler}
};


int main() {
    std::string regex;
    std::getline(std::cin, regex);
    RegExTreeBuilder builder(regex);
    builder.build();
    NFA nfa =  builder.build_nfa();
    nfa.print();
    return 0;
}
