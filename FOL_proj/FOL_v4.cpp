#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <map>
#include <stack>
#include <algorithm>
#include <ctime>

std::clock_t start;
std::map<std::string, std::vector<std::string>> negative_facts;
std::map<std::string, std::vector<std::string>> positive_facts;
std::map<std::string, std::vector<std::vector<std::string>>> negative_equations;
std::map<std::string, std::vector<std::vector<std::string>>> positive_equations;
std::map<std::string, int> standard_variables;
std::map<std::string, int> standard_variables_f;

std::string Unification(std::string predicate, const std::string& variable, const std::string& constant) {
	size_t s_pos = predicate.find("(") + 1;

	while ((s_pos = predicate.find(variable, s_pos)) != std::string::npos) {
		if (predicate[s_pos - 1] == '(' || predicate[s_pos - 1] == ',') {
			predicate.replace(s_pos, variable.length(), constant);
			s_pos += constant.length();
		}
		else s_pos++;
	}
	return predicate;
}

std::vector<std::string> Unify(std::vector<std::string> query, std::string variable, std::string constant) {
	std::vector<std::string> query_resolved;
	for (int i = 0; i < query.size(); i++) {
		query_resolved.push_back(Unification(query[i], variable, constant));
	}
	return query_resolved;
}

std::vector<std::string> Resolution(std::vector<std::string> query, std::vector<std::string> equation) {
	int i = 0;
	int match = 0;
	while (i < query.size()) {
		if (query[i][0] == '~') {
			std::cout << "SEARCHING FOR POSITIVE LITERALS\n";
			std::string name = query[i].substr(1, query[i].find(")"));
			for (int j = 0; j < equation.size(); j++) {
				if (name == equation[j]) {
					match = 1;
					std::cout << "ERASED NAME: " << name << "\n";
					query.erase(query.begin() + i);
					equation.erase(equation.begin() + j);
					break;
				}
			}
			if (match == 1) {
				match = 0;
			}
			else i++;
		}
		else {
			std::cout << "SEARCHING FOR NEGATIVE LITERALS\n";
			std::string name = query[i].substr(0, query[i].find("("));
			for (int j = 0; j < equation.size(); j++) {
				if (name == equation[j].substr(1, equation[j].find("(") - 1)) {
					match = 1;
					std::cout << "ERASED NAME: " << name << "\n";
					query.erase(query.begin() + i);
					equation.erase(equation.begin() + j);
					break;
				}
			}
			if (match == 1) {
				match = 0;
			}
			else i++;
		}
	}
	for (int i = 0; i < equation.size(); i++) {
		query.push_back(equation[i]);
	}
	return query;
}

