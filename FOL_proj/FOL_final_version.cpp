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
			std::string name = query[i].substr(1, query[i].find(")"));
			for (int j = 0; j < equation.size(); j++) {
				if (name == equation[j]) {
					match = 1;
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
			std::string name = query[i].substr(0, query[i].find("("));
			for (int j = 0; j < equation.size(); j++) {
				if (name == equation[j].substr(1, equation[j].find("(") - 1)) {
					match = 1;
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
	if (depth <= 0 || (end - start) > 10) {
		return false;
	}
	bool result = false;
	if (query.empty()) {
		return true;
	}
	else {
		std::string clause = query[0];
		if (clause[0] == '~') {
			// Check in Positive Facts
			int e, f, g, h;
			std::string token = clause.substr(1, clause.find("(") - 1);
			int cl_arg = std::count(clause.begin(), clause.end(), ',') + 1;
			if (positive_facts.count(token) > 0) {
				for (int j = 0; j < positive_facts[token].size(); j++) {
					if (cl_arg == 1) {
						e = clause.find("(");
						f = clause.find(")");
						g = positive_facts[token][j].find("(");
						h = positive_facts[token][j].find(")");
						if (clause.substr(e + 1, f - e - 1) == positive_facts[token][j].substr(g + 1, h - g - 1) && (isupper(clause.substr(e + 1, f - e - 1)[0]) && isupper(positive_facts[token][j].substr(g + 1, h - g - 1)[0]))) {
							std::vector<std::string> query2;
							int k = i + 1;
							while (k < query.size()) {
								query2.push_back(query[k]);
								k++;
							}
							result = FOL_engine(query2, 0, --depth);
							if (result) return true;
						}
						else if (islower(clause.substr(e + 1, f - e - 1)[0]) && isupper(positive_facts[token][j].substr(g + 1, h - g - 1)[0])) {
							std::vector<std::string> query2 = Unify(query, clause.substr(e + 1, f - e - 1), positive_facts[token][j].substr(g + 1, h - g - 1));
							std::vector<std::string> query3;
							int k = 1;
							while (k < query2.size()) {
								query3.push_back(query2[k]);
								k++;
							}
							result = FOL_engine(query3, 0, --depth);
							if (result) return true;
						}
						else if (islower(positive_facts[token][j].substr(g + 1, h - g - 1)[0])) {
							std::vector<std::string> query2;
							int k = 1;
							while (k < query.size()) {
								query2.push_back(query[k]);
								k++;
							}
							result = FOL_engine(query2, 0, --depth);
							if (result) return true;

						}
					}
					if (cl_arg > 1) {
						int match = 0;
						std::vector<std::string> query2 = query;
						std::string pf_str = positive_facts[token][j];
						// Case - I For first argument Eg: Play(x, _ , _ ) Play(Come, Abhi, OP)
						e = query2[i].find("(");
						f = query2[i].find(",");
						g = pf_str.find("(");
						h = pf_str.find(",");

						if (query2[i].substr(e + 1, f - e - 1) == pf_str.substr(g + 1, h - g - 1) && (isupper(query2[i].substr(e + 1, f - e - 1)[0]) && isupper(pf_str.substr(g + 1, h - g - 1)[0]))) {
							match++;
						}
						else if (islower(query2[i].substr(e + 1, f - e - 1)[0]) && isupper(pf_str.substr(g + 1, h - g - 1)[0])) {
							query2 = Unify(query2, query2[i].substr(e + 1, f - e - 1), pf_str.substr(g + 1, h - g - 1));
							match++;
						}
						else if (isupper(query2[i].substr(e + 1, f - e - 1)[0]) && islower(pf_str.substr(g + 1, h - g - 1)[0])) {
							pf_str = Unification(pf_str, pf_str.substr(g + 1, h - g - 1), query2[i].substr(e + 1, f - e - 1));
							match++;
						}
						else if (islower(query2[i].substr(e + 1, f - e - 1)[0]) && islower(pf_str.substr(g + 1, h - g - 1)[0])) {
							pf_str = Unification(pf_str, pf_str.substr(g + 1, h - g - 1), query2[i].substr(e + 1, f - e - 1));
							match++;
						}
						// Case - II For middle arguments Eg: Play(_, x, y, w, _ ) Leaving first and last arguments
						for (int mid_arg = 0; mid_arg < cl_arg - 2; mid_arg++) {
							e = query2[i].find(",", e + 1);
							f = query2[i].find(",", e + 1);
							g = pf_str.find(",", g + 1);
							h = pf_str.find(",", g + 1);

							if (query2[i].substr(e + 1, f - e - 1) == pf_str.substr(g + 1, h - g - 1) && (isupper(query2[i].substr(e + 1, f - e - 1)[0]) && isupper(pf_str.substr(g + 1, h - g - 1)[0]))) {
								match++;
							}
							else if (islower(query2[i].substr(e + 1, f - e - 1)[0]) && isupper(pf_str.substr(g + 1, h - g - 1)[0])) {
								query2 = Unify(query2, query2[i].substr(e + 1, f - e - 1), pf_str.substr(g + 1, h - g - 1));
								match++;
							}
							else if (isupper(query2[i].substr(e + 1, f - e - 1)[0]) && islower(pf_str.substr(g + 1, h - g - 1)[0])) {
								pf_str = Unification(pf_str, pf_str.substr(g + 1, h - g - 1), query2[i].substr(e + 1, f - e - 1));
								match++;
							}
							else if (islower(query2[i].substr(e + 1, f - e - 1)[0]) && islower(pf_str.substr(g + 1, h - g - 1)[0])) {
								pf_str = Unification(pf_str, pf_str.substr(g + 1, h - g - 1), query2[i].substr(e + 1, f - e - 1));
								match++;
							}
						}

						// Case - III For last argument Eg: Play(_, _, y)
						e = query2[i].find(",", e + 1);
						f = query2[i].find(")");
						g = pf_str.find(",", g + 1);
						h = pf_str.find(")");
						if (query2[i].substr(e + 1, f - e - 1) == pf_str.substr(g + 1, h - g - 1) && (isupper(query2[i].substr(e + 1, f - e - 1)[0]) && isupper(pf_str.substr(g + 1, h - g - 1)[0]))) {
							match++;
						}
						else if (islower(query2[i].substr(e + 1, f - e - 1)[0]) && isupper(pf_str.substr(g + 1, h - g - 1)[0])) {
							query2 = Unify(query2, query2[i].substr(e + 1, f - e - 1), pf_str.substr(g + 1, h - g - 1));
							match++;
						}
						else if (isupper(query2[i].substr(e + 1, f - e - 1)[0]) && islower(pf_str.substr(g + 1, h - g - 1)[0])) {
							pf_str = Unification(pf_str, pf_str.substr(g + 1, h - g - 1), query2[i].substr(e + 1, f - e - 1));
							match++;
						}
						else if (islower(query2[i].substr(e + 1, f - e - 1)[0]) && islower(pf_str.substr(g + 1, h - g - 1)[0])) {
							pf_str = Unification(pf_str, pf_str.substr(g + 1, h - g - 1), query2[i].substr(e + 1, f - e - 1));
							match++;
						}
						int p = query2[i].find("(");
						int q = query2[i].find(")");
						int r = pf_str.find("(");
						int s = pf_str.find(")");

						if (cl_arg == match) {
							int flag = 0;
							std::vector<std::string> query3;
							if (query[i].substr(p + 1, q - p - 1) == pf_str.substr(r + 1, s - r - 1)) {
								int k = 1;
								while (k < query2.size()) {
									query3.push_back(query2[k]);
									k++;
								}
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
											query2 = Unify(query2, query2[i].substr(e + 1, f - e - 1), pf_str.substr(g + 1, h - g - 1));
										}
										else if (isupper(query2[i].substr(e + 1, f - e - 1)[0]) && islower(pf_str.substr(g + 1, h - g - 1)[0])) {
											pf_str = Unification(pf_str, pf_str.substr(g + 1, h - g - 1), query2[i].substr(e + 1, f - e - 1));
										}
										else if (islower(query2[i].substr(e + 1, f - e - 1)[0]) && islower(pf_str.substr(g + 1, h - g - 1)[0])) {
											query2 = Unify(query2, query2[i].substr(e + 1, f - e - 1), pf_str.substr(g + 1, h - g - 1));
										}
										// Case - II For middle arguments Eg: Play(_, x, y, w, _ ) Leaving first and last arguments
										for (int mid_arg = 0; mid_arg < cl_arg - 2; mid_arg++) {
											e = query2[i].find(",", e + 1);
											f = query2[i].find(",", e + 1);
											g = pf_str.find(",", g + 1);
											h = pf_str.find(",", g + 1);

											if (islower(query2[i].substr(e + 1, f - e - 1)[0]) && isupper(pf_str.substr(g + 1, h - g - 1)[0])) {
												query2 = Unify(query2, query2[i].substr(e + 1, f - e - 1), pf_str.substr(g + 1, h - g - 1));
											}
											else if (isupper(query2[i].substr(e + 1, f - e - 1)[0]) && islower(pf_str.substr(g + 1, h - g - 1)[0])) {
												pf_str = Unification(pf_str, pf_str.substr(g + 1, h - g - 1), query2[i].substr(e + 1, f - e - 1));
											}
											else if (islower(query2[i].substr(e + 1, f - e - 1)[0]) && islower(pf_str.substr(g + 1, h - g - 1)[0])) {
												query2 = Unify(query2, query2[i].substr(e + 1, f - e - 1), pf_str.substr(g + 1, h - g - 1));
											}
										}

										// Case - III For last argument Eg: Play(_, _, y)
										e = query2[i].find(",", e + 1);
										f = query2[i].find(")");
										g = pf_str.find(",", g + 1);
										h = pf_str.find(")");
										if (islower(query2[i].substr(e + 1, f - e - 1)[0]) && isupper(pf_str.substr(g + 1, h - g - 1)[0])) {
											query2 = Unify(query2, query2[i].substr(e + 1, f - e - 1), pf_str.substr(g + 1, h - g - 1));
										}
										else if (isupper(query2[i].substr(e + 1, f - e - 1)[0]) && islower(pf_str.substr(g + 1, h - g - 1)[0])) {
											pf_str = Unification(pf_str, pf_str.substr(g + 1, h - g - 1), query2[i].substr(e + 1, f - e - 1));
										}
										else if (islower(query2[i].substr(e + 1, f - e - 1)[0]) && islower(pf_str.substr(g + 1, h - g - 1)[0])) {
											query2 = Unify(query2, query2[i].substr(e + 1, f - e - 1), pf_str.substr(g + 1, h - g - 1));
										}
										flag = 1;
									}
									else if (flag == 1) {
										e = query2[i].find("(");
										f = query2[i].find(",");
										g = pf_str.find("(");
										h = pf_str.find(",");
										if (islower(query2[i].substr(e + 1, f - e - 1)[0]) && isupper(pf_str.substr(g + 1, h - g - 1)[0])) {
											query2 = Unify(query2, query2[i].substr(e + 1, f - e - 1), pf_str.substr(g + 1, h - g - 1));
										}
										else if (isupper(query2[i].substr(e + 1, f - e - 1)[0]) && islower(pf_str.substr(g + 1, h - g - 1)[0])) {
											pf_str = Unification(pf_str, pf_str.substr(g + 1, h - g - 1), query2[i].substr(e + 1, f - e - 1));
										}
										else if (islower(query2[i].substr(e + 1, f - e - 1)[0]) && islower(pf_str.substr(g + 1, h - g - 1)[0])) {
											pf_str = Unification(pf_str, pf_str.substr(g + 1, h - g - 1), query2[i].substr(e + 1, f - e - 1));
										}
										for (int mid_arg = 0; mid_arg < cl_arg - 2; mid_arg++) {
											e = query2[i].find(",", e + 1);
											f = query2[i].find(",", e + 1);
											g = pf_str.find(",", g + 1);
											h = pf_str.find(",", h + 1);

											if (islower(query2[i].substr(e + 1, f - e - 1)[0]) && isupper(pf_str.substr(g + 1, h - g - 1)[0])) {
												query2 = Unify(query2, query2[i].substr(e + 1, f - e - 1), pf_str.substr(g + 1, h - g - 1));
												for (auto i = query2.begin(); i < query2.end(); ++i) std::cout << *i << "\n";
											}
											else if (isupper(query2[i].substr(e + 1, f - e - 1)[0]) && islower(pf_str.substr(g + 1, h - g - 1)[0])) {
												pf_str = Unification(pf_str, pf_str.substr(g + 1, h - g - 1), query2[i].substr(e + 1, f - e - 1));
											}
											else if (islower(query2[i].substr(e + 1, f - e - 1)[0]) && islower(pf_str.substr(g + 1, h - g - 1)[0])) {
												pf_str = Unification(pf_str, pf_str.substr(g + 1, h - g - 1), query2[i].substr(e + 1, f - e - 1));
											}
										}

										// Case - III For last argument Eg: Play(_, _, y)
										e = query2[i].find(",", e + 1);
										f = query2[i].find(")");
										g = pf_str.find(",", g + 1);
										h = pf_str.find(")");
										if (islower(query2[i].substr(e + 1, f - e - 1)[0]) && isupper(pf_str.substr(g + 1, h - g - 1)[0])) {
											query2 = Unify(query2, query2[i].substr(e + 1, f - e - 1), pf_str.substr(g + 1, h - g - 1));
										}
										else if (isupper(query2[i].substr(e + 1, f - e - 1)[0]) && islower(pf_str.substr(g + 1, h - g - 1)[0])) {
											pf_str = Unification(pf_str, pf_str.substr(g + 1, h - g - 1), query2[i].substr(e + 1, f - e - 1));
										}
										else if (islower(query2[i].substr(e + 1, f - e - 1)[0]) && islower(pf_str.substr(g + 1, h - g - 1)[0])) {
											pf_str = Unification(pf_str, pf_str.substr(g + 1, h - g - 1), query2[i].substr(e + 1, f - e - 1));
										}
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
								result = FOL_engine(query3, 0, --depth);
								if (result) return true;
							}
						}
					}
				}
			}

			// Now Check in Positive Equations 2D Vector String
			if (positive_equations.count(token) > 0) {
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
								e = query[i].find("(");
								f = query[i].find(")");
								g = all_tok_eq[k].find("(");
								h = all_tok_eq[k].find(")");
								if ((query[i].substr(e + 1, f - e - 1) == all_tok_eq[k].substr(g + 1, h - g - 1)) && isupper(query[i].substr(e + 1, f - e - 1)[0]) && isupper(all_tok_eq[k].substr(g + 1, h - g - 1)[0])) {
									int d = i + 1;
									std::vector<std::string> equation = all_tok_eq;
									query2 = Resolution(query, equation);
									result = FOL_engine(query2, 0, --depth);
									if (result) return true;
								}
								else if (islower(query[i].substr(e + 1, f - e - 1)[0]) && isupper(all_tok_eq[k].substr(g + 1, h - g - 1)[0])) {
									std::vector<std::string> query3 = Unify(query, query[i].substr(e + 1, f - e - 1), all_tok_eq[k].substr(g + 1, h - g - 1));
									std::vector<std::string> equation = all_tok_eq;
									query2 = Resolution(query3, equation);
									result = FOL_engine(query2, 0, --depth);
									if (result) return true;

								}
								else if (islower(all_tok_eq[k].substr(g + 1, h - g - 1)[0])) {
									all_tok_eq = Unify(all_tok_eq, all_tok_eq[k].substr(g + 1, h - g - 1), query[0].substr(e + 1, f - e - 1));
									std::vector<std::string> equation = all_tok_eq;
									query2 = Resolution(query, equation);
									result = FOL_engine(query2, 0, --depth);
									if (result) return true;
								}
							}
							else if (cl_arg > 1) {
								int match = 0;
								std::vector<std::string> query3 = query;
								// Case - I For the first argument
								e = query3[i].find("(");
								f = query3[i].find(",");
								g = all_tok_eq[k].find("(");
								h = all_tok_eq[k].find(",");
								if ((query3[i].substr(e + 1, f - e - 1) == all_tok_eq[k].substr(g + 1, h - g - 1)) && isupper(query3[i].substr(e + 1, f - e - 1)[0]) && isupper(all_tok_eq[k].substr(g + 1, h - g - 1)[0])) {
									match++;
								}
								else if (islower(query3[i].substr(e + 1, f - e - 1)[0]) && isupper(all_tok_eq[k].substr(g + 1, h - g - 1)[0])) {
									query3 = Unify(query3, query3[i].substr(e + 1, f - e - 1), all_tok_eq[k].substr(g + 1, h - g - 1));
									match++;
								}
								else if (islower(all_tok_eq[k].substr(g + 1, h - g - 1)[0])) {
									all_tok_eq = Unify(all_tok_eq, all_tok_eq[k].substr(g + 1, h - g - 1), query3[i].substr(e + 1, f - e - 1));
									match++;
								}
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
										match++;
									}
								}
								e = query3[i].find(",", e + 1);
								f = query3[i].find(")");
								g = all_tok_eq[k].find(",", g + 1);
								h = all_tok_eq[k].find(")");

								if ((query3[i].substr(e + 1, f - e - 1) == all_tok_eq[k].substr(g + 1, h - g - 1)) && isupper(query3[i].substr(e + 1, f - e - 1)[0]) && isupper(all_tok_eq[k].substr(g + 1, h - g - 1)[0])) {
									match++;
								}
								else if (islower(query3[i].substr(e + 1, f - e - 1)[0]) && isupper(all_tok_eq[k].substr(g + 1, h - g - 1)[0])) {
									query3 = Unify(query3, query3[i].substr(e + 1, f - e - 1), all_tok_eq[k].substr(g + 1, h - g - 1));
									match++;
								}
								else if (islower(all_tok_eq[k].substr(g + 1, h - g - 1)[0])) {
									all_tok_eq = Unify(all_tok_eq, all_tok_eq[k].substr(g + 1, h - g - 1), query3[i].substr(e + 1, f - e - 1));
									match++;
								}
								int p = query3[i].find("(");
								int q = query3[i].find(")");
								int r = all_tok_eq[k].find("(");
								int s = all_tok_eq[k].find(")");
								if (cl_arg == match) {
									int flag = 0;
									if (query3[i].substr(p + 1, q - p - 1) == all_tok_eq[k].substr(r + 1, s - r - 1)) {
										std::vector<std::string> equation = all_tok_eq;
										query3 = Resolution(query3, equation);
										query2 = query3;
										result = FOL_engine(query2, 0, --depth);
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
										std::vector<std::string> equation = all_tok_eq;
										query3 = Resolution(query3, equation);
										query2 = query3;
										result = FOL_engine(query2, 0, --depth);
										if (result) return true;
									}
								}
							}
						}
					}
				}
			}
		}
		else {
			// Check in Negative Facts
			int e, f, g, h;
			std::string token = clause.substr(0, clause.find("("));
			int cl_arg = std::count(clause.begin(), clause.end(), ',') + 1;
			if (negative_facts.count(token) > 0) {
				for (int j = 0; j < negative_facts[token].size(); j++) {
					if (cl_arg == 1) {
						e = clause.find("(");
						f = clause.find(")");
						g = negative_facts[token][j].find("(");
						h = negative_facts[token][j].find(")");
						if (clause.substr(e + 1, f - e - 1) == negative_facts[token][j].substr(g + 1, h - g - 1) && (isupper(clause.substr(e + 1, f - e - 1)[0]) && isupper(negative_facts[token][j].substr(g + 1, h - g - 1)[0]))) {
							std::vector<std::string> query2;
							int k = 1;
							while (k < query.size()) {
								query2.push_back(query[k]);
								k++;
							}
							result = FOL_engine(query2, 0, ++depth);
							if (result) return true;
						}
						else if (islower(clause.substr(e + 1, f - e - 1)[0]) && isupper(negative_facts[token][j].substr(g + 1, h - g - 1)[0])) {
							std::vector<std::string> query2 = Unify(query, clause.substr(e + 1, f - e - 1), negative_facts[token][j].substr(g + 1, h - g - 1));
							std::vector<std::string> query3;
							int k = 1;
							while (k < query2.size()) {
								query3.push_back(query2[k]);
								k++;
							}
							result = FOL_engine(query3, 0, --depth);
							if (result) return true;
						}
						else if (islower(negative_facts[token][j].substr(g + 1, h - g - 1)[0])) {
							std::vector<std::string> query2;
							int k = 1;
							while (k < query.size()) {
								query2.push_back(query[k]);
								k++;
							}
							result = FOL_engine(query2, 0, ++depth);
							if (result) return true;
						}
					}
					if (cl_arg > 1) {
						int match = 0;
						std::vector<std::string> query2 = query;
						std::string nf_str = negative_facts[token][j];
						// Case - I For first argument Eg: Play(x, _ , _ ) Play(Come, Abhi, OP)
						e = query2[i].find("(");
						f = query2[i].find(",");
						g = nf_str.find("(");
						h = nf_str.find(",");

						if (query2[i].substr(e + 1, f - e - 1) == nf_str.substr(g + 1, h - g - 1) && (isupper(query2[i].substr(e + 1, f - e - 1)[0]) && isupper(nf_str.substr(g + 1, h - g - 1)[0]))) {
							match++;
						}
						else if (islower(query2[i].substr(e + 1, f - e - 1)[0]) && isupper(nf_str.substr(g + 1, h - g - 1)[0])) {
							query2 = Unify(query2, query2[i].substr(e + 1, f - e - 1), nf_str.substr(g + 1, h - g - 1));
							match++;
						}
						else if (isupper(query2[i].substr(e + 1, f - e - 1)[0]) && islower(nf_str.substr(g + 1, h - g - 1)[0])) {
							nf_str = Unification(nf_str, nf_str.substr(g + 1, h - g - 1), query2[i].substr(e + 1, f - e - 1));
							match++;
						}
						else if (islower(query2[i].substr(e + 1, f - e - 1)[0]) && islower(nf_str.substr(g + 1, h - g - 1)[0])) {
							nf_str = Unification(nf_str, nf_str.substr(g + 1, h - g - 1), query2[i].substr(e + 1, f - e - 1));
							match++;
						}
						// Case - II For middle arguments Eg: Play(_, x, y, w, _ ) Leaving first and last arguments
						for (int mid_arg = 0; mid_arg < cl_arg - 2; mid_arg++) {
							e = query2[i].find(",", e + 1);
							f = query2[i].find(",", e + 1);
							g = nf_str.find(",", g + 1);
							h = nf_str.find(",", g + 1);

							if (query2[i].substr(e + 1, f - e - 1) == nf_str.substr(g + 1, h - g - 1) && (isupper(query2[i].substr(e + 1, f - e - 1)[0]) && isupper(nf_str.substr(g + 1, h - g - 1)[0]))) {
								match++;
							}
							else if (islower(query2[i].substr(e + 1, f - e - 1)[0]) && isupper(nf_str.substr(g + 1, h - g - 1)[0])) {
								query2 = Unify(query2, query2[i].substr(e + 1, f - e - 1), nf_str.substr(g + 1, h - g - 1));
								match++;
							}
							else if (isupper(query2[i].substr(e + 1, f - e - 1)[0]) && islower(nf_str.substr(g + 1, h - g - 1)[0])) {
								nf_str = Unification(nf_str, nf_str.substr(g + 1, h - g - 1), query2[i].substr(e + 1, f - e - 1));
								match++;
							}
							else if (islower(query2[i].substr(e + 1, f - e - 1)[0]) && islower(nf_str.substr(g + 1, h - g - 1)[0])) {
								nf_str = Unification(nf_str, nf_str.substr(g + 1, h - g - 1), query2[i].substr(e + 1, f - e - 1));
								match++;
							}
						}

						// Case - III For last argument Eg: Play(_, _, y)
						e = query2[i].find(",", e + 1);
						f = query2[i].find(")");
						g = nf_str.find(",", g + 1);
						h = nf_str.find(")");
						if (query2[i].substr(e + 1, f - e - 1) == nf_str.substr(g + 1, h - g - 1) && (isupper(query2[i].substr(e + 1, f - e - 1)[0]) && isupper(nf_str.substr(g + 1, h - g - 1)[0]))) {
							match++;
						}
						else if (islower(query2[i].substr(e + 1, f - e - 1)[0]) && isupper(nf_str.substr(g + 1, h - g - 1)[0])) {
							query2 = Unify(query2, query2[i].substr(e + 1, f - e - 1), nf_str.substr(g + 1, h - g - 1));
							match++;
						}
						else if (isupper(query2[i].substr(e + 1, f - e - 1)[0]) && islower(nf_str.substr(g + 1, h - g - 1)[0])) {
							nf_str = Unification(nf_str, nf_str.substr(g + 1, h - g - 1), query2[i].substr(e + 1, f - e - 1));
							match++;
						}
						else if (islower(query2[i].substr(e + 1, f - e - 1)[0]) && islower(nf_str.substr(g + 1, h - g - 1)[0])) {
							nf_str = Unification(nf_str, nf_str.substr(g + 1, h - g - 1), query2[i].substr(e + 1, f - e - 1));
							match++;
						}

						int p = query2[i].find("(");
						int q = query2[i].find(")");
						int r = nf_str.find("(");
						int s = nf_str.find(")");

						if (cl_arg == match) {
							std::vector<std::string> query3;
							int flag = 0;
							if (query2[i].substr(p + 1, q - p - 1) == nf_str.substr(r + 1, s - r - 1)) {
								int k = 1;
								while (k < query2.size()) {
									query3.push_back(query2[k]);
									k++;
								}
								result = FOL_engine(query3, 0, --depth);
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
								result = FOL_engine(query3, 0, --depth);
								if (result) return true;
							}
						}
					}
				}
			}
			// Check for Negative Equations
			if (negative_equations.count(token) > 0) {
				std::string eq_token;
				std::vector<std::string> all_tok_eq;  // Storing all the equations for respective tokens
				std::vector<std::string> query2;
				for (int j = 0; j < negative_equations[token].size(); j++) {
					for (int k = 0; k < negative_equations[token][j].size(); k++) {
						eq_token = negative_equations[token][j][k].substr(1, negative_equations[token][j][k].find("(") - 1);
						if (token == eq_token) {
							all_tok_eq = negative_equations[token][j];

							// For single arguments
							if (cl_arg == 1) {
								e = query[i].find("(");
								f = query[i].find(")");
								g = all_tok_eq[k].find("(");
								h = all_tok_eq[k].find(")");
								if ((query[i].substr(e + 1, f - e - 1) == all_tok_eq[k].substr(g + 1, h - g - 1)) && isupper(query[i].substr(e + 1, f - e - 1)[0]) && isupper(all_tok_eq[k].substr(g + 1, h - g - 1)[0])) {
									std::vector<std::string> equation = all_tok_eq;
									query2 = Resolution(query, equation);
									result = FOL_engine(query2, 0, ++depth);
									if (result) return true;
								}
								else if (islower(query[i].substr(e + 1, f - e - 1)[0]) && isupper(all_tok_eq[k].substr(g + 1, h - g - 1)[0])) {
									std::vector<std::string> query3 = Unify(query, query[i].substr(e + 1, f - e - 1), all_tok_eq[k].substr(g + 1, h - g - 1));
									std::vector<std::string> equation = all_tok_eq;
									query2 = Resolution(query3, equation);
									result = FOL_engine(query2, 0, ++depth);
									if (result) return true;

								}
								else if (islower(all_tok_eq[k].substr(g + 1, h - g - 1)[0])) {
									all_tok_eq = Unify(all_tok_eq, all_tok_eq[k].substr(g + 1, h - g - 1), query[i].substr(e + 1, f - e - 1));
									std::vector<std::string> equation = all_tok_eq;
									query2 = Resolution(query, equation);
									result = FOL_engine(query2, 0, --depth);
									if (result) return true;
								}
							}
							else if (cl_arg > 1) {
								int match = 0;
								std::vector<std::string> query3 = query;

								// Case - I For the first argument

								e = query3[i].find("(");
								f = query3[i].find(",");
								g = all_tok_eq[k].find("(");
								h = all_tok_eq[k].find(",");
								if ((query3[i].substr(e + 1, f - e - 1) == all_tok_eq[k].substr(g + 1, h - g - 1)) && isupper(query3[i].substr(e + 1, f - e - 1)[0]) && isupper(all_tok_eq[k].substr(g + 1, h - g - 1)[0])) {
									match++;
								}
								else if (islower(query3[i].substr(e + 1, f - e - 1)[0]) && isupper(all_tok_eq[k].substr(g + 1, h - g - 1)[0])) {
									query3 = Unify(query3, query3[i].substr(e + 1, f - e - 1), all_tok_eq[k].substr(g + 1, h - g - 1));
									match++;
								}
								else if (islower(all_tok_eq[k].substr(g + 1, h - g - 1)[0])) {
									all_tok_eq = Unify(all_tok_eq, all_tok_eq[k].substr(g + 1, h - g - 1), query3[i].substr(e + 1, f - e - 1));
									match++;
								}
								// Case - II For mid arguments (Except First and last arguments)

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
										match++;
									}
								}

								// Case - III For Last arguments
								e = query3[i].find(",", e + 1);
								f = query3[i].find(")");
								g = all_tok_eq[k].find(",", g + 1);
								h = all_tok_eq[k].find(")");

								if ((query3[i].substr(e + 1, f - e - 1) == all_tok_eq[k].substr(g + 1, h - g - 1)) && isupper(query3[i].substr(e + 1, f - e - 1)[0]) && isupper(all_tok_eq[k].substr(g + 1, h - g - 1)[0])) {
									match++;
								}
								else if (islower(query3[i].substr(e + 1, f - e - 1)[0]) && isupper(all_tok_eq[k].substr(g + 1, h - g - 1)[0])) {
									query3 = Unify(query3, query3[i].substr(e + 1, f - e - 1), all_tok_eq[k].substr(g + 1, h - g - 1));
									match++;
								}
								else if (islower(all_tok_eq[k].substr(g + 1, h - g - 1)[0])) {
									all_tok_eq = Unify(all_tok_eq, all_tok_eq[k].substr(g + 1, h - g - 1), query3[i].substr(e + 1, f - e - 1));
									match++;
								}

								int p = query3[i].find("(");
								int q = query3[i].find(")");
								int r = all_tok_eq[k].find("(");
								int s = all_tok_eq[k].find(")");

								if (cl_arg == match) {
									int flag = 0;
									if (query3[i].substr(p + 1, q - p - 1) == all_tok_eq[k].substr(r + 1, s - r - 1)) {
										std::vector<std::string> equation = all_tok_eq;
										query2 = Resolution(query3, equation);
										result = FOL_engine(query2, 0, --depth);
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
										std::vector<std::string> equation = all_tok_eq;
										query2 = Resolution(query3, equation);
										result = FOL_engine(query2, 0, --depth);
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
	std::ifstream myfile("input.txt");
	std::ofstream output_file;
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
	std::vector<std::string> temp_KB_string;
	for (int i = 0; i < nkb; i++) {
		std::size_t found = KB[i].find("=>");
		if (found != std::string::npos) {
			std::stringstream ss(KB[i]);
			std::string word;
			int implies = 0;
			std::vector<std::string> temp_KB_string;
			while (ss >> word) {
				if (implies == 0) {
					if (word[0] == '~') {
						word.erase(word.begin());
						temp_KB_string.push_back(word);
					}
					else if (word == "=>") implies = 1;
					else if (word != "&" && word != "=>") {
						word.insert(0, "~");
						temp_KB_string.push_back(word);
					}
				}
				else {
					temp_KB_string.push_back(word);
				}
			}
			KB_matrix.push_back(temp_KB_string);
		}
		else {
			KB_facts_matrix.push_back(KB[i]);
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
					f.push_back(KB_facts_matrix[i]);
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
					f.push_back(KB_facts_matrix[i]);
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
						f.push_back(KB_facts_matrix[i]);
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
			a = KB_facts_matrix[i].find(",", a + 1);
			b = KB_facts_matrix[i].find(")");
			if (islower(KB_facts_matrix[i].substr(a + 1, b - a - 1)[0])) {
				if (standard_variables_f.count(KB_facts_matrix[i].substr(a + 1, b - a - 1)) == 0) {
					standard_variables_f[KB_facts_matrix[i].substr(a + 1, b - a - 1)] = i;
				}
				else if (standard_variables_f.count(KB_facts_matrix[i].substr(a + 1, b - a - 1)) > 0 && standard_variables_f[KB_facts_matrix[i].substr(a + 1, b - a - 1)] != i) {
					std::string str;
					std::vector<std::string> f;
					f.push_back(KB_facts_matrix[i]);
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
	int depth = 200;
	output_file.open("output.txt");
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
			output_file << "TRUE\n";
		}
		else {
			output_file << "FALSE\n";
		}

	}
	output_file.close();
}