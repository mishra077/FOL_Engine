# FOL_Engine
The knowledge bases contain sentences with the following defined operators:


NOT X               ~X


X OR Y             X | Y


X AND Y            X & Y


X IMPLIES Y       X => Y


The program takes a query of n actions and provides a logical conclusion to whether each can be performed or not.

**Format for input.txt:**<br />
**<N = NUMBER OF QUERIES>** <br />
**<QUERY 1>**<br />
**...**<br />
**<QUERY N>**<br />
**<K = NUMBER OF GIVEN SENTENCES IN THE KNOWLEDGE BASE>**<br />
**<SENTENCE 1>**<br />
**...**<br />
**<SENTENCE K>**<br />

The first line contains an integer *N* specifying the number of queries. 
The following *N* lines contain one query per line. 
The line after the last query contains an integer *K* specifying the number of sentences in the knowledge base.
The remaining *K* lines contain the sentences in the knowledge base, one sentence per line.
<br />
<br />
***Query Format:*** Each query will be e a single literal of the form Predicate(Constant_Arguments) or ~Predicate(Constant_Arguments) and will not contain any variables.
Each predicate will have between 1 and 25 constant arguments. 
Two or more arguments will be separated by commas.
<br />
<br />
***KB Format:*** Each sentence in the knowledge base is written in one of the following forms:<br />
1. An implication of the form *p1 ∧ p2 ∧... ∧ pm ⇒ q*, where its premise is a conjunction of literals and its conclusion is a single literal. 
Remember that a literal is an atomic sentence or a negated atomic sentence.
2. A single literal: *q or ~q*.
<br />
<br />
NOTES:<br />

1. & denotes the conjunction operator.
2. | denotes the disjunction operator. It will not appear in the queries nor in the KB given as input. But you will likely need it to create your proofs.
3. => denotes the implication operator.
4. ~ denotes the negation operator.
5. No other operators besides &, =>, and ~ are used in the knowledge base.
6. There will be no parentheses in the KB except as used to denote arguments of predicates.
7. Variables are denoted by a single lowercase letters.
8. All predicates  (such  as Vaccinated)  and  constants  (such  as Hayley)  are  case  sensitive alphabetical strings that begin with uppercase letters.
9. Each predicate takes at least one argument. Predicates will take at most 25 arguments. A given predicate name will not appear with different number of arguments.
10. There will be at most 10 queries and 100 sentences in the knowledge base.
11. The KB will be true (i.e., will not contain contradiction.
12. Variables are assumed to be universally quantified. There is no existential quantifier in this homework. There is no need for Skolem functions or Skolem constants.
13. If you run into a loop and there is no alternative path you can try, report FALSE. An example for this would be having two rules (1)A(x) =>B(x) and (2)B(x) =>A(x)  and wanting to prove A(Teddy). In this case your program should report FALSE.
14. Note that the KB is not in Horn form because we allow more than one positive literal. So you indeed must use resolution and cannot use generalized Modus Ponus.
