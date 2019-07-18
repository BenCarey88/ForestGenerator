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
  LSystem L(axiom,rules,2,0.9f,30,0.9f,4);
  EXPECT_EQ(L.m_axiom,axiom);
}

TEST(LSystem, breakDownRules)
{
  std::string axiom = "FFFA";
  std::vector<std::string> rules = {"A=![B]////[B]////B", "B=FFFA"};
  LSystem L(axiom,rules,2,0.9f,30,0.9f,4);

  EXPECT_EQ(L.m_rules.size(),2);
  EXPECT_EQ(L.m_rules[0].m_LHS,"A");
  EXPECT_EQ(L.m_rules[0].m_RHS[0],"![B]////[B]////B");
  EXPECT_EQ(L.m_rules[1].m_LHS,"B");
  EXPECT_EQ(L.m_rules[1].m_RHS[0],"FFFA");

  EXPECT_EQ(L.m_nonTerminals,"[AB]+");
}

TEST(LSystem, countBranches)
{
  std::string axiom = "FFFA";
  std::vector<std::string> rules = {"A=[B]///[B]///[B]", "A=B", "B=FF[C/C][C]", "C=FF[FFF]//[A]"};
  LSystem L(axiom,rules,2,0.9f,30,0.9f,4);

  L.countBranches();
  EXPECT_EQ(L.m_rules[0].m_numBranches,std::vector<int>({3,0}));
  EXPECT_EQ(L.m_rules[1].m_numBranches,std::vector<int>({2}));
  EXPECT_EQ(L.m_rules[2].m_numBranches,std::vector<int>({1}));
}

TEST(LSystem, generateTreeString)
{
  std::string axiom = "FFFA";
  std::vector<std::string> rules = {"A=![B]////[B]////B", "B=FFFA"};
  LSystem L(axiom,rules,2,0.9f,30,0.9f,4);
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
  LSystem L(axiom,rules,2,0.9f,30,0.9f,0);
  L.createGeometry();

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

TEST(LSystem, addInstancingCommands)
{
  std::string axiom = "FFFA";
  std::vector<std::string> rules = {"A=![B]//[f]//[C/C]////B:2", "B=FFFA", "A=F//[B]//F:3", "C=F[A]"};
  LSystem L(axiom,rules,2,0.9f,30,0.9f,6);
  L.m_instancingProb = 0.6f;

  L.addInstancingCommands();

  EXPECT_EQ(L.m_rules[0].m_RHS[0],"!@(0,#)//[f]//@(1,#)////B");
  EXPECT_EQ(L.m_rules[0].m_RHS[1],"!{(0,#)[B]}(0,#)//[f]//@(1,#)////B");
  EXPECT_EQ(L.m_rules[0].m_RHS[2],"!@(0,#)//[f]//{(1,#)[C/C]}(1,#)////B");
  EXPECT_EQ(L.m_rules[0].m_RHS[3],"!{(0,#)[B]}(0,#)//[f]//{(1,#)[C/C]}(1,#)////B");
  EXPECT_EQ(L.m_rules[0].m_RHS[4],"F//@(0,#)//F");
  EXPECT_EQ(L.m_rules[0].m_RHS[5],"F//{(0,#)[B]}(0,#)//F");

  EXPECT_FLOAT_EQ(L.m_rules[0].m_prob[0],0.144);
  EXPECT_FLOAT_EQ(L.m_rules[0].m_prob[1],0.096);
  EXPECT_FLOAT_EQ(L.m_rules[0].m_prob[2],0.096);
  EXPECT_FLOAT_EQ(L.m_rules[0].m_prob[3],0.064);
  EXPECT_FLOAT_EQ(L.m_rules[0].m_prob[4],0.36);
  EXPECT_FLOAT_EQ(L.m_rules[0].m_prob[5],0.24);

  EXPECT_EQ(L.m_branches.size(),3);
  EXPECT_EQ(L.m_branches[0],"B");
  EXPECT_EQ(L.m_branches[1],"C/C");
  EXPECT_EQ(L.m_branches[2],"A");
}

