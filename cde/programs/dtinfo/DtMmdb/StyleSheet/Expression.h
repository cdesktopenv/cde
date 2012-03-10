/* $XConsortium: Expression.h /main/4 1996/08/21 15:50:17 drk $ */
#ifndef _Expression_h
#define _Expression_h

/* **************************************************************
   Defines the tree for evaluating expressions given as feature values
   in the Style Sheet
 * ************************************************************** */



#include "SymTab.h"

#ifndef CDE_NEXT

typedef dlist_array<Symbol> f_items_t;
#else
#include "dti_cc/cc_povec.h"
typedef dlist_array<Symbol> f_items_t;
#endif


class FeatureValue;
class TermNode;

// /////////////////////////////////////////////////////////////////////////
// 	class Expression
//
//	holds root of expression tree 
// /////////////////////////////////////////////////////////////////////////


class Expression
{
public:
  Expression(TermNode *root);
  Expression(const Expression&);
  ~Expression();

  virtual FeatureValue *evaluate() const;
  ostream &print(ostream &) const; 

private:
  TermNode* f_root;
};

class TermNode
{
public:
  virtual ~TermNode();
  virtual FeatureValue *evaluate() const = 0;
  virtual ostream &print(ostream &) const = 0;
  virtual TermNode *clone() const = 0;
};

class VariableNode: public TermNode
{
  // for single name variables eg: "DEFAULT_FONT" 
public:
  VariableNode(const Symbol& name);

  virtual FeatureValue *evaluate() const;
  ostream &print(ostream &) const;

  virtual TermNode *clone() const;

private:
  Symbol f_name;

};

class CompositeVariableNode : public TermNode
{
  // for feature path variables (font.size)
  // eg: font: { size: font.size }
public:
  CompositeVariableNode();
  CompositeVariableNode(size_t capac); /* if we know how many items to expect */
  ~CompositeVariableNode();

  virtual FeatureValue *evaluate() const ;
  ostream &print(ostream &) const;

  virtual TermNode *clone() const;

  void prependItem(const Symbol& item);
  void appendItem(const Symbol& item);

  const Symbol* convertableToVariable();

private:
  //dlist_array<Symbol>	f_items;
  f_items_t f_items;
};


class BinaryOperatorNode: public TermNode
{
public:
  enum operatorType { PLUS, MINUS, TIMES, DIVIDE };

  BinaryOperatorNode(operatorType, TermNode* left, TermNode* right);
  ~BinaryOperatorNode();

  virtual TermNode *clone() const;

  virtual FeatureValue *evaluate() const;
  ostream &print(ostream &) const;


private:
  operatorType	f_operator ;
  TermNode     *f_left;
  TermNode     *f_right;

};

class SgmlAttributeNode: public TermNode
{
public:
  SgmlAttributeNode(const Symbol& name);
  SgmlAttributeNode();

  virtual FeatureValue	*evaluate() const ;
  ostream &print(ostream &) const;

  virtual TermNode *clone() const;


private:
  Symbol      f_name;
};

class ConstantNode: public TermNode
{
public:
  ConstantNode(FeatureValue*);
  ~ConstantNode();
  virtual FeatureValue *evaluate() const;
  ostream &print(ostream &) const;

  virtual TermNode *clone() const;

private:
  FeatureValue *f_value;

};

ostream &operator <<(ostream &, const Expression &);
ostream &operator <<(ostream &, const TermNode &);


#endif /* _Expression_h */
/* DO NOT ADD ANY LINES AFTER THIS #endif */
