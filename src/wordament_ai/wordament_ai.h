#ifndef WORDAMENT_AI_WORDAMENT_AI_H_
#define WORDAMENT_AI_WORDAMENT_AI_H_

#include <cstdint>
#include <stack>
#include <string>
#include <unordered_set>
#include <vector>
#include "wordament_ai/dictionary.h"

namespace wordament_ai
{

class WordamentAI
{
 public:
    enum { GAME_MAP_SIZE = 4 };
    explicit WordamentAI(const std::vector<std::string> &dictionary_files);
    ~WordamentAI() {}

    std::int_least32_t dictionary_word_count() const
    { return dictionary_word_count_; }
    int FindWords(const std::string game_map[][GAME_MAP_SIZE]);

 private:
    struct Node;

    static void PrintSolution(const Node *last_node);
    static bool NodeIsWorse(const wordament_ai::WordamentAI::Node &node1,
                            const wordament_ai::WordamentAI::Node &node2);
    static bool IsPrefix(const std::string &word);

    Dictionary dictionary_;
    std::int_least32_t dictionary_word_count_;
};

inline bool WordamentAI::IsPrefix(const std::string &word)
{
    // if the last charactor of the word is '-', it is a prefix
    return *(word.rbegin()) == '-';
}

}  // namesapce wordament_ai

#endif  // WORDAMENT_AI_WORDAMENT_AI_H_
