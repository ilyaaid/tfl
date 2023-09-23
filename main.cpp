#include <iostream>
#include <vector>
#include <map>
#include <set>
#include <fstream>

using namespace std;

void trimBrackets(string& s) {
    while (s[0] != '(') {
        s.erase(s.begin());
    }
    s.erase(s.begin());
    while (s[s.size() - 1] != ')') {
        s.erase(s.begin() + s.size() - 1);
    }
    s.erase(s.begin() + s.size() - 1);
}

void delSpaces(string& s) {
    for (int i = 0; i < s.size(); ++i) {
        if (s[i] == ' ') {
            s.erase(s.begin() + i);
            i--;
        }
    }
}

vector<string> splitStr(string& str, char delim) {
    vector<string> res;
    size_t pos = 0;
    std::string token;
    while ((pos = str.find(delim)) != std::string::npos) {
        token = str.substr(0, pos);
        res.push_back(token);
        str.erase(0, pos + 1);
    }
    res.push_back(str);
    return res;
}

set<string> getVars(string& vars_str) {
    delSpaces(vars_str);
    trimBrackets(vars_str);
    vector<string> vec_str = splitStr(vars_str, ',');
    set<string> st;
    for (auto str : vec_str) {
        st.insert(str);
    }
    return st;
}

struct Term {
    string val = "";
    Term* k0 = nullptr; // свободный коэф
    Term* k1 = nullptr; // коэф при первой
    Term* k2 = nullptr; // коэф при второй
};

struct Rule {
    Term* l = nullptr;
    Term* r = nullptr;
};

bool charInStr(char c, const string& str) {
    for (int i = 0; i < str.size(); ++i) {
        if (str[i] == c) {
            return true;
        }
    }
    return false;
}

Term* parseConstrStr(string& str, const set<string>& vars, set<string>& coefs) {
    if (str.size() == 0) {
        return nullptr;
    }
    string name;
    while (!charInStr(str[0], "(),")) {
        name += str[0];
        str.erase(str.begin());
    }

    if (name == "") {
        return nullptr;
    }

    Term* t = new Term;
    t->val = name;
    if (vars.find(t->val) == vars.end()) {
        t->k0 = new Term;
        coefs.insert(t->val + "_0");
    }
    if (charInStr(str[0], "),")) {
        return t;
    }
    if (str[0] == '(') {
        str.erase(str.begin());
        t->k1 = parseConstrStr(str, vars, coefs);
        if (t->k1) {
            coefs.insert(t->val + "_1");
        }
        if (str[0] == ',') {
            str.erase(str.begin());
            t->k2 = parseConstrStr(str, vars, coefs);
            if (t->k2) {
                coefs.insert(t->val + "_2");
            }
        }
        if (str[0] == ')') {
            str.erase(str.begin());
            return t;
        }
    }
    return nullptr;
}

void termTreeTraversal(Term* t, string cur_expr, map<string, string>& mp) {
    if (!t) {
        return;
    }
    if (cur_expr == "") {
        cur_expr = "1";
    }
     // если терм является переменной
    if (!t->k0) {
        mp[t->val] = "(+ " + mp[t->val] + " " + cur_expr + ")";
    } else {
        // для свободного коэфа
        mp["0"] = "(+ " + mp["0"] + " " + "(* " + cur_expr + " " + t->val + "_0" + ")" + ")";

        string tmp_cur_expr = cur_expr; 

        // для первого
        cur_expr = "(* " + cur_expr + " " + t->val + "_1" + ")";
        termTreeTraversal(t->k1, cur_expr, mp);

        cur_expr = tmp_cur_expr;

        // для второго
        cur_expr = "(* " + cur_expr + " " + t->val + "_2" + ")";
        termTreeTraversal(t->k2, cur_expr, mp);
        cur_expr = tmp_cur_expr;
    } 
}

