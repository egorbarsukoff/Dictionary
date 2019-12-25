//
// Created by egor on 12/18/19.
//

#ifndef DICTIONARY__DICTIONARY_H
#define DICTIONARY__DICTIONARY_H

#include <fstream>
#include <vector>
#include <set>
#include <regex>
#include <ostream>

class Dictionary {
    std::set<std::string> words; // set гарантрует валидность ссылок,  в отличие от unordered_set
    std::fstream file;
public:
    static int l_distance(std::string_view s1, std::string_view s2);
    struct SearchResult {
        int distance;
        std::string_view result;
        friend std::ostream &operator<<(std::ostream &os, const SearchResult &result);
    };
    explicit Dictionary(std::fstream file);
    explicit Dictionary(const std::string &path_to_file);
    bool exact_match(std::string s) const ;
    std::vector<Dictionary::SearchResult> close_match(std::string s, int threshold) const;
    void add_to_dict(std::string s);
};

#endif //DICTIONARY__DICTIONARY_H
