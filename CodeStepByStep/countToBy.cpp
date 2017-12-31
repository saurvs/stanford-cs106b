void countToBy(int, int, int);
    
void countToBy(int n, int m) {
    if (n < 1 || m < 1)
        throw -1;
    else if (n < m)
        cout << n << endl;
    else {
        countToBy(n, m, n % m == 0 ? 1 : n % m);
        cout << endl;
    }
}

void countToBy(int n, int m, int i) {
    if (i <= n) {
        cout << i;
        if (i != n)
            cout << ", ";
        countToBy(n, m, i + m);
    }
}

