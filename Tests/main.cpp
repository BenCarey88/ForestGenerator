#include <gtest/gtest.h>
#include "LSystem.h"

int main(int argc, char *argv[])
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}


//--------------------------------------------------------------------------------------------------------------------
///TESTS
//--------------------------------------------------------------------------------------------------------------------
TEST(LSystem, UserCtor)
{
  std::string axiom = "FFFA";
  std::vector<std::string> rules = {"A=![B]////[B]////B", "B=FFFA"};
  LSystem L(axiom,rules);
  EXPECT_EQ(L.m_axiom,axiom);
  EXPECT_EQ(L.m_rules,rules);
}

TEST(LSystem, generateTreeString)
{
  std::string axiom = "FFFA";
  std::vector<std::string> rules = {"A=![B]////[B]////B", "B=FFFA"};
  LSystem L(axiom,rules);
  EXPECT_EQ(L.generateTreeString(0),"FFFA");
  EXPECT_EQ(L.generateTreeString(1),"FFF![B]////[B]////B");
  EXPECT_EQ(L.generateTreeString(2),"FFF![FFFA]////[FFFA]////FFFA");
  EXPECT_EQ(L.generateTreeString(3),"FFF![FFF![B]////[B]////B]////[FFF![B]////[B]////B]////FFF![B]////[B]////B");
}