bool FOL_engine(std::vector<std::string> query, int i, int depth) { // i represent no of queries left - 1
	std::clock_t end = clock() / CLOCKS_PER_SEC;
	std::cout << "DEPTH:" << depth << "\n";
	if (depth <= 0 || (end - start) > 60) {
		return false;
	}
	bool result = false;
	if (query.empty()) {
		return true;
	}
	else {
		std::string clause = query[0];
		std::cout << "CLAUSE: " << clause << "\n";
		if (clause[0] == '~') {
			// Check in Positive Facts
			int e, f, g, h;
			std::string token = clause.substr(1, clause.find("(") - 1);
			std::cout << token << "\n";
			int cl_arg = std::count(clause.begin(), clause.end(), ',') + 1;
			//std::cout << positive_facts[token].size() << "\n";
			if (positive_facts.count(token) > 0) {
				std::cout << "IN POSITIVE FACTS:\n";
				for (int j = 0; j < positive_facts[token].size(); j++) {
					if (cl_arg == 1) {
						std::cout << "ARGUMENT = 1" << "\n";
						//std::cout << "YES" << "\n";
						e = clause.find("(");
						f = clause.find(")");
						g = positive_facts[token][j].find("(");
						h = positive_facts[token][j].find(")");
						//std::cout << positive_facts[token][j] << "\n";
						//std::cout << "POSITIVE_FACTS_SIZE: " << positive_facts[token].size() << "\n";
						if (clause.substr(e + 1, f - e - 1) == positive_facts[token][j].substr(g + 1, h - g - 1) && (isupper(clause.substr(e+1, f- e- 1)[0]) && isupper(positive_facts[token][j].substr(g + 1, h - g - 1)[0]))) {
							std::cout << "ARGUMENT EQUALS WITH THE ARGUMENT AND BOTH ARE CONSTANT\n";
							std::vector<std::string> query2;
							int k = i + 1;
							while (k < query.size()) {
								query2.push_back(query[k]);
								k++;
							}
							std::cout << "QUERY2 SIZE:" << query2.size() << "\n";
							if (query2.size()) std::cout << "Contents of Query2 \n";
							for (auto i = query2.begin(); i != query2.end(); ++i) std::cout << *i << "\n";
							result = FOL_engine(query2, 0, --depth);
							std::cout << "******************\n";
							if (result) return true;
						}
						else if (islower(clause.substr(e + 1, f - e - 1)[0]) && isupper(positive_facts[token][j].substr(g + 1, h - g - 1)[0])) {
							std::cout << "CLAUSE'S ARGUMENT IS A VARIABLE AND FACT IS CONSTANT\n";
							//std::cout << "YES\n";
							std::vector<std::string> query2 = Unify(query, clause.substr(e + 1, f - e - 1), positive_facts[token][j].substr(g + 1, h - g - 1));
							std::vector<std::string> query3;
							int k = i + 1;
							std::cout << "AFTER UNIFICATION:\n";
							for (auto i = query2.begin(); i != query2.end(); ++i) std::cout << *i << "\n";
							while (k < query2.size()) {
								query3.push_back(query2[k]);
								k++;
							}
							std::cout << "QUERY3 SIZE:" << query3.size() << "\n";
							if (query3.size()) std::cout << "Contents of Query3 \n";
							for (auto i = query3.begin(); i != query3.end(); ++i) std::cout << *i << "\n";

							result = FOL_engine(query3, 0, --depth);

							std::cout << "******************\n";
							if (result) return true;
						}
						else if (islower(positive_facts[token][j].substr(g + 1, h - g - 1)[0])) {
							std::cout << "BOTH ARE VARIABLES\n";
							std::vector<std::string> query2;
							int k = i + 1;
							while (k < query.size()) {
								query2.push_back(query[k]);
								k++;
							}
							std::cout << "QUERY2 SIZE:" << query2.size() << "\n";
							if (query2.size()) std::cout << "Contents of Query2 \n";
							for (auto i = query2.begin(); i != query2.end(); ++i) std::cout << *i << "\n";
							result = FOL_engine(query2, 0, --depth);
							std::cout << "******************\n";
							if (result) return true;

						}
					}
					if (cl_arg > 1) {
						std::cout << "ARGUMENT MORE THAN ONE\n";
						int match = 0;
						std::vector<std::string> query2 = query;
						std::string pf_str = positive_facts[token][j];
						// Case - I For first argument Eg: Play(x, _ , _ ) Play(Come, Abhi, OP)
						e = query2[i].find("(");
						f = query2[i].find(",");
						g = pf_str.find("(");
						h = pf_str.find(",");

						if (query2[i].substr(e + 1, f - e - 1) == pf_str.substr(g + 1, h - g - 1) && (isupper(query2[i].substr(e + 1, f - e - 1)[0]) && isupper(pf_str.substr(g + 1, h - g - 1)[0]))) {
							std::cout << "ARGUMENT EQUALS WITH THE ARGUMENT\n";
							match++;
						}
						else if (islower(query2[i].substr(e + 1, f - e - 1)[0]) && isupper(pf_str.substr(g + 1, h - g - 1)[0])) {
							//std::cout << "HELLO\n";
							std::cout << "CLAUSE ARGUMENT IS A VARIABLE\n";
							query2 = Unify(query2, query2[i].substr(e + 1, f - e - 1), pf_str.substr(g + 1, h - g - 1));
							std::cout << "AFTER UNIFICATION\n";
							//f = e + (pf_str.substr(g + 1, h - g - 1)).length() + 1;
							for (auto i = query2.begin(); i < query2.end(); ++i) std::cout << *i << "\n";
							match++;
						}
						else if (isupper(query2[i].substr(e + 1, f - e - 1)[0]) && islower(pf_str.substr(g + 1, h - g - 1)[0])) {
							std::cout << "CLAUSE ARGUMENT IS CONSTANT\n";
							pf_str = Unification(pf_str, pf_str.substr(g + 1, h - g - 1), query2[i].substr(e + 1, f - e - 1));
							//h = g + (query2[i].substr(e + 1, f - e - 1)).length() + 1;
							match++;
						}
						else if (islower(query2[i].substr(e + 1, f - e - 1)[0]) && islower(pf_str.substr(g + 1, h - g - 1)[0])) {
							std::cout << "BOTH ARE VARIABLE\n";
							pf_str = Unification(pf_str, pf_str.substr(g + 1, h - g - 1), query2[i].substr(e + 1, f - e - 1));
							//h = g + (query2[i].substr(e + 1, f - e - 1)).length() + 1;
							match++;
						}
						std::cout << "QUERIES AFTER PASSING THROUGH CASE-I\n";
						for (auto i = query2.begin(); i != query2.end(); ++i) std::cout << *i << "\n";
						// Case - II For middle arguments Eg: Play(_, x, y, w, _ ) Leaving first and last arguments
						for (int mid_arg = 0; mid_arg < cl_arg - 2; mid_arg++) {
							e = query2[i].find(",", e + 1);
							f = query2[i].find(",", e + 1);
							g = pf_str.find(",", g + 1);
							h = pf_str.find(",", g + 1);

							if (query2[i].substr(e + 1, f - e - 1) == pf_str.substr(g + 1, h - g - 1) && (isupper(query2[i].substr(e + 1, f - e - 1)[0]) && isupper(pf_str.substr(g + 1, h - g - 1)[0]))) {
								//std::cout << "ARGUMENT EQUALS WITH THE ARGUMENT\n";
								match++;
							}
							else if (islower(query2[i].substr(e + 1, f - e - 1)[0]) && isupper(pf_str.substr(g + 1, h - g - 1)[0])) {
								//std::cout << "HELLO\n";
								//std::cout << "CLAUSE ARGUMENT IS A VARIABLE\n";
								query2 = Unify(query2, query2[i].substr(e + 1, f - e - 1), pf_str.substr(g + 1, h - g - 1));
								//std::cout << "AFTER UNIFICATION\n";
								//f = e + (pf_str.substr(g + 1, h - g - 1)).length() + 1;
								//for (auto i = query2.begin(); i < query2.end(); ++i) std::cout << *i << "\n";
								match++;
							}
							else if (isupper(query2[i].substr(e + 1, f - e - 1)[0]) && islower(pf_str.substr(g + 1, h - g - 1)[0])) {
								//std::cout << "CLAUSE ARGUMENT IS CONSTANT\n";
								pf_str = Unification(pf_str, pf_str.substr(g + 1, h - g - 1), query2[i].substr(e + 1, f - e - 1));
								//h = g + (query2[i].substr(e + 1, f - e - 1)).length() + 1;
								match++;
							}
							else if (islower(query2[i].substr(e + 1, f - e - 1)[0]) && islower(pf_str.substr(g + 1, h - g - 1)[0])) {
								//std::cout << "BOTH ARE VARIABLE\n";
								pf_str = Unification(pf_str, pf_str.substr(g + 1, h - g - 1), query2[i].substr(e + 1, f - e - 1));
								//h = g + (query2[i].substr(e + 1, f - e - 1)).length() + 1;
								match++;
							}
						}
						std::cout << "QUERIES AFTER PASSING THROUGH CASE-II\n";
						for (auto i = query2.begin(); i != query2.end(); ++i) std::cout << *i << "\n";

						// Case - III For last argument Eg: Play(_, _, y)
						e = query2[i].find(",", e + 1);
						f = query2[i].find(")");
						g = pf_str.find(",", g + 1);
						h = pf_str.find(")");
						//std::cout << "CLAUSE: " << query2[i].substr(e + 1, f - e - 1) << "\n";
						//std::cout << "POSITIVE_FACTS: " << positive_facts[token][j].substr(g + 1, h - g - 1) << "\n";
						if (query2[i].substr(e + 1, f - e - 1) == pf_str.substr(g + 1, h - g - 1) && (isupper(query2[i].substr(e + 1, f - e - 1)[0]) && isupper(pf_str.substr(g + 1, h - g - 1)[0]))) {
							//std::cout << "ARGUMENT EQUALS WITH THE ARGUMENT\n";
							match++;
						}
						else if (islower(query2[i].substr(e + 1, f - e - 1)[0]) && isupper(pf_str.substr(g + 1, h - g - 1)[0])) {
							//std::cout << "HELLO\n";
							//std::cout << "CLAUSE ARGUMENT IS A VARIABLE\n";
							query2 = Unify(query2, query2[i].substr(e + 1, f - e - 1), pf_str.substr(g + 1, h - g - 1));
							//std::cout << "AFTER UNIFICATION\n";
							//f = e + (pf_str.substr(g + 1, h - g - 1)).length() + 1;
							//for (auto i = query2.begin(); i < query2.end(); ++i) std::cout << *i << "\n";
							match++;
						}
						else if (isupper(query2[i].substr(e + 1, f - e - 1)[0]) && islower(pf_str.substr(g + 1, h - g - 1)[0])) {
							//std::cout << "CLAUSE ARGUMENT IS CONSTANT\n";
							pf_str = Unification(pf_str, pf_str.substr(g + 1, h - g - 1), query2[i].substr(e + 1, f - e - 1));
							//h = g + (query2[i].substr(e + 1, f - e - 1)).length() + 1;
							match++;
						}
						else if (islower(query2[i].substr(e + 1, f - e - 1)[0]) && islower(pf_str.substr(g + 1, h - g - 1)[0])) {
							//std::cout << "BOTH ARE VARIABLE\n";
							pf_str = Unification(pf_str, pf_str.substr(g + 1, h - g - 1), query2[i].substr(e + 1, f - e - 1));
							//h = g + (query2[i].substr(e + 1, f - e - 1)).length() + 1;
							match++;
						}
						std::cout << "QUERIES AFTER PASSING THROUGH CASE-III\n";
						for (auto i = query2.begin(); i != query2.end(); ++i) std::cout << *i << "\n";
						int p = query2[i].find("(");
						int q = query2[i].find(")");
						int r = pf_str.find("(");
						int s = pf_str.find(")");

						if (cl_arg == match) {
							std::cout << "CLAUSE ARGUMENT EQUALS TO NUMBER OF MATCHES\n";
							int flag = 0;
							std::vector<std::string> query3;
							if (query[i].substr(p + 1, q - p - 1) == pf_str.substr(r + 1, s - r - 1)) {
								int k = 1;
								while (k < query2.size()) {
									query3.push_back(query2[k]);
									k++;
								}
								std::cout << "QUERY3 SIZE:" << query3.size() << "\n";
								if (query3.size()) std::cout << "CONTENTS OF QUERY3\n";
								for (auto i = query3.begin(); i < query3.end(); ++i) std::cout << *i << "\n";
								std::cout << "******************\n";
								result = FOL_engine(query3, 0, --depth);
								if (result) return true;
							}
							else {
								while (query2[i].substr(p + 1, q - p - 1) != pf_str.substr(r + 1, s - r - 1)) {
									if (flag == 0) {
										e = query2[i].find("(");
										f = query2[i].find(",");
										g = pf_str.find("(");
										h = pf_str.find(",");
										if (islower(query2[i].substr(e + 1, f - e - 1)[0]) && isupper(pf_str.substr(g + 1, h - g - 1)[0])) {
											//std::cout << "HELLO\n";
											std::cout << "CLAUSE ARGUMENT IS A VARIABLE\n";
											query2 = Unify(query2, query2[i].substr(e + 1, f - e - 1), pf_str.substr(g + 1, h - g - 1));
											std::cout << "AFTER UNIFICATION\n";
											//f = e + (pf_str.substr(g + 1, h - g - 1)).length() + 1;
											for (auto i = query2.begin(); i < query2.end(); ++i) std::cout << *i << "\n";
										}
										else if (isupper(query2[i].substr(e + 1, f - e - 1)[0]) && islower(pf_str.substr(g + 1, h - g - 1)[0])) {
											std::cout << "CLAUSE ARGUMENT IS CONSTANT\n";
											pf_str = Unification(pf_str, pf_str.substr(g + 1, h - g - 1), query2[i].substr(e + 1, f - e - 1));
											//h = g + (query2[i].substr(e + 1, f - e - 1)).length() + 1;
										}
										else if (islower(query2[i].substr(e + 1, f - e - 1)[0]) && islower(pf_str.substr(g + 1, h - g - 1)[0])) {
											std::cout << "BOTH ARE VARIABLE\n";
											query2 = Unify(query2, query2[i].substr(e + 1, f - e - 1), pf_str.substr(g + 1, h - g - 1));
											std::cout << "AFTER UNIFICATION\n";
											//f = e + (pf_str.substr(g + 1, h - g - 1)).length() + 1;
											for (auto i = query2.begin(); i < query2.end(); ++i) std::cout << *i << "\n";
										}
										std::cout << "QUERIES AFTER PASSING THROUGH CASE-I\n";
										for (auto i = query2.begin(); i != query2.end(); ++i) std::cout << *i << "\n";
										// Case - II For middle arguments Eg: Play(_, x, y, w, _ ) Leaving first and last arguments
										for (int mid_arg = 0; mid_arg < cl_arg - 2; mid_arg++) {
											e = query2[i].find(",", e + 1);
											f = query2[i].find(",", e + 1);
											g = pf_str.find(",", g + 1);
											h = pf_str.find(",", g + 1);

											if (islower(query2[i].substr(e + 1, f - e - 1)[0]) && isupper(pf_str.substr(g + 1, h - g - 1)[0])) {
												//std::cout << "HELLO\n";
												std::cout << "CLAUSE ARGUMENT IS A VARIABLE\n";
												query2 = Unify(query2, query2[i].substr(e + 1, f - e - 1), pf_str.substr(g + 1, h - g - 1));
												std::cout << "AFTER UNIFICATION\n";
												//f = e + (pf_str.substr(g + 1, h - g - 1)).length() + 1;
												for (auto i = query2.begin(); i < query2.end(); ++i) std::cout << *i << "\n";
											}
											else if (isupper(query2[i].substr(e + 1, f - e - 1)[0]) && islower(pf_str.substr(g + 1, h - g - 1)[0])) {
												std::cout << "CLAUSE ARGUMENT IS CONSTANT\n";
												pf_str = Unification(pf_str, pf_str.substr(g + 1, h - g - 1), query2[i].substr(e + 1, f - e - 1));
												//h = g + (query2[i].substr(e + 1, f - e - 1)).length() + 1;
											}
											else if (islower(query2[i].substr(e + 1, f - e - 1)[0]) && islower(pf_str.substr(g + 1, h - g - 1)[0])) {
												std::cout << "BOTH ARE VARIABLE\n";
												query2 = Unify(query2, query2[i].substr(e + 1, f - e - 1), pf_str.substr(g + 1, h - g - 1));
												std::cout << "AFTER UNIFICATION\n";
												//f = e + (pf_str.substr(g + 1, h - g - 1)).length() + 1;
												for (auto i = query2.begin(); i < query2.end(); ++i) std::cout << *i << "\n";
											}
										}
										std::cout << "QUERIES AFTER PASSING THROUGH CASE-II\n";
										for (auto i = query2.begin(); i != query2.end(); ++i) std::cout << *i << "\n";

										// Case - III For last argument Eg: Play(_, _, y)
										e = query2[i].find(",", e + 1);
										f = query2[i].find(")");
										g = pf_str.find(",", g + 1);
										h = pf_str.find(")");
										//std::cout << "CLAUSE: " << query2[i].substr(e + 1, f - e - 1) << "\n";
										//std::cout << "POSITIVE_FACTS: " << positive_facts[token][j].substr(g + 1, h - g - 1) << "\n";
										if (islower(query2[i].substr(e + 1, f - e - 1)[0]) && isupper(pf_str.substr(g + 1, h - g - 1)[0])) {
											//std::cout << "HELLO\n";
											std::cout << "CLAUSE ARGUMENT IS A VARIABLE\n";
											query2 = Unify(query2, query2[i].substr(e + 1, f - e - 1), pf_str.substr(g + 1, h - g - 1));
											std::cout << "AFTER UNIFICATION\n";
											//f = e + (pf_str.substr(g + 1, h - g - 1)).length() + 1;
											for (auto i = query2.begin(); i < query2.end(); ++i) std::cout << *i << "\n";
										}
										else if (isupper(query2[i].substr(e + 1, f - e - 1)[0]) && islower(pf_str.substr(g + 1, h - g - 1)[0])) {
											std::cout << "CLAUSE ARGUMENT IS CONSTANT\n";
											pf_str = Unification(pf_str, pf_str.substr(g + 1, h - g - 1), query2[i].substr(e + 1, f - e - 1));
											//h = g + (query2[i].substr(e + 1, f - e - 1)).length() + 1;
										}
										else if (islower(query2[i].substr(e + 1, f - e - 1)[0]) && islower(pf_str.substr(g + 1, h - g - 1)[0])) {
											std::cout << "BOTH ARE VARIABLE\n";
											query2 = Unify(query2, query2[i].substr(e + 1, f - e - 1), pf_str.substr(g + 1, h - g - 1));
											std::cout << "AFTER UNIFICATION\n";
											//f = e + (pf_str.substr(g + 1, h - g - 1)).length() + 1;
											for (auto i = query2.begin(); i < query2.end(); ++i) std::cout << *i << "\n";
										}
										std::cout << "QUERIES AFTER PASSING THROUGH CASE-III\n";
										for (auto i = query2.begin(); i != query2.end(); ++i) std::cout << *i << "\n";
										flag = 1;
									}
									else if (flag == 1) {
										e = query2[i].find("(");
										f = query2[i].find(",");
										g = pf_str.find("(");
										h = pf_str.find(",");
										if (islower(query2[i].substr(e + 1, f - e - 1)[0]) && isupper(pf_str.substr(g + 1, h - g - 1)[0])) {
											//std::cout << "HELLO\n";
											std::cout << "CLAUSE ARGUMENT IS A VARIABLE\n";
											query2 = Unify(query2, query2[i].substr(e + 1, f - e - 1), pf_str.substr(g + 1, h - g - 1));
											std::cout << "AFTER UNIFICATION\n";
											//f = e + (pf_str.substr(g + 1, h - g - 1)).length() + 1;
											for (auto i = query2.begin(); i < query2.end(); ++i) std::cout << *i << "\n";
										}
										else if (isupper(query2[i].substr(e + 1, f - e - 1)[0]) && islower(pf_str.substr(g + 1, h - g - 1)[0])) {
											std::cout << "CLAUSE ARGUMENT IS CONSTANT\n";
											pf_str = Unification(pf_str, pf_str.substr(g + 1, h - g - 1), query2[i].substr(e + 1, f - e - 1));
											//h = g + (query2[i].substr(e + 1, f - e - 1)).length() + 1;
										}
										else if (islower(query2[i].substr(e + 1, f - e - 1)[0]) && islower(pf_str.substr(g + 1, h - g - 1)[0])) {
											std::cout << "BOTH ARE VARIABLE\n";
											pf_str = Unification(pf_str, pf_str.substr(g + 1, h - g - 1), query2[i].substr(e + 1, f - e - 1));
											//h = g + (query2[i].substr(e + 1, f - e - 1)).length() + 1;
										}
										for (int mid_arg = 0; mid_arg < cl_arg - 2; mid_arg++) {
											e = query2[i].find(",", e + 1);
											f = query2[i].find(",", e + 1);
											g = pf_str.find(",", g + 1);
											h = pf_str.find(",", h + 1);

											if (islower(query2[i].substr(e + 1, f - e - 1)[0]) && isupper(pf_str.substr(g + 1, h - g - 1)[0])) {
												//std::cout << "HELLO\n";
												std::cout << "CLAUSE ARGUMENT IS A VARIABLE\n";
												query2 = Unify(query2, query2[i].substr(e + 1, f - e - 1), pf_str.substr(g + 1, h - g - 1));
												std::cout << "AFTER UNIFICATION\n";
												//f = e + (pf_str.substr(g + 1, h - g - 1)).length() + 1;
												for (auto i = query2.begin(); i < query2.end(); ++i) std::cout << *i << "\n";
											}
											else if (isupper(query2[i].substr(e + 1, f - e - 1)[0]) && islower(pf_str.substr(g + 1, h - g - 1)[0])) {
												std::cout << "CLAUSE ARGUMENT IS CONSTANT\n";
												pf_str = Unification(pf_str, pf_str.substr(g + 1, h - g - 1), query2[i].substr(e + 1, f - e - 1));
												//h = g + (query2[i].substr(e + 1, f - e - 1)).length() + 1;
											}
											else if (islower(query2[i].substr(e + 1, f - e - 1)[0]) && islower(pf_str.substr(g + 1, h - g - 1)[0])) {
												std::cout << "BOTH ARE VARIABLE\n";
												pf_str = Unification(pf_str, pf_str.substr(g + 1, h - g - 1), query2[i].substr(e + 1, f - e - 1));
												//h = g + (query2[i].substr(e + 1, f - e - 1)).length() + 1;
											}
										}
										std::cout << "QUERIES AFTER PASSING THROUGH CASE-II\n";
										for (auto i = query2.begin(); i != query2.end(); ++i) std::cout << *i << "\n";

										// Case - III For last argument Eg: Play(_, _, y)
										e = query2[i].find(",", e + 1);
										f = query2[i].find(")");
										g = pf_str.find(",", g + 1);
										h = pf_str.find(")");
										//std::cout << "CLAUSE: " << query2[i].substr(e + 1, f - e - 1) << "\n";
										//std::cout << "POSITIVE_FACTS: " << positive_facts[token][j].substr(g + 1, h - g - 1) << "\n";
										if (islower(query2[i].substr(e + 1, f - e - 1)[0]) && isupper(pf_str.substr(g + 1, h - g - 1)[0])) {
											//std::cout << "HELLO\n";
											std::cout << "CLAUSE ARGUMENT IS A VARIABLE\n";
											query2 = Unify(query2, query2[i].substr(e + 1, f - e - 1), pf_str.substr(g + 1, h - g - 1));
											std::cout << "AFTER UNIFICATION\n";
											//f = e + (pf_str.substr(g + 1, h - g - 1)).length() + 1;
											for (auto i = query2.begin(); i < query2.end(); ++i) std::cout << *i << "\n";
										}
										else if (isupper(query2[i].substr(e + 1, f - e - 1)[0]) && islower(pf_str.substr(g + 1, h - g - 1)[0])) {
											std::cout << "CLAUSE ARGUMENT IS CONSTANT\n";
											pf_str = Unification(pf_str, pf_str.substr(g + 1, h - g - 1), query2[i].substr(e + 1, f - e - 1));
											//h = g + (query2[i].substr(e + 1, f - e - 1)).length() + 1;
										}
										else if (islower(query2[i].substr(e + 1, f - e - 1)[0]) && islower(pf_str.substr(g + 1, h - g - 1)[0])) {
											std::cout << "BOTH ARE VARIABLE\n";
											pf_str = Unification(pf_str, pf_str.substr(g + 1, h - g - 1), query2[i].substr(e + 1, f - e - 1));
											//h = g + (query2[i].substr(e + 1, f - e - 1)).length() + 1;
										}
										std::cout << "QUERIES AFTER PASSING THROUGH CASE-III\n";
										for (auto i = query2.begin(); i != query2.end(); ++i) std::cout << *i << "\n";
										flag = 0;
									}
									p = query2[i].find("(");
									q = query2[i].find(")");
									r = pf_str.find("(");
									s = pf_str.find(")");
								}
								int k = 1;
								while (k < query2.size()) {
									query3.push_back(query2[k]);
									k++;
								}
								std::cout << "QUERY3 SIZE:" << query3.size() << "\n";
								if (query3.size()) std::cout << "CONTENTS OF QUERY3\n";
								for (auto i = query3.begin(); i < query3.end(); ++i) std::cout << *i << "\n";
								std::cout << "******************\n";
								result = FOL_engine(query3, 0, --depth);
								if (result) return true;
							}
						}
					}
				}
			}

			// Now Check in Positive Equations 2D Vector String
			if (positive_equations.count(token) > 0) {
				std::cout << "CHECKING IN POSITIVE EQUATIONS\n";
				std::string eq_token;
				std::vector<std::string> all_tok_eq;  // Storing all the equations for respective tokens
				std::vector<std::string> query2;
				for (int j = 0; j < positive_equations[token].size(); j++) {
					for (int k = 0; k < positive_equations[token][j].size(); k++) {
						eq_token = positive_equations[token][j][k].substr(0, positive_equations[token][j][k].find("("));
						if (token == eq_token) {
							all_tok_eq = positive_equations[token][j];

							// For single arguments
							if (cl_arg == 1) {
								std::cout << "FOR SINGLE ARGUMENT:\n";
								e = query[i].find("(");
								f = query[i].find(")");
								g = all_tok_eq[k].find("(");
								h = all_tok_eq[k].find(")");
								if ((query[i].substr(e + 1, f - e - 1) == all_tok_eq[k].substr(g + 1, h - g - 1)) && isupper(query[i].substr(e + 1, f - e - 1)[0]) && isupper(all_tok_eq[k].substr(g + 1, h - g - 1)[0])) {
									std::cout << "BOTH ARGUMENTS ARE EQUAL\n";
									int d = i + 1;
									std::vector<std::string> equation = all_tok_eq;
									query2 = Resolution(query, equation);
									std::cout << "AFTER MULTIPLE RESOLUTION\n";
									//while (d < query.size()) {
										//query2.push_back(query[d]);
										//d++;
									//}
									//for (int left = 0; left < all_tok_eq.size(); left++) {
										//if (left != k) query2.push_back(all_tok_eq[left]);
									//}
									std::cout << "QUERY2 SIZE:" << query2.size() << "\n";
									if (query2.size()) std::cout << "CONTENTS OF QUERY2\n";
									for (auto i = query2.begin(); i < query2.end(); ++i) std::cout << *i << "\n";
									result = FOL_engine(query2, 0, --depth);
									std::cout << "******************\n";
									if (result) return true;
								}
								else if (islower(query[i].substr(e + 1, f - e - 1)[0]) && isupper(all_tok_eq[k].substr(g + 1, h - g - 1)[0])) {
									std::cout << "CLAUSE ARGUMENT IS VARIABLE AND EQUATION HAS CONSTANT\n";
									std::vector<std::string> query3 = Unify(query, query[i].substr(e + 1, f - e - 1), all_tok_eq[k].substr(g + 1, h - g - 1));
									std::cout << "AFTER UNIFICATION\n";
									for (auto i = query3.begin(); i < query3.end(); ++i) std::cout << *i << "\n";
									std::vector<std::string> equation = all_tok_eq;
									query2 = Resolution(query3, equation);
									std::cout << "AFTER MULTIPLE RESOLUTION\n";
									//int d = i + 1;
									//while (d < query3.size()) {
										//query2.push_back(query3[d]);
										//d++;
									//}
									//for (int left = 0; left < all_tok_eq.size(); left++) {
										//if (left != k) query2.push_back(all_tok_eq[left]);
									//}
									std::cout << "QUERY2 SIZE:" << query2.size() << "\n";
									if (query2.size()) std::cout << "CONTENTS OF QUERY2\n";
									for (auto i = query2.begin(); i < query2.end(); ++i) std::cout << *i << "\n";
									result = FOL_engine(query2, 0, --depth);
									std::cout << "******************\n";
									if (result) return true;

								}
								else if (islower(all_tok_eq[k].substr(g + 1, h - g - 1)[0])) {
									std::cout << "BOTH CLAUSE AND EQUATIONS ARE VARIABLE\n";
									//std::cout << "HEEEEEEEEEEEEEELLLLLLLLLLLLLLLLLOOOOOOOOOO\n";
									//std::cout << all_tok_eq[k].substr(g + 1, h - g - 1) << "\n";
									//std::cout << query[i].substr(e + 1, f - e - 1) << "\n";
									//std::cout << "HEEEEEEEEEEEEEELLLLLLLLLLLLLLLLLOOOOOOOOOO\n";
									all_tok_eq = Unify(all_tok_eq, all_tok_eq[k].substr(g + 1, h - g - 1), query[0].substr(e + 1, f - e - 1));
									std::cout << "AFTER UNIFICATION OF EQUATION\n";
									for (auto i = all_tok_eq.begin(); i < all_tok_eq.end(); ++i) std::cout << *i << "\n";
									std::vector<std::string> equation = all_tok_eq;
									query2 = Resolution(query, equation);
									std::cout << "AFTER MULTIPLE RESOLUTION\n";
									//int d = i + 1;
									//while (d < query.size()) {
										//query2.push_back(query[d]);
										//d++;
									//}
									//for (int left = 0; left < all_tok_eq.size(); left++) {
										//if (left != k) query2.push_back(all_tok_eq[left]);
									//}
									std::cout << "QUERY2 SIZE:" << query2.size() << "\n";
									if (query2.size()) std::cout << "CONTENTS OF QUERY2\n";
									for (auto i = query2.begin(); i < query2.end(); ++i) std::cout << *i << "\n";
									result = FOL_engine(query2, 0, --depth);
									std::cout << "******************\n";
									if (result) return true;
								}
							}
							else if (cl_arg > 1) {
								std::cout << "ARGUMENTS ARE MORE THAN ONE\n";
								int match = 0;
								std::vector<std::string> query3 = query;
								//std::cout << token << "\n";
								//for (auto i = query3.begin(); i < query3.end(); ++i) std::cout << *i << "\n";
								// Case - I For the first argument
								e = query3[i].find("(");
								f = query3[i].find(",");
								g = all_tok_eq[k].find("(");
								h = all_tok_eq[k].find(",");
								if ((query3[i].substr(e + 1, f - e - 1) == all_tok_eq[k].substr(g + 1, h - g - 1)) && isupper(query3[i].substr(e + 1, f - e - 1)[0]) && isupper(all_tok_eq[k].substr(g + 1, h - g - 1)[0])) {
									std::cout << "BOTH ARGUMENTS ARE EQUAL\n";
									match++;
								}
								else if (islower(query3[i].substr(e + 1, f - e - 1)[0]) && isupper(all_tok_eq[k].substr(g + 1, h - g - 1)[0])) {
									std::cout << "CLAUSE ARGUMENT IS VARIABLE AND EQUATION HAS CONSTANT\n";
									query3 = Unify(query3, query3[i].substr(e + 1, f - e - 1), all_tok_eq[k].substr(g + 1, h - g - 1));
									std::cout << "AFTER UNIFICATION\n";
									for (auto i = query3.begin(); i < query3.end(); ++i) std::cout << *i << "\n";
									match++;
								}
								else if (islower(all_tok_eq[k].substr(g + 1, h - g - 1)[0])) {
									std::cout << "BOTH CLAUSE AND EQUATIONS ARE VARIABLE\n";
									all_tok_eq = Unify(all_tok_eq, all_tok_eq[k].substr(g + 1, h - g - 1), query3[i].substr(e + 1, f - e - 1));
									std::cout << "AFTER UNIFICATION OF EQUATION\n";
									for (auto i = all_tok_eq.begin(); i < all_tok_eq.end(); ++i) std::cout << *i << "\n";
									match++;
								}
								std::cout << "AFTER CASE-I\n";
								for (auto i = query3.begin(); i < query3.end(); ++i) std::cout << *i << "\n";
								// Case - II For mid arguments (Except First and last arguments)
								std::cout << "IN CASE-II\n";
								for (int mid_arg = 0; mid_arg < cl_arg - 2; mid_arg++) {
									e = query3[i].find(",", e + 1);
									f = query3[i].find(",", e + 1);
									g = all_tok_eq[k].find(",", g + 1);
									h = all_tok_eq[k].find(",", g + 1);
									if ((query3[i].substr(e + 1, f - e - 1) == all_tok_eq[k].substr(g + 1, h - g - 1)) && isupper(query3[i].substr(e + 1, f - e - 1)[0]) && isupper(all_tok_eq[k].substr(g + 1, h - g - 1)[0])) {
										std::cout << "BOTH ARGUMENTS ARE EQUAL\n";
										match++;
									}
									else if (islower(query3[i].substr(e + 1, f - e - 1)[0]) && isupper(all_tok_eq[k].substr(g + 1, h - g - 1)[0])) {
										std::cout << "CLAUSE ARGUMENT IS VARIABLE AND EQUATION HAS CONSTANT\n";
										query3 = Unify(query3, query3[i].substr(e + 1, f - e - 1), all_tok_eq[k].substr(g + 1, h - g - 1));
										std::cout << "AFTER UNIFICATION\n";
										for (auto i = query3.begin(); i < query3.end(); ++i) std::cout << *i << "\n";
										match++;
									}
									else if (islower(all_tok_eq[k].substr(g + 1, h - g - 1)[0])) {
										std::cout << "BOTH CLAUSE AND EQUATIONS ARE VARIABLE\n";
										all_tok_eq = Unify(all_tok_eq, all_tok_eq[k].substr(g + 1, h - g - 1), query3[i].substr(e + 1, f - e - 1));
										std::cout << "AFTER UNIFICATION OF EQUATION\n";
										for (auto i = all_tok_eq.begin(); i < all_tok_eq.end(); ++i) std::cout << *i << "\n";
										match++;
									}
								}
								std::cout << "AFTER CASE-II\n";
								for (auto i = query3.begin(); i < query3.end(); ++i) std::cout << *i << "\n";
								// Case - III For Last arguments
								e = query3[i].find(",", e + 1);
								f = query3[i].find(")");
								g = all_tok_eq[k].find(",", g + 1);
								h = all_tok_eq[k].find(")");

								if ((query3[i].substr(e + 1, f - e - 1) == all_tok_eq[k].substr(g + 1, h - g - 1)) && isupper(query3[i].substr(e + 1, f - e - 1)[0]) && isupper(all_tok_eq[k].substr(g + 1, h - g - 1)[0])) {
									std::cout << "BOTH ARGUMENTS ARE EQUAL\n";
									match++;
								}
								else if (islower(query3[i].substr(e + 1, f - e - 1)[0]) && isupper(all_tok_eq[k].substr(g + 1, h - g - 1)[0])) {
									std::cout << "CLAUSE ARGUMENT IS VARIABLE AND EQUATION HAS CONSTANT\n";
									query3 = Unify(query3, query3[i].substr(e + 1, f - e - 1), all_tok_eq[k].substr(g + 1, h - g - 1));
									std::cout << "AFTER UNIFICATION\n";
									for (auto i = query3.begin(); i < query3.end(); ++i) std::cout << *i << "\n";
									match++;
								}
								else if (islower(all_tok_eq[k].substr(g + 1, h - g - 1)[0])) {
									std::cout << "BOTH CLAUSE AND EQUATIONS ARE VARIABLE\n";
									all_tok_eq = Unify(all_tok_eq, all_tok_eq[k].substr(g + 1, h - g - 1), query3[i].substr(e + 1, f - e - 1));
									std::cout << "AFTER UNIFICATION OF EQUATION\n";
									for (auto i = all_tok_eq.begin(); i < all_tok_eq.end(); ++i) std::cout << *i << "\n";
									match++;
								}
								std::cout << "HELLLLLLLOOOOOOOOO\n";
								std::cout << all_tok_eq[k] << "\n";
								std::cout << "HELLLLLLLOOOOOOOOO\n";
								int p = query3[i].find("(");
								int q = query3[i].find(")");
								int r = all_tok_eq[k].find("(");
								int s = all_tok_eq[k].find(")");
								if (cl_arg == match) {
									int flag = 0;
									std::cout << "CLAUSE ARGUMENT EQUALS THE MATCH\n";
									if (query3[i].substr(p + 1, q - p - 1) == all_tok_eq[k].substr(r + 1, s - r - 1)) {
										std::cout << "NO WORRIES\n";
										std::vector<std::string> equation = all_tok_eq;
										query3 = Resolution(query3, equation);
										std::cout << "AFTER RESOLUTION\n";
										query2 = query3;
										std::cout << "QUERY2 SIZE:" << query2.size() << "\n";
										if (query2.size()) std::cout << "CONTENTS OF QUERY2\n";
										for (auto i = query2.begin(); i < query2.end(); ++i) std::cout << *i << "\n";
										result = FOL_engine(query2, 0, --depth);
										std::cout << "******************\n";
										if (result) return true;
									}
									else {
										while (query3[i].substr(p + 1, q - p - 1) != all_tok_eq[k].substr(r + 1, s - r - 1)) {
											if (flag == 0) {
												e = query3[i].find("(");
												f = query3[i].find(",");
												g = all_tok_eq[k].find("(");
												h = all_tok_eq[k].find(",");
												if (islower(query3[i].substr(e + 1, f - e - 1)[0]) && islower(all_tok_eq[k].substr(g + 1, h - g - 1)[0])) {
													query3 = Unify(query3, query3[i].substr(e + 1, f - e - 1), all_tok_eq[k].substr(g + 1, h - g - 1));
												}
												else if (islower(query3[i].substr(e + 1, f - e - 1)[0]) && isupper(all_tok_eq[k].substr(g + 1, h - g - 1)[0])) {
													query3 = Unify(query3, query3[i].substr(e + 1, f - e - 1), all_tok_eq[k].substr(g + 1, h - g - 1));
												}
												else if (isupper(query3[i].substr(e + 1, f - e - 1)[0]) && islower(all_tok_eq[k].substr(g + 1, h - g - 1)[0])) {
													std::cout << query3[i].substr(e + 1, f - e - 1) << "\n";
													all_tok_eq = Unify(all_tok_eq, all_tok_eq[k].substr(g + 1, h - g - 1), query3[i].substr(e + 1, f - e - 1));
													std::cout << all_tok_eq[k] << "\n";
												}
												for (int mid_arg = 0; mid_arg < cl_arg - 2; mid_arg++) {
													e = query3[i].find(",", e + 1);
													f = query3[i].find(",", e + 1);
													g = all_tok_eq[k].find(",", g + 1);
													h = all_tok_eq[k].find(",", g + 1);
													if (islower(query3[i].substr(e + 1, f - e - 1)[0]) && islower(all_tok_eq[k].substr(g + 1, h - g - 1)[0])) {
														query3 = Unify(query3, query3[i].substr(e + 1, f - e - 1), all_tok_eq[k].substr(g + 1, h - g - 1));
													}
													else if (islower(query3[i].substr(e + 1, f - e - 1)[0]) && isupper(all_tok_eq[k].substr(g + 1, h - g - 1)[0])) {
														query3 = Unify(query3, query3[i].substr(e + 1, f - e - 1), all_tok_eq[k].substr(g + 1, h - g - 1));
													}
													else if (isupper(query3[i].substr(e + 1, f - e - 1)[0]) && islower(all_tok_eq[k].substr(g + 1, h - g - 1)[0])) {
														all_tok_eq = Unify(all_tok_eq, all_tok_eq[k].substr(g + 1, h - g - 1), query3[i].substr(e + 1, f - e - 1));
														std::cout << all_tok_eq[k] << "\n";
													}
												}
												e = query3[i].find(",", e + 1);
												f = query3[i].find(")");
												g = all_tok_eq[k].find(",", g + 1);
												h = all_tok_eq[k].find(")");
												if (islower(query3[i].substr(e + 1, f - e - 1)[0]) && islower(all_tok_eq[k].substr(g + 1, h - g - 1)[0])) {
													query3 = Unify(query3, query3[i].substr(e + 1, f - e - 1), all_tok_eq[k].substr(g + 1, h - g - 1));
												}
												else if (islower(query3[i].substr(e + 1, f - e - 1)[0]) && isupper(all_tok_eq[k].substr(g + 1, h - g - 1)[0])) {
													query3 = Unify(query3, query3[i].substr(e + 1, f - e - 1), all_tok_eq[k].substr(g + 1, h - g - 1));
												}
												else if (isupper(query3[i].substr(e + 1, f - e - 1)[0]) && islower(all_tok_eq[k].substr(g + 1, h - g - 1)[0])) {
													all_tok_eq = Unify(all_tok_eq, all_tok_eq[k].substr(g + 1, h - g - 1), query3[i].substr(e + 1, f - e - 1));
													std::cout << all_tok_eq[k] << "\n";
												}
												for (auto i = query3.begin(); i < query3.end(); ++i) std::cout << *i << "\n";
												std::cout << "************\n";
												std::cout << all_tok_eq[k] << "\n";
												flag = 1;
											}
											else if (flag == 1) {
												e = query3[i].find("(");
												f = query3[i].find(",");
												g = all_tok_eq[k].find("(");
												h = all_tok_eq[k].find(",");
												if (islower(query3[i].substr(e + 1, f - e - 1)[0]) && islower(all_tok_eq[k].substr(g + 1, h - g - 1)[0])) {
													all_tok_eq = Unify(all_tok_eq, all_tok_eq[k].substr(g + 1, h - g - 1), query3[i].substr(e + 1, f - e - 1));
													std::cout << all_tok_eq[k] << "\n";
												}
												else if (islower(query3[i].substr(e + 1, f - e - 1)[0]) && isupper(all_tok_eq[k].substr(g + 1, h - g - 1)[0])) {
													query3 = Unify(query3, query3[i].substr(e + 1, f - e - 1), all_tok_eq[k].substr(g + 1, h - g - 1));
												}
												else if (isupper(query3[i].substr(e + 1, f - e - 1)[0]) && islower(all_tok_eq[k].substr(g + 1, h - g - 1)[0])) {
													all_tok_eq = Unify(all_tok_eq, all_tok_eq[k].substr(g + 1, h - g - 1), query3[i].substr(e + 1, f - e - 1));
													std::cout << all_tok_eq[k] << "\n";
												}
												for (int mid_arg = 0; mid_arg < cl_arg - 2; mid_arg++) {
													e = query3[i].find(",", e + 1);
													f = query3[i].find(",", e + 1);
													g = all_tok_eq[k].find(",", g + 1);
													h = all_tok_eq[k].find(",", g + 1);
													if (islower(query3[i].substr(e + 1, f - e - 1)[0]) && islower(all_tok_eq[k].substr(g + 1, h - g - 1)[0])) {
														all_tok_eq = Unify(all_tok_eq, all_tok_eq[k].substr(g + 1, h - g - 1), query3[i].substr(e + 1, f - e - 1));
														std::cout << all_tok_eq[k] << "\n";
													}
													else if (islower(query3[i].substr(e + 1, f - e - 1)[0]) && isupper(all_tok_eq[k].substr(g + 1, h - g - 1)[0])) {
														query3 = Unify(query3, query3[i].substr(e + 1, f - e - 1), all_tok_eq[k].substr(g + 1, h - g - 1));
													}
													else if (isupper(query3[i].substr(e + 1, f - e - 1)[0]) && islower(all_tok_eq[k].substr(g + 1, h - g - 1)[0])) {
														all_tok_eq = Unify(all_tok_eq, all_tok_eq[k].substr(g + 1, h - g - 1), query3[i].substr(e + 1, f - e - 1));
														std::cout << all_tok_eq[k] << "\n";
													}
												}
												e = query3[i].find(",", e + 1);
												f = query3[i].find(")");
												g = all_tok_eq[k].find(",", g + 1);
												h = all_tok_eq[k].find(")");
												if (islower(query3[i].substr(e + 1, f - e - 1)[0]) && islower(all_tok_eq[k].substr(g + 1, h - g - 1)[0])) {
													all_tok_eq = Unify(all_tok_eq, all_tok_eq[k].substr(g + 1, h - g - 1), query3[i].substr(e + 1, f - e - 1));
													std::cout << all_tok_eq[k] << "\n";
												}
												else if (islower(query3[i].substr(e + 1, f - e - 1)[0]) && isupper(all_tok_eq[k].substr(g + 1, h - g - 1)[0])) {
													query3 = Unify(query3, query3[i].substr(e + 1, f - e - 1), all_tok_eq[k].substr(g + 1, h - g - 1));
													std::cout << all_tok_eq[k] << "\n";
												}
												else if (isupper(query3[i].substr(e + 1, f - e - 1)[0]) && islower(all_tok_eq[k].substr(g + 1, h - g - 1)[0])) {
													all_tok_eq = Unify(all_tok_eq, all_tok_eq[k].substr(g + 1, h - g - 1), query3[i].substr(e + 1, f - e - 1));
													std::cout << all_tok_eq[k] << "\n";
												}
												for (auto i = query3.begin(); i < query3.end(); ++i) std::cout << *i << "\n";
												std::cout << "************\n";
												std::cout << all_tok_eq[k] << "\n";
												flag = 0;
											}
											p = query3[i].find("(");
											q = query3[i].find(")");
											r = all_tok_eq[k].find("(");
											s = all_tok_eq[k].find(")");
										}
										std::vector<std::string> equation = all_tok_eq;
										query3 = Resolution(query3, equation);
										std::cout << "AFTER RESOLUTION\n";
										query2 = query3;
										std::cout << "QUERY2 SIZE:" << query2.size() << "\n";
										if (query2.size()) std::cout << "CONTENTS OF QUERY2\n";
										for (auto i = query2.begin(); i < query2.end(); ++i) std::cout << *i << "\n";
										result = FOL_engine(query2, 0, --depth);
										std::cout << "******************\n";
										if (result) return true;
									}								
									//std::cout << k << "\n";
									//std::cout << "QUERY3_SIZE: " << query3.size() << "\n";
									//while (d < query3.size()) {
										//std::cout << "YIPPPY\n";
										//query2.push_back(query3[d]);
										//d++;
									//}
									//query2 = query3;
									//for (int left = 0; left < all_tok_eq.size(); left++) {
										//if (left != k) query2.push_back(all_tok_eq[left]);
									//}
								}
							}
						}
					}
				}
			}
		}
		else {
			// Check in Negative Facts
			std::cout << "CHECKING FOR NEGATIVE FACTS \n";
			int e, f, g, h;
			std::string token = clause.substr(0, clause.find("("));
			//std::cout << "############################################################################\n";
			//std::cout << token << "\n";
			int cl_arg = std::count(clause.begin(), clause.end(), ',') + 1;
			//std::cout << negative_facts[token].size() << "\n";
			if (negative_facts.count(token) > 0) {
				for (int j = 0; j < negative_facts[token].size(); j++) {
					if (cl_arg == 1) {
						std::cout << "ARGUMENT EQUALS ONE\n";
						//std::cout << "YES" << "\n";
						e = clause.find("(");
						f = clause.find(")");
						g = negative_facts[token][j].find("(");
						h = negative_facts[token][j].find(")");
						//std::cout << negative_facts[token][j] << "\n";
						//std::cout << "NEGATIVE_FACTS_SIZE: " << negative_facts[token].size() << "\n";
						if (clause.substr(e + 1, f - e - 1) == negative_facts[token][j].substr(g + 1, h - g - 1) && (isupper(clause.substr(e + 1, f - e - 1)[0]) && isupper(negative_facts[token][j].substr(g + 1, h - g - 1)[0]))) {
							std::cout << "ARGUMENT EQUALS WITH THE ARGUMENT\n";
							std::vector<std::string> query2;
							int k = i + 1;
							while (k < query.size()) {
								query2.push_back(query[k]);
								k++;
							}
							std::cout << "QUERY2 SIZE:" << query2.size() << "\n";
							if (query2.size()) std::cout << "CONTENTS OF QUERY2\n";
							for (auto i = query2.begin(); i != query2.end(); ++i) std::cout << *i << "\n";
							result = FOL_engine(query2, 0, ++depth);
							std::cout << "*********************\n";
							if (result) return true;
						}
						else if (islower(clause.substr(e + 1, f - e - 1)[0]) && isupper(negative_facts[token][j].substr(g + 1, h - g - 1)[0])) {
							std::cout << "ARGUMENT IS VARIABLE\n";
							std::vector<std::string> query2 = Unify(query, clause.substr(e + 1, f - e - 1), negative_facts[token][j].substr(g + 1, h - g - 1));
							std::cout << "AFTER UNIFICATION\n";
							for (auto i = query2.begin(); i != query2.end(); ++i) std::cout << *i << "\n";
							std::vector<std::string> query3;
							int k = i + 1;
							while (k < query2.size()) {
								query3.push_back(query2[k]);
								k++;
							}
							std::cout << "QUERY3 SIZE:" << query3.size() << "\n";
							if (query3.size()) std::cout << "CONTENTS OF QUERY3\n";
							for (auto i = query3.begin(); i != query3.end(); ++i) std::cout << *i << "\n";
							result = FOL_engine(query3, 0, --depth);
							std::cout << "*********************\n";
							if (result) return true;
						}
						else if (islower(negative_facts[token][j].substr(g + 1, h - g - 1)[0])) {
							std::cout << "BOTH ARE VARIABLES\n";
							std::vector<std::string> query2;
							int k = i + 1;
							while (k < query.size()) {
								query2.push_back(query[k]);
								k++;
							}
							std::cout << "QUERY2 SIZE:" << query2.size() << "\n";
							if (query2.size()) std::cout << "CONTENTS OF QUERY2\n";
							for (auto i = query2.begin(); i != query2.end(); ++i) std::cout << *i << "\n";
							result = FOL_engine(query2, 0, ++depth);
							std::cout << "*********************\n";
							if (result) return true;
						}
					}
					if (cl_arg > 1) {
						std::cout << "ARGUMENTS IS MORE THAN ONE\n";
						int match = 0;
						std::vector<std::string> query2 = query;
						std::string nf_str = negative_facts[token][j];
						// Case - I For first argument Eg: Play(x, _ , _ ) Play(Come, Abhi, OP)
						e = query2[i].find("(");
						f = query2[i].find(",");
						g = nf_str.find("(");
						h = nf_str.find(",");

						if (query2[i].substr(e + 1, f - e - 1) == nf_str.substr(g + 1, h - g - 1) && (isupper(query2[i].substr(e + 1, f - e - 1)[0]) && isupper(nf_str.substr(g + 1, h - g - 1)[0]))) {
							std::cout << "BOTH ARGUMENTS ARE EQUAL\n";
							match++;
						}
						else if (islower(query2[i].substr(e + 1, f - e - 1)[0]) && isupper(nf_str.substr(g + 1, h - g - 1)[0])) {
							std::cout << "CLAUSE ARGUMENT IS VARIBALE\n";
							query2 = Unify(query2, query2[i].substr(e + 1, f - e - 1), nf_str.substr(g + 1, h - g - 1));
							std::cout << "AFTER UNIFICATION\n";
							for (auto i = query2.begin(); i != query2.end(); ++i) std::cout << *i << "\n";
							match++;
						}
						else if (isupper(query2[i].substr(e + 1, f - e - 1)[0]) && islower(nf_str.substr(g + 1, h - g - 1)[0])) {
							nf_str = Unification(nf_str, nf_str.substr(g + 1, h - g - 1), query2[i].substr(e + 1, f - e - 1));
							match++;
						}
						else if (islower(query2[i].substr(e + 1, f - e - 1)[0]) && islower(nf_str.substr(g + 1, h - g - 1)[0])) {
							nf_str = Unification(nf_str, nf_str.substr(g + 1, h - g - 1), query2[i].substr(e + 1, f - e - 1));
							std::cout << "BOTH ARGUMENTS ARE VARIABLE\n";
							match++;
						}
						std::cout << "AFTER CASE-I\n";
						for (auto i = query2.begin(); i != query2.end(); ++i) std::cout << *i << "\n";
						// Case - II For middle arguments Eg: Play(_, x, y, w, _ ) Leaving first and last arguments
						std::cout << "IN CASE -II\n";
						for (int mid_arg = 0; mid_arg < cl_arg - 2; mid_arg++) {
							e = query2[i].find(",", e + 1);
							f = query2[i].find(",", e + 1);
							g = nf_str.find(",", g + 1);
							h = nf_str.find(",", g + 1);

							if (query2[i].substr(e + 1, f - e - 1) == nf_str.substr(g + 1, h - g - 1) && (isupper(query2[i].substr(e + 1, f - e - 1)[0]) && isupper(nf_str.substr(g + 1, h - g - 1)[0]))) {
								std::cout << "BOTH ARGUMENTS ARE EQUAL\n";
								match++;
							}
							else if (islower(query2[i].substr(e + 1, f - e - 1)[0]) && isupper(nf_str.substr(g + 1, h - g - 1)[0])) {
								std::cout << "CLAUSE ARGUMENT IS VARIBALE\n";
								query2 = Unify(query2, query2[i].substr(e + 1, f - e - 1), nf_str.substr(g + 1, h - g - 1));
								std::cout << "AFTER UNIFICATION\n";
								for (auto i = query2.begin(); i != query2.end(); ++i) std::cout << *i << "\n";
								match++;
							}
							else if (isupper(query2[i].substr(e + 1, f - e - 1)[0]) && islower(nf_str.substr(g + 1, h - g - 1)[0])) {
								nf_str = Unification(nf_str, nf_str.substr(g + 1, h - g - 1), query2[i].substr(e + 1, f - e - 1));
								match++;
							}
							else if (islower(query2[i].substr(e + 1, f - e - 1)[0]) && islower(nf_str.substr(g + 1, h - g - 1)[0])) {
								nf_str = Unification(nf_str, nf_str.substr(g + 1, h - g - 1), query2[i].substr(e + 1, f - e - 1));
								std::cout << "BOTH ARGUMENTS ARE VARIABLE\n";
								match++;
							}
						}
						std::cout << "AFTER CASE-II\n";
						for (auto i = query2.begin(); i != query2.end(); ++i) std::cout << *i << "\n";

						// Case - III For last argument Eg: Play(_, _, y)
						e = query2[i].find(",", e + 1);
						f = query2[i].find(")");
						g = nf_str.find(",", g + 1);
						h = nf_str.find(")");
						if (query2[i].substr(e + 1, f - e - 1) == nf_str.substr(g + 1, h - g - 1) && (isupper(query2[i].substr(e + 1, f - e - 1)[0]) && isupper(nf_str.substr(g + 1, h - g - 1)[0]))) {
							std::cout << "BOTH ARGUMENTS ARE EQUAL\n";
							match++;
						}
						else if (islower(query2[i].substr(e + 1, f - e - 1)[0]) && isupper(nf_str.substr(g + 1, h - g - 1)[0])) {
							std::cout << "CLAUSE ARGUMENT IS VARIBALE\n";
							query2 = Unify(query2, query2[i].substr(e + 1, f - e - 1), nf_str.substr(g + 1, h - g - 1));
							std::cout << "AFTER UNIFICATION\n";
							for (auto i = query2.begin(); i != query2.end(); ++i) std::cout << *i << "\n";
							match++;
						}
						else if (isupper(query2[i].substr(e + 1, f - e - 1)[0]) && islower(nf_str.substr(g + 1, h - g - 1)[0])) {
							nf_str = Unification(nf_str, nf_str.substr(g + 1, h - g - 1), query2[i].substr(e + 1, f - e - 1));
							match++;
						}
						else if (islower(query2[i].substr(e + 1, f - e - 1)[0]) && islower(nf_str.substr(g + 1, h - g - 1)[0])) {
							nf_str = Unification(nf_str, nf_str.substr(g + 1, h - g - 1), query2[i].substr(e + 1, f - e - 1));
							std::cout << "BOTH ARGUMENTS ARE VARIABLE\n";
							match++;
						}

						int p = query2[i].find("(");
						int q = query2[i].find(")");
						int r = nf_str.find("(");
						int s = nf_str.find(")");

						std::cout << "AFTER CASE-III\n";
						for (auto i = query2.begin(); i != query2.end(); ++i) std::cout << *i << "\n";
						if (cl_arg == match) {
							std::cout << "CLAUSE ARGUMENT AND MATCH ARE EQUAL\n";
							std::vector<std::string> query3;
							int flag = 0;
							if (query2[i].substr(p + 1, q - p - 1) == nf_str.substr(r + 1, s - r - 1)) {
								int k = 1;
								while (k < query2.size()) {
									query3.push_back(query2[k]);
									k++;
								}
								std::cout << "QUERY3 SIZE:" << query3.size() << "\n";
								if (query3.size()) std::cout << "QUERY3 CONTENT\n";
								for (auto i = query3.begin(); i != query3.end(); ++i) std::cout << *i << "\n";
								result = FOL_engine(query3, 0, --depth);
								std::cout << "*********************\n";
								if (result) return true;
							}
							else {
								while (query2[i].substr(p + 1, q - p - 1) != nf_str.substr(r + 1, s - r - 1)) {
									if (flag == 0) {
										e = query2[i].find("(");
										f = query2[i].find(",");
										g = nf_str.find("(");
										h = nf_str.find(",");
										if (islower(query2[i].substr(e + 1, f - e - 1)[0]) && islower(nf_str.substr(g + 1, h - g - 1)[0])) {
											query2 = Unify(query2, query2[i].substr(e + 1, f - e - 1), nf_str.substr(g + 1, h - g - 1));
										}
										else if (islower(query2[i].substr(e + 1, f - e - 1)[0]) && isupper(nf_str.substr(g + 1, h - g - 1)[0])) {
											query2 = Unify(query2, query2[i].substr(e + 1, f - e - 1), nf_str.substr(g + 1, h - g - 1));
										}
										else if (isupper(query2[i].substr(e + 1, f - e - 1)[0]) && islower(nf_str.substr(g + 1, h - g - 1)[0])) {
											nf_str = Unification(nf_str, nf_str.substr(g + 1, h - g - 1), query2[i].substr(e + 1, f - e - 1));
										}
										for (int mid_arg = 0; mid_arg < cl_arg - 2; mid_arg++) {
											e = query2[i].find(",", e + 1);
											f = query2[i].find(",", e + 1);
											g = nf_str.find(",", g + 1);
											h = nf_str.find(",", g + 1);

											if (islower(query2[i].substr(e + 1, f - e - 1)[0]) && islower(nf_str.substr(g + 1, h - g - 1)[0])) {
												query2 = Unify(query2, query2[i].substr(e + 1, f - e - 1), nf_str.substr(g + 1, h - g - 1));
											}
											else if (islower(query2[i].substr(e + 1, f - e - 1)[0]) && isupper(nf_str.substr(g + 1, h - g - 1)[0])) {
												query2 = Unify(query2, query2[i].substr(e + 1, f - e - 1), nf_str.substr(g + 1, h - g - 1));
											}
											else if (isupper(query2[i].substr(e + 1, f - e - 1)[0]) && islower(nf_str.substr(g + 1, h - g - 1)[0])) {
												nf_str = Unification(nf_str, nf_str.substr(g + 1, h - g - 1), query2[i].substr(e + 1, f - e - 1));
											}
										}
										e = query2[i].find(",", e + 1);
										f = query2[i].find(")");
										g = nf_str.find(",", g + 1);
										h = nf_str.find(")");
										if (islower(query2[i].substr(e + 1, f - e - 1)[0]) && islower(nf_str.substr(g + 1, h - g - 1)[0])) {
											query2 = Unify(query2, query2[i].substr(e + 1, f - e - 1), nf_str.substr(g + 1, h - g - 1));
										}
										else if (islower(query2[i].substr(e + 1, f - e - 1)[0]) && isupper(nf_str.substr(g + 1, h - g - 1)[0])) {
											query2 = Unify(query2, query2[i].substr(e + 1, f - e - 1), nf_str.substr(g + 1, h - g - 1));
										}
										else if (isupper(query2[i].substr(e + 1, f - e - 1)[0]) && islower(nf_str.substr(g + 1, h - g - 1)[0])) {
											nf_str = Unification(nf_str, nf_str.substr(g + 1, h - g - 1), query2[i].substr(e + 1, f - e - 1));
										}
										flag = 1;
									}
									else if (flag == 1) {
										e = query2[i].find("(");
										f = query2[i].find(",");
										g = nf_str.find("(");
										h = nf_str.find(",");
										if (islower(query2[i].substr(e + 1, f - e - 1)[0]) && islower(nf_str.substr(g + 1, h - g - 1)[0])) {
											nf_str = Unification(nf_str, nf_str.substr(g + 1, h - g - 1), query2[i].substr(e + 1, f - e - 1));
										}
										else if (islower(query2[i].substr(e + 1, f - e - 1)[0]) && isupper(nf_str.substr(g + 1, h - g - 1)[0])) {
											query2 = Unify(query2, query2[i].substr(e + 1, f - e - 1), nf_str.substr(g + 1, h - g - 1));
										}
										else if (isupper(query2[i].substr(e + 1, f - e - 1)[0]) && islower(nf_str.substr(g + 1, h - g - 1)[0])) {
											nf_str = Unification(nf_str, nf_str.substr(g + 1, h - g - 1), query2[i].substr(e + 1, f - e - 1));
										}
										for (int mid_arg = 0; mid_arg < cl_arg - 2; mid_arg++) {
											e = query2[i].find(",", e + 1);
											f = query2[i].find(",", e + 1);
											g = nf_str.find(",", g + 1);
											h = nf_str.find(",", g + 1);

											if (islower(query2[i].substr(e + 1, f - e - 1)[0]) && islower(nf_str.substr(g + 1, h - g - 1)[0])) {
												nf_str = Unification(nf_str, nf_str.substr(g + 1, h - g - 1), query2[i].substr(e + 1, f - e - 1));
											}
											else if (islower(query2[i].substr(e + 1, f - e - 1)[0]) && isupper(nf_str.substr(g + 1, h - g - 1)[0])) {
												query2 = Unify(query2, query2[i].substr(e + 1, f - e - 1), nf_str.substr(g + 1, h - g - 1));
											}
											else if (isupper(query2[i].substr(e + 1, f - e - 1)[0]) && islower(nf_str.substr(g + 1, h - g - 1)[0])) {
												nf_str = Unification(nf_str, nf_str.substr(g + 1, h - g - 1), query2[i].substr(e + 1, f - e - 1));
											}
										}
										e = query2[i].find(",", e + 1);
										f = query2[i].find(")");
										g = nf_str.find(",", g + 1);
										h = nf_str.find(")");
										if (islower(query2[i].substr(e + 1, f - e - 1)[0]) && islower(nf_str.substr(g + 1, h - g - 1)[0])) {
											nf_str = Unification(nf_str, nf_str.substr(g + 1, h - g - 1), query2[i].substr(e + 1, f - e - 1));
										}
										else if (islower(query2[i].substr(e + 1, f - e - 1)[0]) && isupper(nf_str.substr(g + 1, h - g - 1)[0])) {
											query2 = Unify(query2, query2[i].substr(e + 1, f - e - 1), nf_str.substr(g + 1, h - g - 1));
										}
										else if (isupper(query2[i].substr(e + 1, f - e - 1)[0]) && islower(nf_str.substr(g + 1, h - g - 1)[0])) {
											nf_str = Unification(nf_str, nf_str.substr(g + 1, h - g - 1), query2[i].substr(e + 1, f - e - 1));
										}
										flag = 0;
									}
									p = query2[i].find("(");
									q = query2[i].find(")");
									r = nf_str.find("(");
									s = nf_str.find(")");
								}
								int k = 1;
								while (k < query2.size()) {
									query3.push_back(query2[k]);
									k++;
								}
								std::cout << "QUERY3 SIZE:" << query3.size() << "\n";
								if (query3.size()) std::cout << "QUERY3 CONTENT\n";
								for (auto i = query3.begin(); i != query3.end(); ++i) std::cout << *i << "\n";
								result = FOL_engine(query3, 0, --depth);
								std::cout << "*********************\n";
								if (result) return true;
							}
						}
					}
				}
			}
			// Check for Negative Equations
			if (negative_equations.count(token) > 0) {
				std::cout << "CHECKING IN NEGATIVE EQUATIONS" << "\n";
				std::string eq_token;
				std::vector<std::string> all_tok_eq;  // Storing all the equations for respective tokens
				std::vector<std::string> query2;
				for (int j = 0; j < negative_equations[token].size(); j++) {
					for (int k = 0; k < negative_equations[token][j].size(); k++) {
						eq_token = negative_equations[token][j][k].substr(1, negative_equations[token][j][k].find("(") - 1);
						std::cout << eq_token << "\n";
						if (token == eq_token) {
							all_tok_eq = negative_equations[token][j];

							// For single arguments
							if (cl_arg == 1) {
								std::cout << "FOR SINGLE ARGUMENT\n";
								e = query[i].find("(");
								f = query[i].find(")");
								g = all_tok_eq[k].find("(");
								h = all_tok_eq[k].find(")");
								if ((query[i].substr(e + 1, f - e - 1) == all_tok_eq[k].substr(g + 1, h - g - 1)) && isupper(query[i].substr(e + 1, f - e - 1)[0]) && isupper(all_tok_eq[k].substr(g + 1, h - g - 1)[0])) {
									std::cout << "BOTH ARGUMENTS ARE CONSTANT\n";
									std::vector<std::string> equation = all_tok_eq;
									query2 = Resolution(query, equation);
									std::cout << "AFTER MULTIPLE RESOLUTION\n";
									//int d = i + 1;
									//while (d < query.size()) {
										//query2.push_back(query[d]);
										//d++;
									//}
									//for (int left = 0; left < all_tok_eq.size(); left++) {
										//if (left != k) query2.push_back(all_tok_eq[left]);
									//}
									std::cout << "QUERY2 SIZE:" << query2.size() << "\n";
									if (query2.size()) std::cout << "CONTENT OF QUERY2\n";
									for (auto i = query2.begin(); i != query2.end(); ++i) std::cout << *i << "\n";
									result = FOL_engine(query2, 0, ++depth);
									if (result) return true;
								}
								else if (islower(query[i].substr(e + 1, f - e - 1)[0]) && isupper(all_tok_eq[k].substr(g + 1, h - g - 1)[0])) {
									std::cout << "CLAUSE ARGUMENT IS VARIABLE AND EQUATION ARGUMENT IS CONSTANT\n";
									std::vector<std::string> query3 = Unify(query, query[i].substr(e + 1, f - e - 1), all_tok_eq[k].substr(g + 1, h - g - 1));
									std::cout << "AFTER UNIFICATION\n";
									for (auto i = query3.begin(); i != query3.end(); ++i) std::cout << *i << "\n";
									std::vector<std::string> equation = all_tok_eq;
									query2 = Resolution(query3, equation);
									std::cout << "AFTER MULTIPLE RESOLUTION\n";
									//int d = i + 1;
									//while (d < query3.size()) {
										//query2.push_back(query3[d]);
										//d++;
									//}
									//for (int left = 0; left < all_tok_eq.size(); left++) {
										//if (left != k) query2.push_back(all_tok_eq[left]);
									//}
									std::cout << "QUERY3 SIZE:" << query3.size() << "\n";
									if (query3.size()) std::cout << "CONTENTS OF QUERY3\n";
									for (auto i = query3.begin(); i != query3.end(); ++i) std::cout << *i << "\n";
									result = FOL_engine(query2, 0, ++depth);
									if (result) return true;

								}
								else if (islower(all_tok_eq[k].substr(g + 1, h - g - 1)[0])) {
									std::cout << "BOTH ARGUMENTS ARE VARIABLE\n";
									all_tok_eq = Unify(all_tok_eq, all_tok_eq[k].substr(g + 1, h - g - 1), query[i].substr(e + 1, f - e - 1));
									std::cout << "AFTER UNIFICATION IN EQUATION\n";
									for (auto i = all_tok_eq.begin(); i != all_tok_eq.end(); ++i) std::cout << *i << "\n";
									std::vector<std::string> equation = all_tok_eq;
									query2 = Resolution(query, equation);
									std::cout << "AFTER MULTIPLE RESOLUTION\n";
									//int d = i + 1;
									//while (d < query.size()) {
										//query2.push_back(query[d]);
										//d++;
									//}
									//for (int left = 0; left < all_tok_eq.size(); left++) {
										//if (left != k) query2.push_back(all_tok_eq[left]);
									//}
									std::cout << "QUERY2 SIZE:" << query2.size() << "\n";
									if (query2.size()) std::cout << "CONTENTS OF QUERY2\n";
									for (auto i = query2.begin(); i != query2.end(); ++i) std::cout << *i << "\n";
									result = FOL_engine(query2, 0, --depth);
									std::cout << "*********************\n";
									if (result) return true;
								}
							}
							else if (cl_arg > 1) {
								std::cout << "ARGUMENTS ARE GREATER THAN ONE\n";
								int match = 0;
								std::vector<std::string> query3 = query;
								// Case - I For the first argument
								e = query3[i].find("(");
								f = query3[i].find(",");
								g = all_tok_eq[k].find("(");
								h = all_tok_eq[k].find(",");
								if ((query3[i].substr(e + 1, f - e - 1) == all_tok_eq[k].substr(g + 1, h - g - 1)) && isupper(query3[i].substr(e + 1, f - e - 1)[0]) && isupper(all_tok_eq[k].substr(g + 1, h - g - 1)[0])) {
									std::cout << "BOTH ARGUMENTS ARE CONSTANT\n";
									match++;
								}
								else if (islower(query3[i].substr(e + 1, f - e - 1)[0]) && isupper(all_tok_eq[k].substr(g + 1, h - g - 1)[0])) {
									std::cout << "CLAUSE AREGUMENT IS VARIABLE AND EQUATION ARGUMENT IS CONSTANT\n";
									query3 = Unify(query3, query3[i].substr(e + 1, f - e - 1), all_tok_eq[k].substr(g + 1, h - g - 1));
									std::cout << "AFTER UNIFICATION\n";
									for (auto i = query3.begin(); i < query3.end(); ++i) std::cout << *i << "\n";
									match++;
								}
								else if (islower(all_tok_eq[k].substr(g + 1, h - g - 1)[0])) {
									std::cout << "BOTH ARGUMENTS ARE VARIBALE\n";
									all_tok_eq = Unify(all_tok_eq, all_tok_eq[k].substr(g + 1, h - g - 1), query3[i].substr(e + 1, f - e - 1));
									std::cout << "AFTER UNIFICATION IN EQUATION\n";
									for (auto i = all_tok_eq.begin(); i < all_tok_eq.end(); ++i) std::cout << *i << "\n";
									match++;
								}
								std::cout << "AFTER CASE-I\n";
								for (auto i = query3.begin(); i != query3.end(); ++i) std::cout << *i << "\n";
								// Case - II For mid arguments (Except First and last arguments)
								std::cout << "IN CASE-II\n";
								for (int mid_arg = 0; mid_arg < cl_arg - 2; mid_arg++) {
									e = query3[i].find(",", e + 1);
									f = query3[i].find(",", e + 1);
									g = all_tok_eq[k].find(",", g + 1);
									h = all_tok_eq[k].find(",", g + 1);
									if ((query3[i].substr(e + 1, f - e - 1) == all_tok_eq[k].substr(g + 1, h - g - 1)) && isupper(query3[i].substr(e + 1, f - e - 1)[0]) && isupper(all_tok_eq[k].substr(g + 1, h - g - 1)[0])) {
										match++;
									}
									else if (islower(query3[i].substr(e + 1, f - e - 1)[0]) && isupper(all_tok_eq[k].substr(g + 1, h - g - 1)[0])) {
										query3 = Unify(query3, query3[i].substr(e + 1, f - e - 1), all_tok_eq[k].substr(g + 1, h - g - 1));
										match++;
									}
									else if (islower(all_tok_eq[k].substr(g + 1, h - g - 1)[0])) {
										all_tok_eq = Unify(all_tok_eq, all_tok_eq[k].substr(g + 1, h - g - 1), query3[i].substr(e + 1, f - e - 1));
										for (auto i = all_tok_eq.begin(); i < all_tok_eq.end(); ++i) std::cout << *i << "\n";
										match++;
									}
								}
								std::cout << "AFTER CASE-II\n";
								for (auto i = query3.begin(); i != query3.end(); ++i) std::cout << *i << "\n";

								// Case - III For Last arguments
								e = query3[i].find(",", e + 1);
								f = query3[i].find(")");
								g = all_tok_eq[k].find(",", g + 1);
								h = all_tok_eq[k].find(")");

								if ((query3[i].substr(e + 1, f - e - 1) == all_tok_eq[k].substr(g + 1, h - g - 1)) && isupper(query3[i].substr(e + 1, f - e - 1)[0]) && isupper(all_tok_eq[k].substr(g + 1, h - g - 1)[0])) {
									std::cout << "BOTH ARE CONSTANT\n";
									match++;
								}
								else if (islower(query3[i].substr(e + 1, f - e - 1)[0]) && isupper(all_tok_eq[k].substr(g + 1, h - g - 1)[0])) {
									std::cout << "CLAUSE ARGUMENT IS VARIABLE AND EQUATION ARGUMENT IS CONSTANT\n";
									query3 = Unify(query3, query3[i].substr(e + 1, f - e - 1), all_tok_eq[k].substr(g + 1, h - g - 1));
									std::cout << "AFTER UNIFICAITON\n";
									for (auto i = query3.begin(); i != query3.end(); ++i) std::cout << *i << "\n";
									match++;
								}
								else if (islower(all_tok_eq[k].substr(g + 1, h - g - 1)[0])) {
									std::cout << "BOTH ARE VARIABLE\n";
									all_tok_eq = Unify(all_tok_eq, all_tok_eq[k].substr(g + 1, h - g - 1), query3[i].substr(e + 1, f - e - 1));
									std::cout << "AFTER UNIFICATION IN VARIBALE\n";
									for (auto i = all_tok_eq.begin(); i < all_tok_eq.end(); ++i) std::cout << *i << "\n";
									match++;
								}

								int p = query3[i].find("(");
								int q = query3[i].find(")");
								int r = all_tok_eq[k].find("(");
								int s = all_tok_eq[k].find(")");

								if (cl_arg == match) {
									int flag = 0;
									if (query3[i].substr(p + 1, q - p - 1) == all_tok_eq[k].substr(r + 1, s - r - 1)) {
										std::cout << "CLAUSE ARGUMENT EQUALS MATCH\n";
										std::vector<std::string> equation = all_tok_eq;
										query2 = Resolution(query3, equation);
										std::cout << "AFTER MULTIPLE RESOLUTION\n";
										std::cout << "QUERY2 SIZE:" << query2.size() << "\n";
										if (query2.size()) std::cout << "CONTENTS OF QUERY2\n";
										for (auto i = query2.begin(); i < query2.end(); ++i) std::cout << *i << "\n";
										result = FOL_engine(query2, 0, --depth);
										std::cout << "*********************\n";
										if (result) return true;
									}
									else {
										while (query3[i].substr(p + 1, q - p - 1) != all_tok_eq[k].substr(r + 1, s - r - 1)) {
											if (flag == 0) {
												e = query3[i].find("(");
												f = query3[i].find(",");
												g = all_tok_eq[k].find("(");
												h = all_tok_eq[k].find(",");
												if (islower(query3[i].substr(e + 1, f - e - 1)[0]) && islower(all_tok_eq[k].substr(g + 1, h - g - 1)[0])) {
													query3 = Unify(query3, query3[i].substr(e + 1, f - e - 1), all_tok_eq[k].substr(g + 1, h - g - 1));
												}
												else if (islower(query3[i].substr(e + 1, f - e - 1)[0]) && isupper(all_tok_eq[k].substr(g + 1, h - g - 1)[0])) {
													query3 = Unify(query3, query3[i].substr(e + 1, f - e - 1), all_tok_eq[k].substr(g + 1, h - g - 1));
												}
												else if (isupper(query3[i].substr(e + 1, f - e - 1)[0]) && islower(all_tok_eq[k].substr(g + 1, h - g - 1)[0])) {
													all_tok_eq = Unify(all_tok_eq, all_tok_eq[k].substr(g + 1, h - g - 1), query3[i].substr(e + 1, f - e - 1));
												}
												for (int mid_arg = 0; mid_arg < cl_arg - 2; mid_arg++) {
													e = query3[i].find(",", e + 1);
													f = query3[i].find(",", e + 1);
													g = all_tok_eq[k].find(",", g + 1);
													h = all_tok_eq[k].find(",", g + 1);
													if (islower(query3[i].substr(e + 1, f - e - 1)[0]) && islower(all_tok_eq[k].substr(g + 1, h - g - 1)[0])) {
														query3 = Unify(query3, query3[i].substr(e + 1, f - e - 1), all_tok_eq[k].substr(g + 1, h - g - 1));
													}
													else if (islower(query3[i].substr(e + 1, f - e - 1)[0]) && isupper(all_tok_eq[k].substr(g + 1, h - g - 1)[0])) {
														query3 = Unify(query3, query3[i].substr(e + 1, f - e - 1), all_tok_eq[k].substr(g + 1, h - g - 1));
													}
													else if (isupper(query3[i].substr(e + 1, f - e - 1)[0]) && islower(all_tok_eq[k].substr(g + 1, h - g - 1)[0])) {
														all_tok_eq = Unify(all_tok_eq, all_tok_eq[k].substr(g + 1, h - g - 1), query3[i].substr(e + 1, f - e - 1));
													}
												}
												e = query3[i].find(",", e + 1);
												f = query3[i].find(")");
												g = all_tok_eq[k].find(",", g + 1);
												h = all_tok_eq[k].find(")");
												if (islower(query3[i].substr(e + 1, f - e - 1)[0]) && islower(all_tok_eq[k].substr(g + 1, h - g - 1)[0])) {
													query3 = Unify(query3, query3[i].substr(e + 1, f - e - 1), all_tok_eq[k].substr(g + 1, h - g - 1));
												}
												else if (islower(query3[i].substr(e + 1, f - e - 1)[0]) && isupper(all_tok_eq[k].substr(g + 1, h - g - 1)[0])) {
													query3 = Unify(query3, query3[i].substr(e + 1, f - e - 1), all_tok_eq[k].substr(g + 1, h - g - 1));
												}
												else if (isupper(query3[i].substr(e + 1, f - e - 1)[0]) && islower(all_tok_eq[k].substr(g + 1, h - g - 1)[0])) {
													all_tok_eq = Unify(all_tok_eq, all_tok_eq[k].substr(g + 1, h - g - 1), query3[i].substr(e + 1, f - e - 1));
												}
												flag = 1;
											}
											else if (flag == 1) {
												e = query3[i].find("(");
												f = query3[i].find(",");
												g = all_tok_eq[k].find("(");
												h = all_tok_eq[k].find(",");
												if (islower(query3[i].substr(e + 1, f - e - 1)[0]) && islower(all_tok_eq[k].substr(g + 1, h - g - 1)[0])) {
													all_tok_eq = Unify(all_tok_eq, all_tok_eq[k].substr(g + 1, h - g - 1), query3[i].substr(e + 1, f - e - 1));
												}
												else if (islower(query3[i].substr(e + 1, f - e - 1)[0]) && isupper(all_tok_eq[k].substr(g + 1, h - g - 1)[0])) {
													query3 = Unify(query3, query3[i].substr(e + 1, f - e - 1), all_tok_eq[k].substr(g + 1, h - g - 1));
												}
												else if (isupper(query3[i].substr(e + 1, f - e - 1)[0]) && islower(all_tok_eq[k].substr(g + 1, h - g - 1)[0])) {
													all_tok_eq = Unify(all_tok_eq, all_tok_eq[k].substr(g + 1, h - g - 1), query3[i].substr(e + 1, f - e - 1));
												}
												for (int mid_arg = 0; mid_arg < cl_arg - 2; mid_arg++) {
													e = query3[i].find(",", e + 1);
													f = query3[i].find(",", e + 1);
													g = all_tok_eq[k].find(",", g + 1);
													h = all_tok_eq[k].find(",", g + 1);
													if (islower(query3[i].substr(e + 1, f - e - 1)[0]) && islower(all_tok_eq[k].substr(g + 1, h - g - 1)[0])) {
														all_tok_eq = Unify(all_tok_eq, all_tok_eq[k].substr(g + 1, h - g - 1), query3[i].substr(e + 1, f - e - 1));
													}
													else if (islower(query3[i].substr(e + 1, f - e - 1)[0]) && isupper(all_tok_eq[k].substr(g + 1, h - g - 1)[0])) {
														query3 = Unify(query3, query3[i].substr(e + 1, f - e - 1), all_tok_eq[k].substr(g + 1, h - g - 1));
													}
													else if (isupper(query3[i].substr(e + 1, f - e - 1)[0]) && islower(all_tok_eq[k].substr(g + 1, h - g - 1)[0])) {
														all_tok_eq = Unify(all_tok_eq, all_tok_eq[k].substr(g + 1, h - g - 1), query3[i].substr(e + 1, f - e - 1));
													}
												}
												e = query3[i].find(",", e + 1);
												f = query3[i].find(")");
												g = all_tok_eq[k].find(",", g + 1);
												h = all_tok_eq[k].find(")");
												if (islower(query3[i].substr(e + 1, f - e - 1)[0]) && islower(all_tok_eq[k].substr(g + 1, h - g - 1)[0])) {
													all_tok_eq = Unify(all_tok_eq, all_tok_eq[k].substr(g + 1, h - g - 1), query3[i].substr(e + 1, f - e - 1));
												}
												else if (islower(query3[i].substr(e + 1, f - e - 1)[0]) && isupper(all_tok_eq[k].substr(g + 1, h - g - 1)[0])) {
													query3 = Unify(query3, query3[i].substr(e + 1, f - e - 1), all_tok_eq[k].substr(g + 1, h - g - 1));
												}
												else if (isupper(query3[i].substr(e + 1, f - e - 1)[0]) && islower(all_tok_eq[k].substr(g + 1, h - g - 1)[0])) {
													all_tok_eq = Unify(all_tok_eq, all_tok_eq[k].substr(g + 1, h - g - 1), query3[i].substr(e + 1, f - e - 1));
												}
												flag = 0;
											}
											p = query3[i].find("(");
											q = query3[i].find(")");
											r = all_tok_eq[k].find("(");
											s = all_tok_eq[k].find(")");
										}
										std::cout << "CLAUSE ARGUMENT EQUALS MATCH\n";
										std::vector<std::string> equation = all_tok_eq;
										query2 = Resolution(query3, equation);
										std::cout << "AFTER MULTIPLE RESOLUTION\n";
										std::cout << "QUERY2 SIZE:" << query2.size() << "\n";
										if (query2.size()) std::cout << "CONTENTS OF QUERY2\n";
										for (auto i = query2.begin(); i < query2.end(); ++i) std::cout << *i << "\n";
										result = FOL_engine(query2, 0, --depth);
										std::cout << "*********************\n";
										if (result) return true;
									}
								}
							}
						}
					}
				}
			}
		}
	}
	return false;
}

