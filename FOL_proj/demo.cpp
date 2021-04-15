/*#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <math.h>
#include <sstream>
#include <map>
#include <algorithm>
//#include <bits/stdc++.h>

using namespace std;

map<string, vector<string>> map_pfact;
map<string, vector<string>> map_nfact;
map<string, vector<vector<string>>> map_pequate;
map<string, vector<vector<string>>> map_nequate;

std::string ReplaceAll(std::string str, const std::string& from, const std::string& to) {
    size_t start_pos = 0;
    while ((start_pos = str.find(from, start_pos)) != std::string::npos) {
        str.replace(start_pos, from.length(), to);
        start_pos += to.length(); // Handles case where 'to' is a substring of 'from'
    }
    return str;
}

vector<string> Substitute(vector<string> solution_vector, string i, string j) {
    vector<string> solution_vector2(100, "O");
    for (int k = 0; solution_vector[k] != "O" && k < 100; k++) {
        solution_vector2[k] = ReplaceAll(solution_vector[k], i, j);
    }

    return solution_vector2;

}


bool FOL_Resolver(vector<string> solution_vector, int i, int j) {
    bool org_result;//bool_result=false,
    vector<string> solution_vector3(100, "O");
    vector<string> implication(100, "O");
    if (i > j) {
        return true;
    }
    else {
        string unit_predicate, up; size_t n; int x, y, w, z, a, b, c;
        if (solution_vector[i][0] == '~') {
            unit_predicate = solution_vector[i].substr(1, solution_vector[i].find("(") - 1);
            //map_nfact[token] = map_2d_initializer;
            n = count(solution_vector[i].begin(), solution_vector[i].end(), ',');
            //for facts
            if (map_pfact.count(unit_predicate) > 0) {

                for (int k = 0; (map_pfact[unit_predicate][k] != "O") && (k < 100); k++) {
                    vector<string> solution_vector2(100, "O");
                    if (n == 0) {
                        x = solution_vector[i].find("(");
                        y = solution_vector[i].find(")");
                        w = map_pfact[unit_predicate][k].find("(");
                        z = map_pfact[unit_predicate][k].find(")");
                        if (solution_vector[i].substr(x + 1, y - x - 1) == map_pfact[unit_predicate][k].substr(w + 1, z - w - 1)) {
                            a = i + 1; b = j; c = 0;
                            while (a <= b) {
                                solution_vector2[c] = solution_vector[a];
                                c++; a++;
                            }
                            org_result = FOL_Resolver(solution_vector2, 0, c - 1);
                            if (org_result == true) {
                                //bool_result=true;
                                return true;
                            }
                        }
                        else if (islower((solution_vector[i].substr(x + 1, y - x - 1))[0])) {
                            solution_vector3 = Substitute(solution_vector, solution_vector[i].substr(x + 1, y - x - 1), map_pfact[unit_predicate][k].substr(w + 1, z - w - 1));//checking
                            a = i + 1; b = j; c = 0;
                            while (a <= b) {
                                solution_vector2[c] = solution_vector3[a];
                                c++; a++;
                            }
                            org_result = FOL_Resolver(solution_vector2, 0, c - 1);
                            if (org_result == true) {
                                //bool_result=true;
                                return true;
                            }
                        }

                    }
                    else if (n > 0) {
                        int f = 0;
                        solution_vector3 = solution_vector;
                        x = solution_vector3[i].find("(");
                        y = solution_vector3[i].find(",");
                        w = map_pfact[unit_predicate][k].find("(");
                        z = map_pfact[unit_predicate][k].find(",");
                        if (solution_vector3[i].substr(x + 1, y - x - 1) == map_pfact[unit_predicate][k].substr(w + 1, z - w - 1)) {
                            f++;
                        }
                        else if (islower((solution_vector3[i].substr(x + 1, y - x - 1))[0])) {
                            solution_vector3 = Substitute(solution_vector3, solution_vector3[i].substr(x + 1, y - x - 1), map_pfact[unit_predicate][k].substr(w + 1, z - w - 1));
                            f++;
                        }

                        for (int l = n - 1; l > 0; l--) {
                            x = solution_vector3[i].find(",", x + 1);
                            y = solution_vector3[i].find(",", y + 1);
                            w = map_pfact[unit_predicate][k].find(",", w + 1);
                            z = map_pfact[unit_predicate][k].find(",", z + 1);
                            if (solution_vector3[i].substr(x + 1, y - x - 1) == map_pfact[unit_predicate][k].substr(w + 1, z - w - 1)) {
                                f++;
                            }
                            else if (islower((solution_vector3[i].substr(x + 1, y - x - 1))[0])) {
                                solution_vector3 = Substitute(solution_vector3, solution_vector3[i].substr(x + 1, y - x - 1), map_pfact[unit_predicate][k].substr(w + 1, z - w - 1));
                                f++;
                            }

                        }

                        x = solution_vector3[i].find(",", x + 1);
                        y = solution_vector3[i].find(")");
                        w = map_pfact[unit_predicate][k].find(",", w + 1);
                        z = map_pfact[unit_predicate][k].find(")");
                        if (solution_vector3[i].substr(x + 1, y - x - 1) == map_pfact[unit_predicate][k].substr(w + 1, z - w - 1)) {
                            f++;
                        }
                        else if (islower((solution_vector3[i].substr(x + 1, y - x - 1))[0])) {
                            solution_vector3 = Substitute(solution_vector3, solution_vector3[i].substr(x + 1, y - x - 1), map_pfact[unit_predicate][k].substr(w + 1, z - w - 1));
                            f++;
                        }

                        if (f == n + 1) {
                            a = i + 1; b = j; c = 0;
                            while (a <= b) {
                                solution_vector2[c] = solution_vector3[a];
                                c++; a++;
                            }
                            org_result = FOL_Resolver(solution_vector2, 0, c - 1);
                            if (org_result == true) {
                                //bool_result=true;
                                return true;
                            }
                        }

                    }
                }
            }
            //for equations
            if (map_pequate.count(unit_predicate) > 0) {

                for (int k = 0; (map_pequate[unit_predicate][k][0] != "O") && k < 100; k++) {

                    //implication=map_pequate[unit_predicate][k];
                    for (int t = 0; (map_pequate[unit_predicate][k][t] != "O") && t < 100; t++) {
                        vector<string> solution_vector2(100, "O");
                        up = map_pequate[unit_predicate][k][t].substr(0, map_pequate[unit_predicate][k][t].find("("));
                        //checking
                        if (unit_predicate == up) {
                            implication = map_pequate[unit_predicate][k];

                            if (n == 0) {
                                //copied
                                x = solution_vector[i].find("(");
                                y = solution_vector[i].find(")");
                                w = implication[t].find("(");
                                z = implication[t].find(")");
                                if ((solution_vector[i].substr(x + 1, y - x - 1) == implication[t].substr(w + 1, z - w - 1)) && (isupper((solution_vector[i].substr(x + 1, y - x - 1))[0])) && (isupper((implication[t].substr(w + 1, z - w - 1))[0]))) {
                                    a = i + 1; b = j; c = 0;
                                    while (a <= b) {
                                        solution_vector2[c] = solution_vector[a];
                                        c++; a++;
                                    }

                                    for (int m = 0; (implication[m] != "O") && m < 100; m++) {
                                        if (m != t) {
                                            solution_vector2[c] = implication[m];
                                            c++;
                                        }
                                    }

                                    org_result = FOL_Resolver(solution_vector2, 0, c - 1);
                                    if (org_result == true) {
                                        //bool_result=true;
                                        return true;
                                    }
                                }
                                else if ((islower((solution_vector[i].substr(x + 1, y - x - 1))[0])) && (isupper((implication[t].substr(w + 1, z - w - 1))[0]))) {//checking
                                    solution_vector3 = Substitute(solution_vector, solution_vector[i].substr(x + 1, y - x - 1), implication[t].substr(w + 1, z - w - 1));
                                    a = i + 1; b = j; c = 0;
                                    while (a <= b) {
                                        solution_vector2[c] = solution_vector3[a];
                                        c++; a++;
                                    }

                                    for (int m = 0; (implication[m] != "O") && m < 100; m++) {
                                        if (m != t) {
                                            solution_vector2[c] = implication[m];
                                            c++;
                                        }
                                    }

                                    org_result = FOL_Resolver(solution_vector2, 0, c - 1);
                                    if (org_result == true) {
                                        //bool_result=true;
                                        return true;
                                    }
                                }
                                else if (islower((implication[t].substr(w + 1, z - w - 1))[0])) {//(isupper((solution_vector[i].substr(x+1, y-x-1))[0]))&&//checking
                                    implication = Substitute(implication, implication[t].substr(w + 1, z - w - 1), solution_vector[i].substr(x + 1, y - x - 1));
                                    a = i + 1; b = j; c = 0;
                                    while (a <= b) {
                                        solution_vector2[c] = solution_vector[a];
                                        c++; a++;
                                    }

                                    for (int m = 0; (implication[m] != "O") && m < 100; m++) {
                                        if (m != t) {
                                            solution_vector2[c] = implication[m];
                                            c++;
                                        }
                                    }

                                    org_result = FOL_Resolver(solution_vector2, 0, c - 1);
                                    if (org_result == true) {
                                        //bool_result=true;
                                        return true;
                                    }
                                }
                            }
                            else if (n > 0) {
                                //copied
                                int f = 0;
                                solution_vector3 = solution_vector;
                                x = solution_vector3[i].find("(");
                                y = solution_vector3[i].find(",");
                                w = implication[t].find("(");
                                z = implication[t].find(",");
                                if ((solution_vector3[i].substr(x + 1, y - x - 1) == implication[t].substr(w + 1, z - w - 1)) && (isupper((solution_vector3[i].substr(x + 1, y - x - 1))[0])) && (isupper((implication[t].substr(w + 1, z - w - 1))[0]))) {
                                    f++;
                                }
                                else if ((islower((solution_vector3[i].substr(x + 1, y - x - 1))[0])) && (isupper((implication[t].substr(w + 1, z - w - 1))[0]))) {
                                    solution_vector3 = Substitute(solution_vector3, solution_vector3[i].substr(x + 1, y - x - 1), implication[t].substr(w + 1, z - w - 1));
                                    f++;
                                }
                                else if (islower((implication[t].substr(w + 1, z - w - 1))[0])) {//(isupper((solution_vector[i].substr(x+1, y-x-1))[0]))&&
                                    implication = Substitute(implication, implication[t].substr(w + 1, z - w - 1), solution_vector3[i].substr(x + 1, y - x - 1));
                                    f++;
                                }

                                for (int l = n - 1; l > 0; l--) {
                                    x = solution_vector3[i].find(",", x + 1);
                                    y = solution_vector3[i].find(",", y + 1);
                                    w = implication[t].find(",", w + 1);
                                    z = implication[t].find(",", z + 1);
                                    if ((solution_vector3[i].substr(x + 1, y - x - 1) == implication[t].substr(w + 1, z - w - 1)) && (isupper((solution_vector3[i].substr(x + 1, y - x - 1))[0])) && (isupper((implication[t].substr(w + 1, z - w - 1))[0]))) {
                                        f++;
                                    }
                                    else if ((islower((solution_vector3[i].substr(x + 1, y - x - 1))[0])) && (isupper((implication[t].substr(w + 1, z - w - 1))[0]))) {
                                        solution_vector3 = Substitute(solution_vector3, solution_vector3[i].substr(x + 1, y - x - 1), implication[t].substr(w + 1, z - w - 1));
                                        f++;
                                    }
                                    else if (islower((implication[t].substr(w + 1, z - w - 1))[0])) {//(isupper((solution_vector[i].substr(x+1, y-x-1))[0]))&&
                                        implication = Substitute(implication, implication[t].substr(w + 1, z - w - 1), solution_vector3[i].substr(x + 1, y - x - 1));
                                        f++;
                                    }

                                }

                                x = solution_vector3[i].find(",", x + 1);
                                y = solution_vector3[i].find(")");
                                w = implication[t].find(",", w + 1);
                                z = implication[t].find(")");
                                if ((solution_vector3[i].substr(x + 1, y - x - 1) == implication[t].substr(w + 1, z - w - 1)) && (isupper((solution_vector3[i].substr(x + 1, y - x - 1))[0])) && (isupper((implication[t].substr(w + 1, z - w - 1))[0]))) {
                                    f++;
                                }
                                else if ((islower((solution_vector3[i].substr(x + 1, y - x - 1))[0])) && (isupper((implication[t].substr(w + 1, z - w - 1))[0]))) {
                                    solution_vector3 = Substitute(solution_vector3, solution_vector3[i].substr(x + 1, y - x - 1), implication[t].substr(w + 1, z - w - 1));
                                    f++;
                                }
                                else if (islower((implication[t].substr(w + 1, z - w - 1))[0])) {//(isupper((solution_vector[i].substr(x+1, y-x-1))[0]))&&
                                    implication = Substitute(implication, implication[t].substr(w + 1, z - w - 1), solution_vector3[i].substr(x + 1, y - x - 1));
                                    f++;
                                }

                                if (f == n + 1) {
                                    a = i + 1; b = j; c = 0;
                                    while (a <= b) {
                                        solution_vector2[c] = solution_vector3[a];
                                        c++; a++;
                                    }

                                    for (int m = 0; (implication[m] != "O") && m < 100; m++) {
                                        if (m != t) {
                                            solution_vector2[c] = implication[m];
                                            c++;
                                        }
                                    }

                                    org_result = FOL_Resolver(solution_vector2, 0, c - 1);
                                    if (org_result == true) {
                                        //bool_result=true;
                                        return true;
                                    }
                                }
                            }
                        }
                    }

                }
            }
        }
        else {
            unit_predicate = solution_vector[i].substr(0, solution_vector[i].find("("));
            //map_pfact[token] = map_2d_initializer;

        }
    }

    return false;
}

int main()
{
    string no_queries, no_kb, str, token;
    int i = 0, e1 = 0, e2 = 0, f = 0, flag, k, t;
    bool bool_result;
    string query[10], KB[100];
    vector<vector<string>> equations(100, vector<string>(100, "O"));
    vector<string> facts(100, "O");
    vector<string> map_2d_initializer(100, "O");
    vector<vector<string>> map_3d_initializer(100, vector<string>(100, "O"));
    //map<string, vector<string>> map_pfact;
    //map<string, vector<string>> map_nfact;
    //map<string, vector<vector<string>>> map_pequate;
    //map<string, vector<vector<string>>> map_nequate;
    //string equations[100][100], facts[100];
    ifstream input_file("input3.txt");
    ofstream output_file("output.txt");
    
    while (getline(input_file, str))
    {
        cout<<str<<endl;
    }
    
    getline(input_file, no_queries);


    while (i < stoi(no_queries)) {
        getline(input_file, query[i]);
        //cout<<query[i]<<endl;
        i++;
    }
    getline(input_file, no_kb);

    i = 0;
    while (i < stoi(no_kb)) {
        getline(input_file, KB[i]);
        //cout<<KB[i]<<endl;
        i++;
    }

    i = 0;
    while (i < stoi(no_kb)) {

        size_t found = KB[i].find("=>");
        if (found != string::npos) {
            e2 = 0; flag = 0;
            stringstream ss(KB[i]);
            while (ss >> str) {
                if (str == "=>") {
                    flag = 1;
                }
                if (flag == 0 && str != "&" && str != "=>") {

                    if (str[0] == '~') {
                        str.erase(0, 1);
                    }
                    else {
                        str.insert(0, "~");
                    }
                    equations[e1][e2] = str;
                    e2++;
                }
                else if (flag == 1 && str != "&" && str != "=>") {
                    equations[e1][e2] = str;
                    e2++;
                }

            }
            e1++;
        }
        else {

            facts[f] = KB[i];
            f++;
        }

        i++;
    }
    /*
    cout<<"Equations"<<endl;
    e1=0;
    while(e1<100&&equations[e1][0]!="O"){
        e2=0;
        while(e2<100&&equations[e1][e2]!="O"){
            cout<<equations[e1][e2]<<" ";
            e2++;
        }
        cout<<endl;
        e1++;
    }

    cout<<"facts"<<endl;
    f=0;
    while(f<100&&facts[f]!="O"){
        cout<<facts[f]<<endl;
        f++;
    }
    
    f = 0;
    while (f < 100 && facts[f] != "O") {
        if (facts[f][0] == '~') {
            token = facts[f].substr(1, facts[f].find("(") - 1);
            map_nfact[token] = map_2d_initializer;
            //cout<<"negative "<<token<<" "<<map_nfact[token][0]<<endl;
        }
        else {
            token = facts[f].substr(0, facts[f].find("("));
            map_pfact[token] = map_2d_initializer;
            //cout<<"positive "<<token<<" "<<map_pfact[token][0]<<endl;
        }
        f++;
    }

    f = 0;
    while (f < 100 && facts[f] != "O") {
        if (facts[f][0] == '~') {
            token = facts[f].substr(1, facts[f].find("(") - 1);
            k = 0;
            while (map_nfact[token][k] != "O") {
                k++;
            }
            map_nfact[token][k] = facts[f];

        }
        else {
            token = facts[f].substr(0, facts[f].find("("));
            k = 0;
            while (map_pfact[token][k] != "O") {
                k++;
            }
            map_pfact[token][k] = facts[f];
        }
        f++;
    }
    //check equation logic once or twice
    e1 = 0;
    while (e1 < 100 && equations[e1][0] != "O") {
        e2 = 0;
        while (e2 < 100 && equations[e1][e2] != "O") {
            //cout<<equations[e1][e2]<<" ";
            if (equations[e1][e2][0] == '~') {
                token = equations[e1][e2].substr(1, equations[e1][e2].find("(") - 1);
                map_nequate[token] = map_3d_initializer;
            }
            else {
                token = equations[e1][e2].substr(0, equations[e1][e2].find("("));
                map_pequate[token] = map_3d_initializer;
            }
            e2++;
        }
        //cout<<endl;
        e1++;
    }

    e1 = 0;
    while (e1 < 100 && equations[e1][0] != "O") {
        e2 = 0;
        while (e2 < 100 && equations[e1][e2] != "O") {
            //cout<<equations[e1][e2]<<" ";
            if (equations[e1][e2][0] == '~') {
                token = equations[e1][e2].substr(1, equations[e1][e2].find("(") - 1);
                k = 0;
                while (map_nequate[token][k][0] != "O") {
                    k++;
                }
                t = 0;
                while (equations[e1][t] != "O") {
                    map_nequate[token][k][t] = equations[e1][t];
                    t++;
                }
            }
            else {
                token = equations[e1][e2].substr(0, equations[e1][e2].find("("));
                k = 0;
                while (map_pequate[token][k][0] != "O") {
                    k++;
                }
                t = 0;
                while (equations[e1][t] != "O") {
                    map_pequate[token][k][t] = equations[e1][t];
                    t++;
                }
            }
            e2++;
        }
        //cout<<endl;
        e1++;
    }
    /*
    k=0;
    while(map_pfact["Vaccinated"][k]!="O"){
        cout<<map_pfact["Vaccinated"][k]<<endl;
        //cout<<" ";
        k++;
    }
    */
    //cout<<map_pfact["Healthy"][1]<<endl;
    /*
    k=0;
    while(map_nequate["Learn"][k][0]!="O"){
        t=0;
        while(map_nequate["Learn"][k][t]!="O"){
            cout<<map_nequate["Learn"][k][t]<<" ";
            t++;
        }
        cout<<endl;
        k++;
    }
     

     //CODE FOR MAIN LOGIC OF RESOLUTION

    for (int q = 0; q < stoi(no_queries); q++) {
        vector<string> solution_vector(100, "O");
        solution_vector[0] = query[q];
        if (solution_vector[0][0] == '~') {
            solution_vector[0].erase(0, 1);
        }
        else {
            solution_vector[0].insert(0, "~");
        }
        bool_result = FOL_Resolver(solution_vector, 0, 0);
        //cout<<bool_result<<endl;
        cout << " Query: " << q << ", Result: " << ((bool_result == 1) ? "True" : "False") << " " << solution_vector[0] << endl;

    }

}*/