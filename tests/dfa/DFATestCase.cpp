#include "DFATestCase.h"
#include "DFA.h"
#include "RegexTreeBuilder.h"

TEST_F(DFATestCase, SimpleTest) {
    RegExTreeBuilder builder("a(a(ab)*a(ab)* + b)*");
    builder.build();
    NFA nfa = builder.build_nfa();
    nfa.build_empty_edges_closure();
    DFA dfa(nfa);

    ASSERT_TRUE(dfa.accepts("a"));
    ASSERT_TRUE(dfa.accepts("abbbbbbbbbb"));
    ASSERT_TRUE(dfa.accepts("aaabaaba"));
    ASSERT_TRUE(dfa.accepts("aaabaabaabaabaaaaaabbbbb"));
    ASSERT_TRUE(dfa.accepts("aaaaaaaaaaabaaaaaabab"));
    ASSERT_TRUE(dfa.accepts("abbbbbbbbbbbbbbbbbbbbbbaabaabbbbbbbbbbb"));
    ASSERT_TRUE(dfa.accepts("aaababababababababbb"));

    ASSERT_FALSE(dfa.accepts(""));
    ASSERT_FALSE(dfa.accepts("aab"));
    ASSERT_FALSE(dfa.accepts("aaabababaabba"));
    ASSERT_FALSE(dfa.accepts("aa"));
    ASSERT_FALSE(dfa.accepts("abbbbbab"));
    ASSERT_FALSE(dfa.accepts("babab"));
    ASSERT_FALSE(dfa.accepts("asdsadsad"));
}

TEST_F(DFATestCase, FDFATest) {
    RegExTreeBuilder builder("a(a(ab)*a(ab)* + b)*");
    builder.build();
    NFA nfa = builder.build_nfa();
    nfa.build_empty_edges_closure();
    DFA dfa(nfa);
    dfa.build_FDFA();

    ASSERT_TRUE(dfa.accepts("a"));
    ASSERT_TRUE(dfa.accepts("abbbbbbbbbb"));
    ASSERT_TRUE(dfa.accepts("aaabaaba"));
    ASSERT_TRUE(dfa.accepts("aaabaabaabaabaaaaaabbbbb"));
    ASSERT_TRUE(dfa.accepts("aaaaaaaaaaabaaaaaabab"));
    ASSERT_TRUE(dfa.accepts("abbbbbbbbbbbbbbbbbbbbbbaabaabbbbbbbbbbb"));
    ASSERT_TRUE(dfa.accepts("aaababababababababbb"));

    ASSERT_FALSE(dfa.accepts(""));
    ASSERT_FALSE(dfa.accepts("aab"));
    ASSERT_FALSE(dfa.accepts("aaabababaabba"));
    ASSERT_FALSE(dfa.accepts("aa"));
    ASSERT_FALSE(dfa.accepts("abbbbbab"));
    ASSERT_FALSE(dfa.accepts("babab"));
    ASSERT_FALSE(dfa.accepts("asdsadsad"));
}

TEST_F(DFATestCase, ComplementTest) {
    RegExTreeBuilder builder("a(a(ab)*a(ab)* + b)*");
    builder.build();
    NFA nfa = builder.build_nfa();
    nfa.build_empty_edges_closure();
    DFA dfa(nfa);
    dfa.build_FDFA();
    dfa.rebuild_to_complement();

    ASSERT_FALSE(dfa.accepts("a"));
    ASSERT_FALSE(dfa.accepts("abbbbbbbbbb"));
    ASSERT_FALSE(dfa.accepts("aaabaaba"));
    ASSERT_FALSE(dfa.accepts("aaabaabaabaabaaaaaabbbbb"));
    ASSERT_FALSE(dfa.accepts("aaaaaaaaaaabaaaaaabab"));
    ASSERT_FALSE(dfa.accepts("abbbbbbbbbbbbbbbbbbbbbbaabaabbbbbbbbbbb"));
    ASSERT_FALSE(dfa.accepts("aaababababababababbb"));

    ASSERT_TRUE(dfa.accepts(""));
    ASSERT_TRUE(dfa.accepts("aab"));
    ASSERT_TRUE(dfa.accepts("aaabababaabba"));
    ASSERT_TRUE(dfa.accepts("aa"));
    ASSERT_TRUE(dfa.accepts("abbbbbab"));
    ASSERT_TRUE(dfa.accepts("babab"));
}

