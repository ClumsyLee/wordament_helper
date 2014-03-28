#ifndef WORDAMENT_AI_DICTIONARY_H_
#define WORDAMENT_AI_DICTIONARY_H_

#include <cstdint>
#include <string>
#include <unordered_map>

namespace wordament_ai
{

class Dictionary
{
 public:
    enum { IS_WORD = 1, IS_SUB_WORD = 2 };
    enum { LEAST_LENGTH = 3 };

    Dictionary() {};
    ~Dictionary() {};

    std::int_least32_t ReadWordsFromFile(const char *filename);
    int LookUp(const std::string &word) const;
    void Show() const;

 private:
    std::unordered_map<std::string, int> word_map_;
};

}  // namespace wordament_ai

#endif  // WORDAMENT_AI_DICTIONARY_H_
