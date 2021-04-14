#include <iostream> 
#include <fstream> 
#include <string>
#include <sstream>
#include <vector>
#include <map>
#include <stack>

void FOL_engine(std::stack<std::string> query, std::map<std::string, std::vector<std::string>> positive_facts) {
	bool result = false; 
	std::string demo;
	std::string clause = query.top(); // Removing the Top element from the stack to be resolved
	if (clause[0] == '~') {
		auto it = positive_facts.begin();
		for (it ; (it != positive_facts.end() && it->first != clause.substr(1, clause.find("(") - 1)); it++) { // Find the token == clause token
			std::cout << it->first << " : \n";
			std::cout << it->second.size() << "\n";
		}
		std::cout << it->first << " : \n" ;
		std::cout << it->second.size() << "\n";
		int cl_arguments = std::count(clause.begin(), clause.end(), ',') + 1; // Number of arguments in Clause
		std::vector<std::string> param_cl;
		std::string cl_arg_str = clause.substr(clause.find("(")+1, clause.find(")") - clause.find("(") - 1); // Argument String

		// Storing arguments in a string vector
		std::stringstream ss(cl_arg_str);
		while (ss.good()) {
			std::string word;
			getline(ss, word, ',');
			param_cl.push_back(word);
		}
		for (size_t i = 0; i < it->second.size(); i++) {
			std::vector<std::string> param_pred;
			std::string pred_arg_str = it->second[i].substr(it->second[i].find("(") + 1, it->second[i].find(")") - it->second[i].find(")") - 1);
			std::stringstream ss2(pred_arg_str);
			while (ss2.good()) {
				std::string word;
				getline(ss2, word, ",");
				param_pred.push_back(word);
			}
			if (param_cl == param_pred) {
				std::cout << "TRUE" << "\n";
			}
			else if (param_pred)
		}
		for (int i = 0; i < param_cl.size(); i++) {
			std::cout << param[i] << "\n";
		}
		std::cout << arg_str << "\n";
		//std::cout << arguments << "\n";
		for (int i = 0; i < it->second.size(); i++) {
			
			std::cout << "DOLO \n";
		}
	}
	else {
		std::cout << "Hello \n";
	}
}

int main() {
	std::string temp_query;
	std::string temp_num_KB;
	std::map<std::string, std::vector<std::string>> negative_facts;
	std::map<std::string, std::vector<std::string>> positive_facts;
	std::map<std::string, std::vector<std::vector<std::string>>> negative_equations;
	std::map<std::string, std::vector<std::vector<std::string>>> positive_equations;
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
	std::stack<std::string> q;
	std::string den;
	den = query[0];
	if (den != "~") {
		den.insert(0, "~");
	} 
	q.push(den);
	FOL_engine(q, positive_facts);

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