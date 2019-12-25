#include <iostream>
#include "Dictionary.h"

using namespace std::string_literals;

void replace_dialog(std::string &word, Dictionary &d) {
    if (d.exact_match(word)) {
        return;
    }
    auto sug = d.close_match(word, 1);
    if (sug.empty()) {
        std::cout << "The word \""s + word + "\" not found in the dictionary.\n"
                                             "Input 0 to add in the dictionary or input replacement:\n";
        std::string res;
        std::cin >> res;
        if (res == "0"s) {
            d.add_to_dict(word);
            return;
        } else {
            d.add_to_dict(res);
            word = std::move(res);
            return;
        }
    }
    std::cout << "The word \""s + word + "\" not found in the dictionary.\n"
                                         "Input 0 to add in the dictionary, number of the suggested "
                                         "replacement or input your own replacement:\n";
    for (size_t i = 0; i < sug.size(); ++i) {
        std::cout << std::to_string(i + 1) + ": " << sug[i] << std::endl;
    }

    while (true) {
        std::string res;
        std::cin >> res;
        int n;
        try {
            n = std::stoi(res);
        } catch (std::invalid_argument &e) {
            word = res;
            return;
        }

        if (n == 0) {
            d.add_to_dict(word);
            return;
        } else if (n <= sug.size()) {
            word = sug[n - 1].result;
            return;
        } else {
            std::cout << "Incorrect input. Try again.\n";
        }
    }
}

std::tuple<std::ifstream, std::fstream, std::ofstream> parse_args(int argc, char *argv[]) {
    if (argc != 4) {
        std::cout << "Incorrect arguments.\n"
                     "Usage: ./Dictionary inputfile outputfile dictfile\n";
        std::exit(0);
    }
    auto input = std::ifstream{argv[1]};
    auto output = std::ofstream{argv[2]};
    auto dict = std::fstream{argv[3]};
    if (!input.is_open()) {
        std::cerr << "Couldn't open file " << argv[1] << "\n";;
        std::exit(1);
    }
    if (!output.is_open()) {
        std::cerr << "Couldn't open file " << argv[2] << "\n";;
        std::exit(1);
    }
    if (!dict.is_open()) {
        std::cerr << "Couldn't open file " << argv[3] << "\n";;
        std::exit(1);
    }
    return {std::move(input), std::move(dict), std::move(output)};
}

std::string replacer(const std::string &s, Dictionary &d) {
    std::string res;
    res.reserve(s.size());
    static std::regex word_re{R"(\b(\w+)\b)"};
    std::sregex_token_iterator start_it{s.cbegin(), s.cend(), word_re};
    std::sregex_token_iterator stop_it{};
    auto first_unwritten_sym = s.cbegin();
    for (auto it = start_it; it != stop_it; ++it) {
        res += std::string(first_unwritten_sym, it->first);
        first_unwritten_sym = it->second;
        auto word = it->str();
        replace_dialog(word, d);
        res += word;
    }
    res += std::string(first_unwritten_sym, s.cend());
    return res;
}

int main(int argc, char *argv[]) {
    auto[inputf, dictf, outputf] = parse_args(argc, argv);
    try {
        Dictionary dictionary{std::move(dictf)};
        std::string s;
        while (inputf) {
            std::getline(inputf, s);
            if (!inputf) {
                break;
            }
            outputf << replacer(s, dictionary) << std::endl;
        }

    } catch (std::exception &e) {
        std::cerr << e.what() << std::endl;
    }
    return 0;
}