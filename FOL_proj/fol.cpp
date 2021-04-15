#include <iostream> 
#include <fstream> 
#include <string>
#include <sstream>
#include <vector>
#include <map>
#include <stack>

std::map<std::string, std::vector<std::string>> negative_facts;
std::map<std::string, std::vector<std::string>> positive_facts;
std::map<std::string, std::vector<std::vector<std::string>>> negative_equations;
std::map<std::string, std::vector<std::vector<std::string>>> positive_equations;

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

bool FOL_engine(std::vector<std::string> query, int i) { // i represent no of queries left - 1
	bool result = false;
	if (query.empty()) {
		return true;
	}
	std::string clause = query[0];
	std::cout << "CLAUSE: " << clause << "\n";
	if (clause[0] == '~') {
		// Check in Positive Facts
		int e, f, g, h;
		std::string token = clause.substr(1, clause.find("(") - 1);
		int cl_arg = std::count(clause.begin(), clause.end(), ',') + 1;
		//std::cout << positive_facts[token].size() << "\n";
		if (positive_facts.count(token) > 0) {
			for (int j = 0; j < positive_facts[token].size(); j++) {
				if (cl_arg == 1) {
					std::cout << "YES" << "\n";
					e = clause.find("(");
					f = clause.find(")");
					g = positive_facts[token][j].find("(");
					h = positive_facts[token][j].find(")");
					if (clause.substr(e + 1, f - e - 1) == positive_facts[token][j].substr(g + 1, h - g - 1)) {
						std::vector<std::string> query2;
						int k = i + 1;
						while (k < query.size()) {
							query2.push_back(query[k]);
							k++;
						}
						result = FOL_engine(query2, query2.size() - 1);
						std::cout << "Contents of Query2 \n";
						for (auto i = query2.begin(); i != query2.end(); ++i) std::cout << *i << "\n";
						if (result) return true;
					}
					else if (islower(clause.substr(e + 1, f - e - 1)[0])) {
						std::cout << "YES\n";
						std::vector<std::string> query2 = Unify(query, clause.substr(e + 1, f - e - 1), positive_facts[token][j].substr(g + 1, h - g - 1));
						std::vector<std::string> query3;
						int k = i + 1;
						std::cout << "Contents of Query2 \n";
						for (auto i = query2.begin(); i != query2.end(); ++i) std::cout << *i << "\n";
						while (k < query2.size()) {
							query3.push_back(query2[k]);
							k++;
						}
						std::cout << "Contents of Query3 \n";
						for (auto i = query3.begin(); i != query3.end(); ++i) std::cout << *i << "\n";
						result = FOL_engine(query3, query3.size() - 1);
						if (result) return true;
					}
				}
				if (cl_arg > 0) {
					int match = 0;
					std::vector<std::string> query2 = query;
					// Case - I For first argument Eg: Play(x, _ , _ ) Play(Come, Abhi, OP)
					e = query2[i].find("(");
					f = query2[i].find(",");
					g = positive_facts[token][j].find("(");
					h = positive_facts[token][j].find(",");

					if (query2[i].substr(e + 1, f - e - 1) == positive_facts[token][j].substr(g + 1, h - g - 1)) match++;
					else if (islower(clause.substr(e + 1, f - e - 1)[0])) {
						std::cout << "HELLO\n";
						query2 = Unify(query2, query2[i].substr(e + 1, f - e - 1), positive_facts[token][j].substr(g + 1, h - g - 1));
						match++;
					}
					std::cout << "Contents of Query2 when arg is greater 1 when CASE - 1  \n";
					for (auto i = query2.begin(); i != query2.end(); ++i) std::cout << *i << "\n";
					// Case - II For middle arguments Eg: Play(_, x, y, w, _ ) Leaving first and last arguments
					for (int mid_arg = 0; mid_arg < cl_arg - 2; mid_arg++) {
						e = query2[i].find(",", e + 1);
						f = query2[i].find(",", e + 1);
						g = positive_facts[token][j].find(",", g + 1);
						h = positive_facts[token][j].find(",", h + 1);

						if (query2[i].substr(e + 1, f - e - 1) == positive_facts[token][j].substr(g + 1, h - g - 1)) match++;
						else if (islower(query2[i].substr(e + 1, f - e - 1)[0])) {
							query2 = Unify(query2, query2[i].substr(e + 1, f - e - 1), positive_facts[token][j].substr(g + 1, h - g - 1));
							match++;
						}
						std::cout << "Contents of Query2 when arg greater than 1  CASE - 2\n";
						for (auto i = query2.begin(); i != query2.end(); ++i) std::cout << *i << "\n";
					}

					// Case - III For last argument Eg: Play(_, _, y)
					e = query2[i].find(",", e + 1);
					f = query2[i].find(")");
					std::cout << "Value of e: " << e << "\n";
					std::cout << "Value of f: " << f << "\n";
					g = positive_facts[token][j].find(",", g + 1);
					h = positive_facts[token][j].find(")");
					std::cout << "CLAUSE: " << query2[i].substr(e + 1, f - e - 1) << "\n";
					std::cout << "POSITIVE_FACTS: " << positive_facts[token][j].substr(g + 1, h - g - 1) << "\n";
					if (query2[i].substr(e + 1, f - e - 1) == positive_facts[token][j].substr(g + 1, h - g - 1)) match++;
					else if (islower(query2[i].substr(e + 1, f - e - 1)[0])) {
						query2 = Unify(query2, query2[i].substr(e + 1, f - e - 1), positive_facts[token][j].substr(g + 1, h - g - 1));
						match++;
					}
					std::cout << "Contents of Query2 when arg greater than 1 CASE-3 \n";
					for (auto i = query2.begin(); i != query2.end(); ++i) std::cout << *i << "\n";
					if (cl_arg == match) {
						std::vector<std::string> query3;
						int k = i + 1;
						while (k < query2.size()) {
							query3.push_back(query2[k]);
							k++;
						}
						std::cout << "Contents of Query3 when arg greater than 1 case -3 \n";
						for (auto i = query3.begin(); i != query3.end(); ++i) std::cout << *i << "\n";
						result = FOL_engine(query3, query3.size() - 1);
						if (result) return true;
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
								int k = i + 1;
								while (k < query.size()) {
									query2.push_back(query[k]);
									k++;
								}
								for (int left = 0; left < all_tok_eq.size(); left++) {
									if (left != k) query2.push_back(all_tok_eq[left]);
								}
								result = FOL_engine(query2, 0);
								if (result) return true;
							}
							else if (islower(query[i].substr(e + 1, f - e - 1)[0]) && isupper(all_tok_eq[k].substr(g + 1, h - g - 1)[0])) {
								std::vector<std::string> query3 = Unify(query, query[i].substr(e + 1, f - e - 1), all_tok_eq[k].substr(g + 1, h - g - 1));
								int k = i + 1;
								while (k < query3.size()) {
									query2.push_back(query3[k]);
									k++;
								}
								for (int left = 0; left < all_tok_eq.size(); left++) {
									if (left != k) query2.push_back(all_tok_eq[left]);
								}
								result = FOL_engine(query2, 0);
								if (result) return true;

							}
							else if (islower(all_tok_eq[k].substr(g + 1, h - g - 1)[0])) {
								all_tok_eq = Unify(all_tok_eq, all_tok_eq[k].substr(g + 1, h - g - 1), query[i].substr(e + 1, f - e - 1));
								int k = i + 1;
								while (k < query.size()) {
									query2.push_back(query[k]);
									k++;
								}
								for (int left = 0; left < all_tok_eq.size(); left++) {
									if (left != k) query2.push_back(all_tok_eq[left]);
								}
								result = FOL_engine(query2, 0);
								if (result) return true;
							}
						}
						else if (cl_arg > 1) {
							int match = 0;
							std::vector<std::string> query3 = query;

							// Case - I For the first argument
							e = query[i].find("(");
							f = query[i].find(",");
							g = all_tok_eq[k].find("(");
							h = all_tok_eq[k].find(",");
							if ((query3[i].substr(e + 1, f - e - 1) == all_tok_eq[k].substr(g + 1, h - g - 1)) && isupper(query3[i].substr(e + 1, f - e - 1)[0]) && isupper(all_tok_eq[k].substr(g + 1, h - g - 1)[0])) {
								match++;
							}
							else if (islower(query[i].substr(e + 1, f - e - 1)[0]) && isupper(all_tok_eq[k].substr(g + 1, h - g - 1)[0])) {
								query3 = Unify(query3, query3[i].substr(e + 1, f - e - 1), all_tok_eq[k].substr())
							}
						}
					}
				}
			}
		}

	}
		
	
			/*auto it = positive_facts.begin();
			for (it; (it != positive_facts.end() && it->first != clause.substr(1, clause.find("(") - 1)); it++) { // Find the token == clause token
				std::cout << it->first << " : \n";
				std::cout << it->second.size() << "\n";
			}
			std::cout << it->first << " : \n";
			std::cout << it->second.size() << "\n";
			int cl_arguments = std::count(clause.begin(), clause.end(), ',') + 1; // Number of arguments in Clause
			std::cout << "Clause_Arguments_count: " << cl_arguments << "\n";
			std::vector<std::string> param_cl;
			std::string cl_arg_str = clause.substr(clause.find("(") + 1, clause.find(")") - clause.find("(") - 1); // Argument String

			// Storing arguments in a string vector
			std::stringstream ss(cl_arg_str);
			while (ss.good()) {
				std::string word;
				getline(ss, word, ',');
				param_cl.push_back(word);
			}
			// To resolve with each facts it matches with Clause
			for (size_t i = 0; i < it->second.size(); i++) {
				std::vector<std::string> param_pred;
				std::string pred_arg_str = it->second[i].substr(it->second[i].find("(") + 1, it->second[i].find(")") - it->second[i].find("(") - 1);
				std::stringstream ss2(pred_arg_str);
				while (ss2.good()) {
					std::string word;
					getline(ss2, word, ',');
					param_pred.push_back(word);
				}
				for (int i = 0; i < param_pred.size(); i++) {
					std::cout << param_pred[i] << "\n";
				}
				if (param_cl.size() == param_pred.size()) {
					if (param_cl == param_pred) {
						std::vector<std::string> query2 = query;
						query2.erase(query2.begin() + i);
						if (!query2.empty()) {
							result = FOL_engine(query2, positive_facts);
						}
						else {
							return true;
						}
					}
					for (int i = 0; i < param_cl.size(); i++) {
						if (islower(param_cl[i][0])) {

						}
					}
				}

				else if ()
					if (cl_arguments == 0) {
						if (param_cl == param_pred) {
							std::stack<std::string> query2 = query;
							query2.pop();
							//std::cout << "Query2 Size: " << query2.size() << "\n";
							if (!query2.empty()) {
								result = FOL_engine(query2, positive_facts);
							}
							else std::cout << "TRUE \n";
						}
					}
					else if (cl_arguments > 0) {
						if (param_cl == param_pred) {
							std::stack<std::string> query2 = query;
							query2.pop();
							//std::cout << "Query2 Size: " << query2.size() << "\n";
							if (!query2.empty()) {
								result = FOL_engine(query2, positive_facts);
							}
							else std::cout << "True \n";
						}
					}
			}
			for (int i = 0; i < param_cl.size(); i++) {
				std::cout << param_cl[i] << "\n";
			}
		}
		else {
			return result;
			std::cout << "Hello \n";
		}*/
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
	std::getline(myfile, temp_query);
	nq = std::stoi(temp_query);
	for (int i = 0; i < nq; i++) {
		std::getline(myfile, query[i]);
	}
	std::getline(myfile,temp_num_KB);
	nkb = std::stoi(temp_num_KB);
	for (int i = 0; i < nkb; i++) {
		std::getline(myfile, KB[i]);
	}
	// CNF Conversion for KB
	std::vector<std::vector<std::string>> KB_matrix(100, std::vector<std::string> (100, "0"));
	std::vector<std::string> KB_facts_matrix(100, "0");
	int facts_counter = 0;
	//std::string cnf_KB[100];
	for (int i = 0; i < nkb; i++) {
		std::size_t found = KB[i].find("=>");
		if (found!= std::string::npos) {
			std::stringstream ss(KB[i]);
			std::string word;
			int implies = 0;
			int k = 0;
			while (ss >> word) {
				if (implies == 0) {
					if (word[0] == '~') {
						word.erase(word.begin());
						KB_matrix[i][k] = word;
						k++;
					}
					else if (word == "=>") implies = 1;
					else if (word != "&" && word != "=>") {
						word.insert(0, "~");
						KB_matrix[i][k] = word;
						k++;
					}
				}
				else {
					KB_matrix[i][k] = word;
					k++;
				}
			}
		}
		else {
			KB_facts_matrix[facts_counter] = KB[i];
			facts_counter++;
		}
	}
	// Printing CNF Transformed KB Equations
	for (int i = 0; KB_matrix[i][0]!="0"; i++) {
		for (int j = 0; KB_matrix[i][j] != "0"; j++) {
			std::cout << KB_matrix[i][j] << " ";
		}
		std::cout << "\n";
	}
	// Printing KB Facts
	for (int i = 0; KB_facts_matrix[i] != "0"; i++) {
		std::cout << KB_facts_matrix[i] << "\n";
	}
	// Facts Token Initialization
	for (int i = 0; KB_facts_matrix[i] != "0"; i++) {
		if (KB_facts_matrix[i][0] == '~') {
			negative_facts[KB_facts_matrix[i].substr(1, KB_facts_matrix[i].find("(") - 1)];
		}
		else {
			positive_facts[KB_facts_matrix[i].substr(0, KB_facts_matrix[i].find("("))];
		}
	}
	// Negative Facts Value Insertion
	for (auto it = negative_facts.begin(); it != negative_facts.end(); it++) {
		for (int i = 0; KB_facts_matrix[i] != "0"; i++) {
			if (it->first == KB_facts_matrix[i].substr(1, KB_facts_matrix[i].find("(") - 1)) {
				negative_facts[it->first].push_back(KB_facts_matrix[i]);
			}
		}
	}
	// Positive Facts Value Insertion
	for (auto it = positive_facts.begin(); it != positive_facts.end(); it++) {
		for (int i = 0; KB_facts_matrix[i] != "0"; i++) {
			if (it->first == KB_facts_matrix[i].substr(0, KB_facts_matrix[i].find("("))) {
				positive_facts[it->first].push_back(KB_facts_matrix[i]);
			}
		}
	}
	// Equations Token Initialization
	for (int i = 0; KB_matrix[i][0] != "0"; i++) {
		for (int j = 0; KB_matrix[i][j] != "0"; j++) {
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
		for (int i = 0; KB_matrix[i][0] != "0"; i++) {
			int k = 0;
			int match = 0;
			while (KB_matrix[i][k] != "0") {
				if (it->first == KB_matrix[i][k].substr(1, KB_matrix[i][k].find("(") - 1)) {
					match = 1;
					break;
				}
				k++;
			}
			if (match == 1) {
				k = 0;
				std::vector<std::string> eq;
				while (KB_matrix[i][k] != "0") {
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
		for (int i = 0; KB_matrix[i][0] != "0"; i++) {
			int k = 0;
			int match = 0;
			while (KB_matrix[i][k] != "0") {
				if (it->first == KB_matrix[i][k].substr(0, KB_matrix[i][k].find("("))) {
					match = 1;
					break;
				}
				k++;
			}
			if (match == 1) {
				k = 0;
				std::vector<std::string> eq;
				while (KB_matrix[i][k] != "0") {
					eq.push_back(KB_matrix[i][k]);
					k++;
				}
				it->second.push_back(eq);
				a++;
			}
		}
	}
	std::cout << "*************************" << "\n ";
	// Printing Negative Equations
	for (auto it = negative_equations.begin(); it != negative_equations.end(); it++) {
		std::cout << it->first << " : \n";
		for (int j = 0; j < it->second.size(); j++) {
			for (int k = 0; k < it->second[j].size(); k++) {
				std::cout << it->second[j][k] << " ";
			}
			std::cout << "\n";
		}
	}
	// Printing Positive Equations
	std::cout << "*************************" << "\n";
	for (auto it = positive_equations.begin(); it != positive_equations.end(); it++) {
		std::cout << it->first << " : \n";
		for (int j = 0; j < it->second.size(); j++) {
			for (int k = 0; k < it->second[j].size(); k++) {
				std::cout << it->second[j][k] << " ";
			}
			std::cout << "\n";
		}
	}
	std::vector<std::string> q;
	std::string den;
	den = query[0];
	if (den != "~") {
		den.insert(0, "~");
	} 
	q.push_back(den);
	std::cout<< "RESULT:" << FOL_engine(q, q.size() - 1) << "\n";

	/*std::cout << "\n" <<"Negative Facts Token" << "\n" << "\n";
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
	std::cout << "\n";
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