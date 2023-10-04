#include <iostream>
#include <string>
#include "NFA.h"
#include "RegexTreeBuilder.h"

int main() {
    std::string regex;
    std::getline(std::cin, regex);
    RegExTreeBuilder builder(regex);
    builder.build();
    NFA nfa =  builder.build_nfa();
    std::cout << nfa.to_string() << '\n';
    return 0;
}
