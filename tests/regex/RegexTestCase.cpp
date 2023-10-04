#include "RegexTestCase.h"
#include "RegexTreeBuilder.h"
#include "NFA.h"

TEST_F(RegexTestCase, Regex1) {
    std::string regex = "(ab)*a*";
    RegExTreeBuilder builder(regex);
    builder.build();
    NFA nfa =  builder.build_nfa();

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

TEST_F(RegexTestCase, Regex2) {
    std::string regex = "a( a ( a     b )  * a (   a   b ) * + b)      *";
    RegExTreeBuilder builder(regex);
    builder.build();
    NFA nfa =  builder.build_nfa();

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