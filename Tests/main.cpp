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
}

TEST(LSystem, breakDownRules)
{
  std::string axiom = "FFFA";
  std::vector<std::string> rules = {"A=![B]////[B]////B", "B=FFFA"};
  LSystem L(axiom,rules);

  EXPECT_EQ(L.m_rules.size(),2);
  EXPECT_EQ(L.m_rules[0].m_LHS,"A");
  EXPECT_EQ(L.m_rules[0].m_RHS[0],"![B]////[B]////B");
  EXPECT_EQ(L.m_rules[1].m_LHS,"B");
  EXPECT_EQ(L.m_rules[1].m_RHS[0],"FFFA");

  EXPECT_EQ(L.m_nonTerminals,"[AB]+");
}

TEST(LSystem, detectBranching)
{
  std::string axiom = "FFFA";
  std::vector<std::string> rules = {"A=[B]///[B]///[B]", "B=FF[C/C][C]", "C=FF[FFF]//[A]"};
  LSystem L(axiom,rules);

  EXPECT_EQ(L.m_branches.size(),4);
  EXPECT_EQ(L.m_branches[0],"B");
  EXPECT_EQ(L.m_branches[1],"C/C");
  EXPECT_EQ(L.m_branches[2],"C");
  EXPECT_EQ(L.m_branches[3],"A");
}

TEST(LSystem, generateTreeString)
{
  std::string axiom = "FFFA";
  std::vector<std::string> rules = {"A=![B]////[B]////B", "B=FFFA"};
  LSystem L(axiom,rules);
  L.m_generation=0;
  EXPECT_EQ(L.generateTreeString(),"FFFA");
  L.m_generation=1;
  EXPECT_EQ(L.generateTreeString(),"FFF![B]////[B]////B");
  L.m_generation=2;
  EXPECT_EQ(L.generateTreeString(),"FFF![FFFA]////[FFFA]////FFFA");
  L.m_generation=3;
  EXPECT_EQ(L.generateTreeString(),"FFF![FFF![B]////[B]////B]////[FFF![B]////[B]////B]////FFF![B]////[B]////B");
}

TEST(LSystem, createGeometry)
{
  std::string axiom = "FFFA";
  std::vector<std::string> rules = {"A=![B]////[B]////B", "B=FFFA"};
  LSystem L(axiom,rules);
  L.m_generation=0;
  L.createGeometry();
  L.m_stepSize = 2;

  EXPECT_EQ(L.m_vertices.size(),4);
  EXPECT_EQ(L.m_vertices[0],ngl::Vec3(0,0,0));
  EXPECT_EQ(L.m_vertices[1],ngl::Vec3(0,2,0));
  EXPECT_EQ(L.m_vertices[2],ngl::Vec3(0,4,0));
  EXPECT_EQ(L.m_vertices[3],ngl::Vec3(0,6,0));

  EXPECT_EQ(L.m_indices.size(),6);
  EXPECT_EQ(L.m_indices[0],0);
  EXPECT_EQ(L.m_indices[1],1);
  EXPECT_EQ(L.m_indices[2],1);
  EXPECT_EQ(L.m_indices[3],2);
  EXPECT_EQ(L.m_indices[4],2);
  EXPECT_EQ(L.m_indices[5],3);
}
