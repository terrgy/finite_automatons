#include "NFATestCase.h"
#include "NFA.h"
#include <vector>

TEST_F(NFATestCase, CreationTest) {
    NFA nfa(5);
    ASSERT_TRUE(nfa.size() == 5);
    ASSERT_TRUE(nfa.graph.size() == 5);

    nfa.set_start(3);
    ASSERT_TRUE(nfa.start_vertex == 3);

    nfa.set_end(0);
    nfa.set_end(1);
    nfa.set_end(2);
    std::vector<size_t> test = {0, 1, 2};
    ASSERT_TRUE(nfa.terminal_vertices == test);
    for (auto i : {0, 1, 2}) {
        ASSERT_TRUE(nfa.graph[i].is_terminal);
    }

    nfa.remove_all_end_vertices();
    ASSERT_TRUE(nfa.terminal_vertices.empty());
    for (auto i : {0, 1, 2}) {
        ASSERT_FALSE(nfa.graph[i].is_terminal);
    }

    nfa.add_bidirectional_edge(0, 1, "a");
    ASSERT_TRUE(nfa.graph[0].edges.size() == 1);
    ASSERT_TRUE(nfa.graph[0].edges[0].to == 1);
    ASSERT_TRUE(nfa.graph[0].edges[0].symbol == "a");
    ASSERT_TRUE(nfa.graph[1].edges.size() == 1);
    ASSERT_TRUE(nfa.graph[1].edges[0].to == 0);
    ASSERT_TRUE(nfa.graph[1].edges[0].symbol == "a");

    nfa.add_vertices(1);
    ASSERT_TRUE(nfa.size() == 6);
    ASSERT_TRUE(nfa.graph.size() == 6);

    NFA copy = nfa;
    nfa.merge(copy);
    ASSERT_TRUE(nfa.size() == 12);
    ASSERT_TRUE(nfa.graph[6].edges[0].to == 7);

    nfa.set_end(2);

    ASSERT_EQ(nfa.to_string(), "3\n\n2\n\n0 1 a\n1 0 a\n6 7 a\n7 6 a\n");
}

TEST_F(NFATestCase, AceeptsTest) {
    NFA nfa = NFA(8);
    nfa.set_start(4);
    nfa.set_end(7);

    nfa.add_edge(0, 1, "a");
    nfa.add_edge(1, 2, "");
    nfa.add_edge(2, 3, "b");
    nfa.add_edge(3, 4, "");
    nfa.add_edge(4, 0, "");
    nfa.add_edge(4, 7, "");
    nfa.add_edge(7, 5, "");
    nfa.add_edge(5, 6, "a");
    nfa.add_edge(6, 7, "");

    ASSERT_TRUE(nfa.accepts("ababaa"));
    ASSERT_TRUE(nfa.accepts(""));
    ASSERT_TRUE(nfa.accepts("a"));
    ASSERT_TRUE(nfa.accepts("ab"));
    ASSERT_TRUE(nfa.accepts("aaaaaaaaaaaaaaaaaaa"));
    ASSERT_TRUE(nfa.accepts("ababababababababababab"));
    ASSERT_TRUE(nfa.accepts("ababababababababababababaaaaaaaaaaaaaaaa"));

    ASSERT_FALSE(nfa.accepts("aaaaab"));
    ASSERT_FALSE(nfa.accepts("lkgdjfnglkjdfhtg"));
    ASSERT_FALSE(nfa.accepts("lolkekcheburek"));
    ASSERT_FALSE(nfa.accepts("ababababababaaaaaaaaaaaaaab"));
    ASSERT_FALSE(nfa.accepts("babababab"));
    ASSERT_FALSE(nfa.accepts("aabababababababab"));
}

TEST_F(NFATestCase, EmptyTest) {
    NFA nfa;

    ASSERT_TRUE(nfa.accepts(""));

    ASSERT_FALSE(nfa.accepts("sadsfasdfdsaf"));
    ASSERT_FALSE(nfa.accepts("a"));
    ASSERT_FALSE(nfa.accepts("safas"));
}

