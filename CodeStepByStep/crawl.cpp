void crawl(string filename, string indent = "") {
	cout << indent;
    cout << getTail(filename) << endl;
    if (isDirectory(filename)) {
        indent.append("    ");
        filename.append(getDirectoryPathSeparator());
        
        vector<string> entries;
        listDirectory(filename, entries);
        for (auto& entry : entries)
            crawl(filename + "/" + entry, indent);
    }
}