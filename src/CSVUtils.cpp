#include "CSVUtils.h"
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <algorithm>
#include <cctype>

namespace CSV {

    // Split a CSV line into fields. Very simple quoted-field handling:
    // - Fields may be enclosed in double quotes.
    // - Double quotes inside quoted field are not handled (kept simple for school project).
    std::vector<std::string> split(const std::string &line, char delimiter) {
        std::vector<std::string> result;
        std::string cur;
        bool inQuotes = false;
        for (size_t i = 0; i < line.size(); ++i) {
            char ch = line[i];
            if (ch == '"') {
                inQuotes = !inQuotes;
                continue;
            }
            if (ch == delimiter && !inQuotes) {
                result.push_back(cur);
                cur.clear();
            } else {
                cur.push_back(ch);
            }
        }
        result.push_back(cur);
        return result;
    }

    std::string trim(const std::string &s) {
        if (s.empty()) return s;
        size_t start = 0;
        while (start < s.size() && std::isspace(static_cast<unsigned char>(s[start]))) ++start;
        size_t end = s.size();
        while (end > start && std::isspace(static_cast<unsigned char>(s[end - 1]))) --end;
        return s.substr(start, end - start);
    }

    std::vector<std::vector<std::string>> readCSV(const std::string &filename) {
        std::ifstream file(filename);
        if (!file.is_open()) throw std::runtime_error("Cannot open file: " + filename);

        std::vector<std::vector<std::string>> rows;
        std::string line;
        bool first = true;

        while (std::getline(file, line)) {
            if (first) { first = false; continue; } // skip header
            if (line.empty()) continue;
            auto parts = split(line, ',');
            for (auto &p : parts) p = trim(p);
            rows.push_back(parts);
        }
        return rows;
    }

    void writeCSV(const std::string &filename,
                  const std::vector<std::string> &header,
                  const std::vector<std::vector<std::string>> &rows) {
        std::ofstream file(filename);
        if (!file.is_open()) throw std::runtime_error("Cannot write to file: " + filename);

        for (size_t i = 0; i < header.size(); ++i) {
            file << header[i];
            if (i + 1 < header.size()) file << ",";
        }
        file << "\n";

        for (const auto &row : rows) {
            for (size_t i = 0; i < row.size(); ++i) {
                // quote if contains comma
                bool needQuote = row[i].find(',') != std::string::npos;
                if (needQuote) file << '"';
                file << row[i];
                if (needQuote) file << '"';
                if (i + 1 < row.size()) file << ",";
            }
            file << "\n";
        }
    }
}
