void collapseSequences(const string&, int, char, string&);
    
string collapseSequences(string s, char c) {
    string temp;
    collapseSequences(s, 0, c, temp);
    return temp;
}

void collapseSequences(const string& s, int i, char c, string& temp) {
    if (i < s.size()) {
        if (s[i] != c || temp.size() == 0 || temp[temp.size() - 1] != s[i])
            temp.push_back(s[i]);
        collapseSequences(s, i + 1, c, temp);
    }
}