#include "wordament_ai/dictionary.h"
#include <cstdlib>
#include <iostream>

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        std::cerr << "Usage: " << argv[0] << " <input file>\n";
        std::exit(EXIT_FAILURE);
    }

    wordament_ai::Dictionary dictionary;
    int word_count = dictionary.ReadWordsFromFile(argv[1]);
    dictionary.Show();
    std::cout << word_count << " words in map\n";
    
    return 0;
}