int main() {
	std::string temp_query;
	std::string temp_num_KB;
	int nq, nkb;  // nq: Number of Query, nkb: Total number of KB
	std::string query[10];
	std::string KB[100];
	std::string s;
	std::ifstream myfile("input20.txt");
	std::getline(myfile, temp_query);
	nq = std::stoi(temp_query);
	for (int i = 0; i < nq; i++) {
		std::getline(myfile, query[i]);
	}
	std::getline(myfile, temp_num_KB);
	nkb = std::stoi(temp_num_KB);
	for (int i = 0; i < nkb; i++) {
		std::getline(myfile, KB[i]);
	}
	// CNF Conversion for KB

	std::vector<std::vector<std::string>> KB_matrix;
	std::vector<std::string> KB_facts_matrix;
	//int facts_counter = 0;
	std::vector<std::string> temp_KB_string;
	//std::string cnf_KB[100];
	for (int i = 0; i < nkb; i++) {
		std::size_t found = KB[i].find("=>");
		if (found != std::string::npos) {
			std::stringstream ss(KB[i]);
			std::string word;
			int implies = 0;
			//int k = 0;
			std::vector<std::string> temp_KB_string;
			while (ss >> word) {
				//std::cout << word << "\n";
				if (implies == 0) {
					if (word[0] == '~') {
						word.erase(word.begin());
						temp_KB_string.push_back(word);
						//KB_matrix[i][k] = word;
						//k++;
					}
					else if (word == "=>") implies = 1;
					else if (word != "&" && word != "=>") {
						word.insert(0, "~");
						temp_KB_string.push_back(word);
						//KB_matrix[i][k] = word;
						//k++;
					}
				}
				else {
					temp_KB_string.push_back(word);
					//KB_matrix[i][k] = word;
					//k++;
				}
			}
			KB_matrix.push_back(temp_KB_string);
		}
		else {
			KB_facts_matrix.push_back(KB[i]);
			//facts_counter++;
		}
	}

	// Standardizing the variabels For Implications
	for (int i = 0; i < KB_matrix.size(); i++) {
		for (int j = 0; j < KB_matrix[i].size(); j++) {
			int arguments = std::count(KB_matrix[i][j].begin(), KB_matrix[i][j].end(), ',') + 1;
			if (arguments == 1) {
				int a = KB_matrix[i][j].find("(");
				int b = KB_matrix[i][j].find(")");
				if (islower(KB_matrix[i][j].substr(a + 1, b - a - 1)[0])) {
					if (standard_variables.count(KB_matrix[i][j].substr(a + 1, b - a - 1)) == 0) {
						standard_variables[KB_matrix[i][j].substr(a + 1, b - a - 1)] = i;
					}
					else if (standard_variables.count(KB_matrix[i][j].substr(a + 1, b - a - 1)) > 0 && standard_variables[KB_matrix[i][j].substr(a + 1, b - a - 1)] != i) {
						std::string str;
						while (standard_variables.count(KB_matrix[i][j].substr(a + 1, b - a - 1)) != 0) {
							str = KB_matrix[i][j].substr(a + 1, b - a - 1);
							str += KB_matrix[i][j].substr(a + 1, b - a - 1)[0];
							KB_matrix[i] = Unify(KB_matrix[i], KB_matrix[i][j].substr(a + 1, b - a - 1), str);
							b++;
						}
						standard_variables[KB_matrix[i][j].substr(a + 1, b - a - 1)] = i;
					}
				}
			}
			else if (arguments > 1) {
				int a = KB_matrix[i][j].find("(");
				int b = KB_matrix[i][j].find(",");
				if (islower(KB_matrix[i][j].substr(a + 1, b - a - 1)[0])) {
					if (standard_variables.count(KB_matrix[i][j].substr(a + 1, b - a - 1)) == 0) {
						standard_variables[KB_matrix[i][j].substr(a + 1, b - a - 1)] = i;
					}
					else if (standard_variables.count(KB_matrix[i][j].substr(a + 1, b - a - 1)) > 0 && standard_variables[KB_matrix[i][j].substr(a + 1, b - a - 1)] != i) {
						std::string str;
						while (standard_variables.count(KB_matrix[i][j].substr(a + 1, b - a - 1)) != 0) {
							str = KB_matrix[i][j].substr(a + 1, b - a - 1);
							str += KB_matrix[i][j].substr(a + 1, b - a - 1)[0];
							KB_matrix[i] = Unify(KB_matrix[i], KB_matrix[i][j].substr(a + 1, b - a - 1), str);
							b++;
						}
						standard_variables[KB_matrix[i][j].substr(a + 1, b - a - 1)] = i;
					}
				}
				for (int k = 0; k < arguments - 2; k++) {
					a = KB_matrix[i][j].find(",", a + 1);
					b = KB_matrix[i][j].find(",", a + 1);
					if (islower(KB_matrix[i][j].substr(a + 1, b - a - 1)[0])) {
						if (standard_variables.count(KB_matrix[i][j].substr(a + 1, b - a - 1)) == 0) {
							standard_variables[KB_matrix[i][j].substr(a + 1, b - a - 1)] = i;
						}
						else if (standard_variables.count(KB_matrix[i][j].substr(a + 1, b - a - 1)) > 0 && standard_variables[KB_matrix[i][j].substr(a + 1, b - a - 1)] != i) {
							std::string str;
							while (standard_variables.count(KB_matrix[i][j].substr(a + 1, b - a - 1)) != 0) {
								str = KB_matrix[i][j].substr(a + 1, b - a - 1);
								str += KB_matrix[i][j].substr(a + 1, b - a - 1)[0];
								KB_matrix[i] = Unify(KB_matrix[i], KB_matrix[i][j].substr(a + 1, b - a - 1), str);
								b++;
							}
							standard_variables[KB_matrix[i][j].substr(a + 1, b - a - 1)] = i;
						}
					}
				}
				a = KB_matrix[i][j].find(",", a + 1);
				b = KB_matrix[i][j].find(")");
				if (islower(KB_matrix[i][j].substr(a + 1, b - a - 1)[0])) {
					if (standard_variables.count(KB_matrix[i][j].substr(a + 1, b - a - 1)) == 0) {
						standard_variables[KB_matrix[i][j].substr(a + 1, b - a - 1)] = i;
					}
					else if (standard_variables.count(KB_matrix[i][j].substr(a + 1, b - a - 1)) > 0 && standard_variables[KB_matrix[i][j].substr(a + 1, b - a - 1)] != i) {
						std::string str;
						while (standard_variables.count(KB_matrix[i][j].substr(a + 1, b - a - 1)) != 0) {
							str = KB_matrix[i][j].substr(a + 1, b - a - 1);
							str += KB_matrix[i][j].substr(a + 1, b - a - 1)[0];
							KB_matrix[i] = Unify(KB_matrix[i], KB_matrix[i][j].substr(a + 1, b - a - 1), str);
							b++;
						}
						standard_variables[KB_matrix[i][j].substr(a + 1, b - a - 1)] = i;
					}
				}
			}
		}
	}

	// Standardizing the Facts
	for (int i = 0; i < KB_facts_matrix.size(); i++) {
		int arguments = std::count(KB_facts_matrix[i].begin(), KB_facts_matrix[i].end(), ',') + 1;
		if (arguments == 1) {
			int a = KB_facts_matrix[i].find("(");
			int b = KB_facts_matrix[i].find(")");
			if (islower(KB_facts_matrix[i].substr(a + 1, b - a - 1)[0])) {
				if (standard_variables_f.count(KB_facts_matrix[i].substr(a + 1, b - a - 1)) == 0) {
					standard_variables_f[KB_facts_matrix[i].substr(a + 1, b - a - 1)] = i;
				}
				else if (standard_variables_f.count(KB_facts_matrix[i].substr(a + 1, b - a - 1)) > 0 && standard_variables_f[KB_facts_matrix[i].substr(a + 1, b - a - 1)] != i) {
					std::string str;
					std::vector<std::string> f;
					f.push_back(KB_facts_matrix[i].substr(a + 1, b - a - 1));
					while (standard_variables_f.count(KB_facts_matrix[i].substr(a + 1, b - a - 1)) != 0) {
						str = KB_facts_matrix[i].substr(a + 1, b - a - 1);
						str += KB_facts_matrix[i].substr(a + 1, b - a - 1)[0];
						f = Unify(f, KB_facts_matrix[i].substr(a + 1, b - a - 1), str);
						KB_facts_matrix[i] = f[0];
						b++;
					}
					standard_variables_f[KB_facts_matrix[i].substr(a + 1, b - a - 1)] = i;
				}
			}
		}
		else if (arguments > 1) {
			int a = KB_facts_matrix[i].find("(");
			int b = KB_facts_matrix[i].find(",");
			if (islower(KB_facts_matrix[i].substr(a + 1, b - a - 1)[0])) {
				if (standard_variables_f.count(KB_facts_matrix[i].substr(a + 1, b - a - 1)) == 0) {
					standard_variables_f[KB_facts_matrix[i].substr(a + 1, b - a - 1)] = i;
				}
				else if (standard_variables_f.count(KB_facts_matrix[i].substr(a + 1, b - a - 1)) > 0 && standard_variables_f[KB_facts_matrix[i].substr(a + 1, b - a - 1)] != i) {
					std::string str;
					std::vector<std::string> f;
					f.push_back(KB_facts_matrix[i].substr(a + 1, b - a - 1));
					while (standard_variables_f.count(KB_facts_matrix[i].substr(a + 1, b - a - 1)) != 0) {
						str = KB_facts_matrix[i].substr(a + 1, b - a - 1);
						str += KB_facts_matrix[i].substr(a + 1, b - a - 1)[0];
						f = Unify(f, KB_facts_matrix[i].substr(a + 1, b - a - 1), str);
						KB_facts_matrix[i] = f[0];
						//KB_facts_matrix[i] = Unify(KB_facts_matrix[i], KB_facts_matrix[i].substr(a + 1, b - a - 1), str);
						b++;
					}
					standard_variables_f[KB_facts_matrix[i].substr(a + 1, b - a - 1)] = i;
				}
			}
			for (int j = 0; j < arguments - 2; j++) {
				a = KB_facts_matrix[i].find(",", a + 1);
				b = KB_facts_matrix[i].find(",", a + 1);
				if (islower(KB_facts_matrix[i].substr(a + 1, b - a - 1)[0])) {
					if (standard_variables_f.count(KB_facts_matrix[i].substr(a + 1, b - a - 1)) == 0) {
						standard_variables_f[KB_facts_matrix[i].substr(a + 1, b - a - 1)] = i;
					}
					else if (standard_variables_f.count(KB_facts_matrix[i].substr(a + 1, b - a - 1)) > 0 && standard_variables_f[KB_facts_matrix[i].substr(a + 1, b - a - 1)] != i) {
						std::string str;
						std::vector<std::string> f;
						f.push_back(KB_facts_matrix[i].substr(a + 1, b - a - 1));
						while (standard_variables_f.count(KB_facts_matrix[i].substr(a + 1, b - a - 1)) != 0) {
							str = KB_facts_matrix[i].substr(a + 1, b - a - 1);
							str += KB_facts_matrix[i].substr(a + 1, b - a - 1)[0];
							f = Unify(f, KB_facts_matrix[i].substr(a + 1, b - a - 1), str);
							KB_facts_matrix[i] = f[0];
							//KB_facts_matrix[i] = Unify(KB_facts_matrix[i], KB_facts_matrix[i].substr(a + 1, b - a - 1), str);
							b++;
						}
						standard_variables_f[KB_facts_matrix[i].substr(a + 1, b - a - 1)] = i;
					}
				}
			}
			a = KB_facts_matrix[i].find(",", a + 1);
			b = KB_facts_matrix[i].find(")");
			if (islower(KB_facts_matrix[i].substr(a + 1, b - a - 1)[0])) {
				if (standard_variables_f.count(KB_facts_matrix[i].substr(a + 1, b - a - 1)) == 0) {
					standard_variables_f[KB_facts_matrix[i].substr(a + 1, b - a - 1)] = i;
				}
				else if (standard_variables_f.count(KB_facts_matrix[i].substr(a + 1, b - a - 1)) > 0 && standard_variables_f[KB_facts_matrix[i].substr(a + 1, b - a - 1)] != i) {
					std::string str;
					std::vector<std::string> f;
					f.push_back(KB_facts_matrix[i].substr(a + 1, b - a - 1));
					while (standard_variables_f.count(KB_facts_matrix[i].substr(a + 1, b - a - 1)) != 0) {
						str = KB_facts_matrix[i].substr(a + 1, b - a - 1);
						str += KB_facts_matrix[i].substr(a + 1, b - a - 1)[0];
						f = Unify(f, KB_facts_matrix[i].substr(a + 1, b - a - 1), str);
						KB_facts_matrix[i] = f[0];
						//KB_facts_matrix[i] = Unify(KB_facts_matrix[i], KB_facts_matrix[i].substr(a + 1, b - a - 1), str);
						b++;
					}
					standard_variables_f[KB_facts_matrix[i].substr(a + 1, b - a - 1)] = i;
				}
			}
		}
	}


	// Facts Token Initialization
	for (int i = 0; i < KB_facts_matrix.size(); i++) {
		if (KB_facts_matrix[i][0] == '~') {
			negative_facts[KB_facts_matrix[i].substr(1, KB_facts_matrix[i].find("(") - 1)];
		}
		else {
			positive_facts[KB_facts_matrix[i].substr(0, KB_facts_matrix[i].find("("))];
		}
	}

	// Negative Facts Value Insertion
	for (auto it = negative_facts.begin(); it != negative_facts.end(); it++) {
		for (int i = 0; i < KB_facts_matrix.size(); i++) {
			if (it->first == KB_facts_matrix[i].substr(1, KB_facts_matrix[i].find("(") - 1)) {
				negative_facts[it->first].push_back(KB_facts_matrix[i]);
			}
		}
	}
	// Positive Facts Value Insertion
	for (auto it = positive_facts.begin(); it != positive_facts.end(); it++) {
		for (int i = 0; i < KB_facts_matrix.size(); i++) {
			if (it->first == KB_facts_matrix[i].substr(0, KB_facts_matrix[i].find("("))) {
				positive_facts[it->first].push_back(KB_facts_matrix[i]);
			}
		}
	}

	// Equations Token Initialization
	for (int i = 0; i < KB_matrix.size(); i++) {
		for (int j = 0; j < KB_matrix[i].size(); j++) {
			if (KB_matrix[i][j][0] == '~') {
				negative_equations[KB_matrix[i][j].substr(1, KB_matrix[i][j].find("(") - 1)];
			}
			else {
				positive_equations[KB_matrix[i][j].substr(0, KB_matrix[i][j].find("("))];
			}
		}
	}
	// Negative Equations Value Insertion
	for (auto it = negative_equations.begin(); it != negative_equations.end(); it++) {
		int a = 0;
		for (int i = 0; i < KB_matrix.size(); i++) {
			int k = 0;
			int match = 0;
			//std::cout << "HEEELLLLOOO\n";
			while (k < KB_matrix[i].size()) {
				if (it->first == KB_matrix[i][k].substr(1, KB_matrix[i][k].find("(") - 1) && KB_matrix[i][k][0] == '~') {
					//std::cout << KB_matrix[i][k].substr(1, KB_matrix[i][k].find("(") - 1) << "\n";
					match = 1;
					break;
				}
				k++;
			}
			if (match == 1) {
				k = 0;
				std::vector<std::string> eq;
				while (k < KB_matrix[i].size()) {
					eq.push_back(KB_matrix[i][k]);
					k++;
				}
				it->second.push_back(eq);
				a++;
			}
		}
	}

	// Positive Equations Value Insertion
	for (auto it = positive_equations.begin(); it != positive_equations.end(); it++) {
		int a = 0;
		for (int i = 0; i < KB_matrix.size(); i++) {
			int k = 0;
			int match = 0;
			while (k < KB_matrix[i].size()) {
				if (it->first == KB_matrix[i][k].substr(0, KB_matrix[i][k].find("("))) {
					match = 1;
					break;
				}
				k++;
			}
			if (match == 1) {
				k = 0;
				std::vector<std::string> eq;
				while (k < KB_matrix[i].size()) {
					eq.push_back(KB_matrix[i][k]);
					k++;
				}
				it->second.push_back(eq);
				a++;
			}
		}
	}
	for (int i = 0; i < KB_facts_matrix.size(); i++) {
		std::cout << KB_facts_matrix[i] << "\n";
	}
	for (int i = 0; i < KB_matrix.size(); i++) {
		for (int j = 0; j < KB_matrix[i].size(); j++) {
			std::cout << KB_matrix[i][j] << " ";
		}
		std::cout << "\n";
	}
	int depth = 20;
	std::vector<std::string> result_vec;
	for (int i = 0; i < nq; i++) {
		std::vector<std::string> q;
		std::string den;
		den = query[i];
		if (den[0] != '~') {
			den.insert(0, "~");
		}
		else den = den.substr(1, den.find(")"));
		q.push_back(den);
		start = clock() / CLOCKS_PER_SEC;
		bool result = FOL_engine(q, 0, depth);
		if (result) {
			std::cout << "TRUE : ";
			std::cout << query[i] << "\n";
			result_vec.push_back("TRUE");
		}
		else {
			std::cout << "FALSE : ";
			std::cout << query[i] << "\n";
			result_vec.push_back("FALSE");
		}

	}
	for (auto i = result_vec.begin(); i < result_vec.end(); ++i) std::cout << *i << "\n";
	/*std::vector<std::string> q;
	std::string den;
	den = query[0];
	if (den != "~") {
		den.insert(0, "~");
	}
	q.push_back(den);
	start = clock() / CLOCKS_PER_SEC;
	std::cout << "RESULT:" << FOL_engine(q, 0, depth) << "\n";

	std::cout << "\n" <<"Negative Facts Token" << "\n" << "\n";
	for (auto it = negative_facts.begin(); it != negative_facts.end(); it++) {
		std::cout << it->first << "\n";
	}
	std::cout << "\n" <<"Positive Facts Token" << "\n" << "\n";
	for (auto it = positive_facts.begin(); it != positive_facts.end(); it++) {
		std::cout << it->first << "\n";
	}
	std::cout << "\n" <<"Negative Equations Token" << "\n" << "\n";
	for (auto it = negative_equations.begin(); it != negative_equations.end(); it++) {
		std::cout << it->first << "\n";
	}
	std::cout << "\n" << "Positive Equations Token" << "\n" << "\n";
	for (auto it = positive_equations.begin(); it != positive_equations.end(); it++) {
		std::cout << it->first << "\n";
	}

	// Printing Negative Facts as Key-Value Pair
	std::cout << "Printing Negative Facts as Key-Value Pair\n";
	for (auto it = negative_facts.begin(); it != negative_facts.end(); it++) {
		std::cout << it->first << " : ";
		for (auto it2 = it->second.begin(); it2 != it->second.end(); ++it2) {
			std::cout << *it2 << " ";
		}
		std::cout << "\n";
	}

	// Printing Positive Facts as Key-Value Pair
	std::cout << "\n";
	for (auto it = positive_facts.begin(); it != positive_facts.end(); it++) {
		std::cout << it->first << " : ";
		for (auto it2 = it->second.begin(); it2 != it->second.end(); ++it2) {
			std::cout << *it2 << " ";
		}
		std::cout << "\n";
	}*/
}