#include <cstdint>
#include <iostream>
#include <string>
#include "wordament_ai/wordament_ai.h"

namespace
{

bool GetGameMap(std::string
                game_map[][wordament_ai::WordamentAI::GAME_MAP_SIZE])
{
    for (int row = 0; row < wordament_ai::WordamentAI::GAME_MAP_SIZE; row++)
        for (int col = 0; col < wordament_ai::WordamentAI::GAME_MAP_SIZE; col++)
        {
            if (!(std::cin >> game_map[row][col]))  // failed to read string
            {
                return false;
            }
        }
    // read all
    return true;
}

void PrintGameMap(const std::string
                  game_map[][wordament_ai::WordamentAI::GAME_MAP_SIZE])
{
    for (int row = 0; row < wordament_ai::WordamentAI::GAME_MAP_SIZE; row++)
    {
        for (int col = 0; col < wordament_ai::WordamentAI::GAME_MAP_SIZE; col++)
        {
            std::cout << game_map[row][col] << ' ';
        }
        std::cout << std::endl;
    }
}

}  // unnamed namespace

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        std::cerr << "Usage: " << argv[0] << " <dictionary files> ...\n";
        std::exit(EXIT_FAILURE);
    }

    // read dictionary file names from argv
    std::vector<std::string> dictionary_names;
    for (int i = 1; i < argc; i++)
    {
        std::string file_name(argv[i]);
        dictionary_names.push_back(file_name);
    }

    // build the solver
    wordament_ai::WordamentAI solver(dictionary_names);

    // building success
    // tell user the number of words in dictionaries
    std::cout << solver.dictionary_word_count() << " word(s) read\n\n";

    std::cout << "Please enter the game map:\n";
    std::string game_map[wordament_ai::WordamentAI::GAME_MAP_SIZE]
                        [wordament_ai::WordamentAI::GAME_MAP_SIZE];
    while (GetGameMap(game_map))
    {
        // echo game map
        std::cout << "\nYour input:\n";
        PrintGameMap(game_map);
        std::cout << std::endl;
        int word_found_count = solver.FindWords(game_map);
        std::cout << "\nFound " << word_found_count << " words\n\n"
                  << "Please enter another map (q to quit):\n";
    }

    return 0;
}