void updateMapToCoefSmt(map<string, string>& mp, Term* root, const set<string>& vars) {
    for (auto var : vars) {
        mp[var] = "0";
    }
    mp["0"] = "0"; // добавили свободный коэффициент

    string cur_expr; // стек, который будет меняться при обходе дерева термов

    termTreeTraversal(root, cur_expr, mp);

}

void addCoefsToSmt(string& smt_content, const set<string>& coefs) {
    for (auto coef : coefs) {
        smt_content += "(declare-fun " + coef + " () Int)\n";
    }
}

void addAssertionsToSmt(string& smt_content, const set<string>& coefs,
const map<string, string>& l, const map<string, string>& r) {
    // по каждым коэффициентам сравнение
    for (auto p_l : l) {
        auto p_r = r.find(p_l.first);
        smt_content += "(assert (>= " + p_l.second + " " + p_r->second + "))\n";
    }
    smt_content += "(assert (or";

    // условие строгости убывания
    for (auto p_l : l) {
        auto p_r = r.find(p_l.first);
        smt_content += " (> " + p_l.second + " " + p_r->second + ")";
    }
    smt_content += "))\n";

    // нестрогая монотонность 
    smt_content += "(assert (and";
    for (auto coef : coefs) {
        smt_content += " (>= " + coef;
        if (coef[coef.size() - 1] == '0') {
            smt_content +=" 0)";
        } else {
            smt_content +=" 1)";
        }
    }
    smt_content += "))\n";

    map<string, string> const_to_smt_str;
    for (auto coef : coefs) {
        string const_name(coef.begin(), coef.begin() + coef.size() - 2);
        if (const_to_smt_str.find(const_name) == const_to_smt_str.end()) {
            const_to_smt_str[const_name] = "(or";
        }
        const_to_smt_str[const_name] += " (> " + coef + " ";
        if (coef[coef.size() - 1] == '0') {
            const_to_smt_str[const_name] += "0)";
        } else {
            const_to_smt_str[const_name] += "1)";
        }
    }

    smt_content += "(assert (and";
    for (auto coef_smt : const_to_smt_str) {
        smt_content += " " + coef_smt.second + ")";
    }
    smt_content += "))\n";
}

// TODO: ДОДЕЛАТЬ ОЧИЩЕНИЕ ДИНАМИЧЕСКОЙ ПАМЯТИ ДЛЯ ДЕРЕВА ТЕРМОВ!!!

int main() {
    cout << "Введите variables(в скобках через запятую без пробелов) в формате \"(x,y,z,...)\":" << endl;
    string vars_str;
    getline(cin, vars_str);
    // Получили список переменных
    set<string> vars = getVars(vars_str);

    cout << "Введите правила в формате f(g(x,y)) = g(h(y),x) (по окончании ввода ввести Ctrl+D):" << endl;
    vector<Rule> rules; // список пар указателей на вершинные термы
    string rule_str;
    set<string> coefs;
    while (getline(cin, rule_str)) {
        delSpaces(rule_str);
        vector<string> rule_vec = splitStr(rule_str, '=');
        Rule newR;
        newR.l = parseConstrStr(rule_vec[0], vars, coefs);
        newR.r = parseConstrStr(rule_vec[1], vars, coefs);
        rules.push_back(newR);
    }
    // на данный момент дерево термов построено

    string smt_content = "";
    smt_content += "(set-logic QF_NIA)\n"; // добавил header
    addCoefsToSmt(smt_content, coefs); // добавил коэффициенты
    smt_content += "\n";

    for (auto rule : rules) {
        // соответствие между переменной и коэффициентом перед ней в формате smt выражения
        map<string, string> l;
        map<string, string> r;
        updateMapToCoefSmt(l, rule.l, vars);
        updateMapToCoefSmt(r, rule.r, vars);
        addAssertionsToSmt(smt_content, coefs, l, r);
    }
    
    smt_content += "\n";
    smt_content += "(check-sat)\n(get-model)\n";

    ofstream out("test.smt2", ios_base::trunc);
    out << smt_content;
    out.close();
    cout << smt_content;


    cout << endl;
    return 0;
}
