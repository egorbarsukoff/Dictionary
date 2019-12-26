//
// Created by egor on 12/18/19.
//

#include <iostream>
#include "Dictionary.h"

Dictionary::Dictionary(const std::string &path_to_file) : Dictionary{std::fstream{path_to_file}} {

}

Dictionary::Dictionary(std::fstream file_) : file{std::move(file_)} {
    static std::regex word{"\\w+"};
    std::transform(std::istream_iterator<std::string>{file},
                   std::istream_iterator<std::string>{},
                   std::inserter(words, words.end()),
                   [](auto s) {
                       if (auto word_match = std::regex_match(s, word); word_match) {
                           std::transform(s.begin(), s.end(), s.begin(),
                                          [](unsigned char c) { return std::tolower(c); });
                           return s;
                       }
                       std::cerr << s.size();
                       throw std::runtime_error{"Failed to parse \"" + s + "\""};
                   });
    file.clear();
}

bool Dictionary::exact_match(std::string s) const {
    std::transform(s.begin(), s.end(), s.begin(),
                   [](unsigned char c) { return std::tolower(c); });
    return words.count(s);
}
void Dictionary::add_to_dict(std::string s) {
    std::transform(s.begin(), s.end(), s.begin(),
                   [](unsigned char c) { return std::tolower(c); });
    if (!words.count(s)) {
        file << "\n" << s;
        words.emplace(std::move(s));
    }
}

int Dictionary::l_distance(std::string_view s1, std::string_view s2) {
    std::vector<std::vector<int>> arr(s1.size() + 1, std::vector<int>(s2.size() + 1));
    for (size_t i = 1; i < arr.size(); ++i) {
        arr[i][0] = i;
    }
    for (size_t i = 1; i < arr.back().size(); ++i) {
        arr[0][i] = i;
    }
    for (size_t j = 1; j < arr.back().size(); ++j) {
        for (size_t i = 1; i < arr.size(); ++i) {
            arr[i][j] = std::min({arr[i - 1][j] + 1,
                                  arr[i][j - 1] + 1,
                                  arr[i - 1][j - 1] + (s1[i-1] != s2[j-1] ? 1 : 0)});
        }
    }
    return arr.back().back();
}

std::vector<Dictionary::SearchResult> Dictionary::close_match(std::string s1, int threshold) const {
    std::vector<SearchResult> distances;
    distances.reserve(words.size());
    std::transform(s1.begin(), s1.end(), s1.begin(),
                   [](unsigned char c) { return std::tolower(c); });
    std::transform(words.cbegin(), words.cend(), std::back_insert_iterator{distances},
                   [&s1](auto &s2) {
                       return Dictionary::SearchResult{l_distance(s1, s2), s2};
                   });
    distances.erase(std::remove_if(distances.begin(), distances.end(),
                                   [threshold](auto res) {
                                       return res.distance > threshold;
                                   }),
                    distances.end());
    return distances;
}
std::ostream &operator<<(std::ostream &os, const Dictionary::SearchResult &result) {
    os << " result: " << result.result << " (distance " << result.distance << ")";
    return os;
}