TEST_F(NFATestCase, EmptyEdgesClosure) {
    NFA nfa(20);
    nfa.set_start(0);
    nfa.set_end(19);

    nfa.add_edge(0, 1, "a");
    nfa.add_edge(1, 19, "");
    nfa.add_edge(2, 3, "a");
    nfa.add_edge(3, 8, "");
    nfa.add_edge(4, 5, "a");
    nfa.add_edge(5, 6, "");
    nfa.add_edge(6, 7, "b");
    nfa.add_edge(7, 8, "");
    nfa.add_edge(8, 4, "");
    nfa.add_edge(8, 9, "");
    nfa.add_edge(9, 10, "a");
    nfa.add_edge(10, 15, "");
    nfa.add_edge(11, 12, "a");
    nfa.add_edge(12, 13, "");
    nfa.add_edge(13, 14, "b");
    nfa.add_edge(14, 15, "");
    nfa.add_edge(15, 11, "");
    nfa.add_edge(15, 19, "");
    nfa.add_edge(16, 17, "b");
    nfa.add_edge(17, 19, "");
    nfa.add_edge(18, 2, "");
    nfa.add_edge(18, 16, "");
    nfa.add_edge(19, 18, "");

    ASSERT_TRUE(nfa.accepts("a"));
    ASSERT_TRUE(nfa.accepts("abbbbbbbbbb"));
    ASSERT_TRUE(nfa.accepts("aaabaaba"));
    ASSERT_TRUE(nfa.accepts("aaabaabaabaabaaaaaabbbbb"));
    ASSERT_TRUE(nfa.accepts("aaaaaaaaaaabaaaaaabab"));
    ASSERT_TRUE(nfa.accepts("abbbbbbbbbbbbbbbbbbbbbbaabaabbbbbbbbbbb"));
    ASSERT_TRUE(nfa.accepts("aaababababababababbb"));

    ASSERT_FALSE(nfa.accepts(""));
    ASSERT_FALSE(nfa.accepts("aab"));
    ASSERT_FALSE(nfa.accepts("aaabababaabba"));
    ASSERT_FALSE(nfa.accepts("aa"));
    ASSERT_FALSE(nfa.accepts("abbbbbab"));
    ASSERT_FALSE(nfa.accepts("babab"));
    ASSERT_FALSE(nfa.accepts("asdsadsad"));

    nfa.build_empty_edges_closure();

    ASSERT_TRUE(nfa.accepts("a"));
    ASSERT_TRUE(nfa.accepts("abbbbbbbbbb"));
    ASSERT_TRUE(nfa.accepts("aaabaaba"));
    ASSERT_TRUE(nfa.accepts("aaabaabaabaabaaaaaabbbbb"));
    ASSERT_TRUE(nfa.accepts("aaaaaaaaaaabaaaaaabab"));
    ASSERT_TRUE(nfa.accepts("abbbbbbbbbbbbbbbbbbbbbbaabaabbbbbbbbbbb"));
    ASSERT_TRUE(nfa.accepts("aaababababababababbb"));

    ASSERT_FALSE(nfa.accepts(""));
    ASSERT_FALSE(nfa.accepts("aab"));
    ASSERT_FALSE(nfa.accepts("aaabababaabba"));
    ASSERT_FALSE(nfa.accepts("aa"));
    ASSERT_FALSE(nfa.accepts("abbbbbab"));
    ASSERT_FALSE(nfa.accepts("babab"));
    ASSERT_FALSE(nfa.accepts("asdsadsad"));
}

TEST_F(NFATestCase, GraphvizTest) {
    NFA nfa(20);
    nfa.set_start(0);
    nfa.set_end(19);

    nfa.add_edge(0, 1, "a");
    nfa.add_edge(1, 19, "");
    nfa.add_edge(2, 3, "a");
    nfa.add_edge(3, 8, "");
    nfa.add_edge(4, 5, "a");
    nfa.add_edge(5, 6, "");
    nfa.add_edge(6, 7, "b");
    nfa.add_edge(7, 8, "");
    nfa.add_edge(8, 4, "");
    nfa.add_edge(8, 9, "");
    nfa.add_edge(9, 10, "a");
    nfa.add_edge(10, 15, "");
    nfa.add_edge(11, 12, "a");
    nfa.add_edge(12, 13, "");
    nfa.add_edge(13, 14, "b");
    nfa.add_edge(14, 15, "");
    nfa.add_edge(15, 11, "");
    nfa.add_edge(15, 19, "");
    nfa.add_edge(16, 17, "b");
    nfa.add_edge(17, 19, "");
    nfa.add_edge(18, 2, "");
    nfa.add_edge(18, 16, "");
    nfa.add_edge(19, 18, "");
    nfa.build_empty_edges_closure();

    std::string correct_output = "digraph NFA {\n"
    "node [style=\"filled\"];\n"
    "\"0\" -> \"1\" [label=\"a\"];\n"
    "\"1\" -> \"2\" [label=\"a\"];\n"
    "\"1\" -> \"8\" [label=\"b\"];\n"
    "\"2\" -> \"3\" [label=\"a\"];\n"
    "\"2\" -> \"5\" [label=\"a\"];\n"
    "\"3\" -> \"4\" [label=\"b\"];\n"
    "\"4\" -> \"3\" [label=\"a\"];\n"
    "\"4\" -> \"5\" [label=\"a\"];\n"
    "\"5\" -> \"2\" [label=\"a\"];\n"
    "\"5\" -> \"6\" [label=\"a\"];\n"
    "\"5\" -> \"8\" [label=\"b\"];\n"
    "\"6\" -> \"7\" [label=\"b\"];\n"
    "\"7\" -> \"2\" [label=\"a\"];\n"
    "\"7\" -> \"6\" [label=\"a\"];\n"
    "\"7\" -> \"8\" [label=\"b\"];\n"
    "\"8\" -> \"2\" [label=\"a\"];\n"
    "\"8\" -> \"8\" [label=\"b\"];\n"
    "\"0\" [fillcolor=\"green\"];\n"
    "\"1\" [fillcolor=\"red\"];\n"
    "\"5\" [fillcolor=\"red\"];\n"
    "\"7\" [fillcolor=\"red\"];\n"
    "\"8\" [fillcolor=\"red\"];\n"
    "}\n";

    ASSERT_EQ(nfa.to_graphviz(), correct_output);
}

TEST_F(NFATestCase, ClearTest) {
    NFA nfa;
    nfa.clear();

    ASSERT_EQ(nfa.size(), 0);
    ASSERT_TRUE(nfa.graph.empty());
    ASSERT_TRUE(nfa.terminal_vertices.empty());
    ASSERT_EQ(nfa.start_vertex, 0);
}