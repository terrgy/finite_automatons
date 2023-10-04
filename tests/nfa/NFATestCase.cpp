#include "NFATestCase.h"
#include "NFA.h"
#include <vector>

TEST_F(NFATestCase, CreationTest) {
    NFA nfa(5);
    ASSERT_TRUE(nfa.size == 5);
    ASSERT_TRUE(nfa.graph.size() == 5);

    nfa.set_start(3);
    ASSERT_TRUE(nfa.start_vertex == 3);

    nfa.set_end(0);
    nfa.set_end(1);
    nfa.set_end(2);
    std::vector<int> test = {0, 1, 2};
    ASSERT_TRUE(nfa.end_vertices == test);
    for (auto i : {0, 1, 2}) {
        ASSERT_TRUE(nfa.graph[i].is_end);
    }

    nfa.remove_all_end_vertices();
    ASSERT_TRUE(nfa.end_vertices.empty());
    for (auto i : {0, 1, 2}) {
        ASSERT_FALSE(nfa.graph[i].is_end);
    }

    nfa.add_bidirectional_edge(0, 1, "a");
    ASSERT_TRUE(nfa.graph[0].edges.size() == 1);
    ASSERT_TRUE(nfa.graph[0].edges[0].to == 1);
    ASSERT_TRUE(nfa.graph[0].edges[0].symbol == "a");
    ASSERT_TRUE(nfa.graph[1].edges.size() == 1);
    ASSERT_TRUE(nfa.graph[1].edges[0].to == 0);
    ASSERT_TRUE(nfa.graph[1].edges[0].symbol == "a");

    nfa.add_vertices(1);
    ASSERT_TRUE(nfa.size == 6);
    ASSERT_TRUE(nfa.graph.size() == 6);

    NFA copy = nfa;
    nfa.merge(copy);
    ASSERT_TRUE(nfa.size == 12);
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