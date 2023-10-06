#include <string>
#include <iostream>
#include "RegexTreeBuilder.h"

RegExTreeBuilder::Vertex::Vertex(std::string  operation, std::function<NFA(int, std::vector<Vertex>&)> builder) :
    operation(std::move(operation)), builder(std::move(builder)) {}
RegExTreeBuilder::Vertex::Vertex(std::string  operation, std::vector<int> edges, std::function<NFA(int, std::vector<Vertex>&)> builder) :
    operation(std::move(operation)), edges(std::move(edges)), builder(std::move(builder)) {}

RegExTreeBuilder::RegExTreeBuilder(const std::string& regex) : regex(regex) {}

void RegExTreeBuilder::build() {
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
            if (isalpha(regex[i]) || (regex[i] == '!')) {
                AlphaHandler(i, graph, regex);
                is_found = true;
                break;
            }
        }
        if (!is_found) {
            break;
        }
    }
//    std::cout << "After removing charactes: " << regex << '\n';

    build_operations();
}

int RegExTreeBuilder::build_operations() {
    while (true) {
        bool is_found = false;
        for (auto& op : operations) {
            auto pos = regex.find(op.target);
            if (pos == std::string::npos) {
                continue;
            }
            is_found = true;
            op.handler(pos, graph, regex);
//            std::cout << regex << '\n';
            break;
        }
        if (!is_found) {
            break;
        }
    }
    return 0;
}

NFA RegExTreeBuilder::build_nfa() {
    return nfa_builder(graph.size() - 1, graph);
}

const std::vector<RegExTreeBuilder::RegExParseOperation> RegExTreeBuilder::operations = {
        RegExParseOperation{"#*", RegExTreeBuilder::KleeneStarHandler},
        RegExParseOperation{"#%", RegExTreeBuilder::KleenePlusHandler},
        RegExParseOperation{"#^", RegExTreeBuilder::PowerHandler},
        RegExParseOperation{"##", RegExTreeBuilder::MultiplyHandler},
        RegExParseOperation{"#+#", RegExTreeBuilder::PlusHandler}
};