TEST_F(DFATestCase, IteratorsTest) {
    DFAVertex vertex;
    vertex.add_edge(1, 'a');
    vertex.add_edge(2, 'b');
    vertex.add_edge(3, 'c');
    vertex.add_edge(14, 'd');

    auto it = vertex.begin();
    ASSERT_EQ((it.operator->())->to(), 1);
    auto prev = it++;
    ASSERT_EQ(it->to(), 2);
    ASSERT_EQ(prev->to(), 1);

    prev = it--;
    ASSERT_EQ(it->to(), 1);
    ASSERT_EQ(prev->to(), 2);
    ASSERT_EQ((--prev)->to(), 1);

    std::vector<size_t> test = {14, 3, 2, 1};
    std::vector<size_t> result;
    for (auto rit = vertex.rbegin(); rit != vertex.rend(); ++rit) {
        result.push_back((*rit).to());
    }
    ASSERT_EQ(result, test);
    result.clear();
    const DFAVertex cvertex = vertex;
    for (auto rit = cvertex.rbegin(); rit != cvertex.rend(); ++rit) {
        result.push_back((*rit).to());
    }
    ASSERT_EQ(result, test);

    result.clear();
    test = {1, 2, 3, 14};
    for (auto cit = cvertex.begin(); cit != cvertex.end(); ++cit) {
        result.push_back(cit->to());
    }
    ASSERT_EQ(result, test);
}

TEST_F(DFATestCase, EdgesTest) {
    DFAVertex vertex;
    vertex.add_edge(1, 'a');
    vertex.add_edge(2, 'b');
    vertex.add_edge(3, 'c');
    vertex.add_edge(14, 'd');

    ASSERT_EQ(vertex._edges.size(), 4);
    vertex.remove_multiple_edges();
    ASSERT_EQ(vertex._edges.size(), 4);
    vertex.clear_edges();
    ASSERT_TRUE(vertex._edges.empty());
}

TEST_F(DFATestCase, Constructors1Test) {
    DFA dfa;
    ASSERT_TRUE(dfa.accepts(""));
    ASSERT_EQ(dfa._start_vertex, 0);
    ASSERT_TRUE(dfa._graph[0].is_terminal());
    ASSERT_EQ(dfa._size, 1);
    ASSERT_EQ(dfa._graph.size(), 1);
}

TEST_F(DFATestCase, Constructors2Test) {
    DFA dfa(5);
    ASSERT_EQ(dfa._start_vertex, 0);
    ASSERT_EQ(dfa._size, 5);
    ASSERT_EQ(dfa._graph.size(), 5);
}

TEST_F(DFATestCase, GraphvizTest) {
    RegExTreeBuilder builder("a(a(ab)*a(ab)* + b)*");
    builder.build();
    NFA nfa = builder.build_nfa();
    nfa.build_empty_edges_closure();
    DFA dfa(nfa);
    dfa.build_FDFA();

    std::string correct_output = "digraph NFA {\n"
                                 "node [style=\"filled\"];\n"
                                 "\"0\" -> \"1\" [label=\"a\"];\n"
                                 "\"0\" -> \"11\" [label=\"b\"];\n"
                                 "\"1\" -> \"2\" [label=\"a\"];\n"
                                 "\"1\" -> \"3\" [label=\"b\"];\n"
                                 "\"2\" -> \"4\" [label=\"a\"];\n"
                                 "\"2\" -> \"11\" [label=\"b\"];\n"
                                 "\"3\" -> \"2\" [label=\"a\"];\n"
                                 "\"3\" -> \"3\" [label=\"b\"];\n"
                                 "\"4\" -> \"5\" [label=\"a\"];\n"
                                 "\"4\" -> \"6\" [label=\"b\"];\n"
                                 "\"5\" -> \"4\" [label=\"a\"];\n"
                                 "\"5\" -> \"7\" [label=\"b\"];\n"
                                 "\"6\" -> \"8\" [label=\"a\"];\n"
                                 "\"6\" -> \"3\" [label=\"b\"];\n"
                                 "\"7\" -> \"5\" [label=\"a\"];\n"
                                 "\"7\" -> \"3\" [label=\"b\"];\n"
                                 "\"8\" -> \"9\" [label=\"a\"];\n"
                                 "\"8\" -> \"6\" [label=\"b\"];\n"
                                 "\"9\" -> \"9\" [label=\"a\"];\n"
                                 "\"9\" -> \"10\" [label=\"b\"];\n"
                                 "\"10\" -> \"9\" [label=\"a\"];\n"
                                 "\"10\" -> \"3\" [label=\"b\"];\n"
                                 "\"11\" -> \"11\" [label=\"a\"];\n"
                                 "\"11\" -> \"11\" [label=\"b\"];\n"
                                 "\"0\" [fillcolor=\"green\"];\n"
                                 "\"1\" [fillcolor=\"red\"];\n"
                                 "\"3\" [fillcolor=\"red\"];\n"
                                 "\"4\" [fillcolor=\"red\"];\n"
                                 "\"6\" [fillcolor=\"red\"];\n"
                                 "\"7\" [fillcolor=\"red\"];\n"
                                 "\"8\" [fillcolor=\"red\"];\n"
                                 "\"9\" [fillcolor=\"red\"];\n"
                                 "\"10\" [fillcolor=\"red\"];\n"
                                 "}\n";
    ASSERT_EQ(dfa.to_graphviz(), correct_output);
}