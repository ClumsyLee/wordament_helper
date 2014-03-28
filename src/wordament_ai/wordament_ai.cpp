#include "wordament_ai/wordament_ai.h"
#include <cstdlib>
#include <iostream>
#include <vector>

namespace
{
    enum { U, D, L, R, LU, LD, RU, RD };
    enum { DIRECTION_NUM = 8 };

    const int direction_row_offset[DIRECTION_NUM] = {
        -1, 1, 0, 0, -1, 1, -1, 1
    };
    const int direction_col_offset[DIRECTION_NUM] = {
        0, 0, -1, 1, -1, -1, 1, 1
    };
    const char *direction_arrow[DIRECTION_NUM] = {
        "↑", "↓", "←", "→", "↖", "↙", "↗", "↘"
    };
    const char *green_direction_arrow[DIRECTION_NUM] = {
        "\e[32m↑\e[0m", "\e[32m↓\e[0m", "\e[32m←\e[0m", "\e[32m→\e[0m",
        "\e[32m↖\e[0m", "\e[32m↙\e[0m", "\e[32m↗\e[0m", "\e[32m↘\e[0m"
    };

}  // unnamed namespace

namespace wordament_ai
{

struct WordamentAI::Node
{
    Node(int row, int col, const std::string &word);
    Node(const Node *old_node, int direction, const std::string &add_on_word);

    bool moved_squares[GAME_MAP_SIZE][GAME_MAP_SIZE];
    int currrent_row;
    int currrent_col;
    std::vector<int> previous_moves;
    std::string word_now;
};

// functions for WordamentAI
WordamentAI::WordamentAI(const std::vector<std::string> &dictionary_files)
        : dictionary_(),
          dictionary_word_count_(0),
          node_stack_()
{
    // load dictionaries
    for (std::string filename : dictionary_files)
        dictionary_word_count_ +=
                dictionary_.ReadWordsFromFile(filename.c_str());

    if (dictionary_word_count_ == 0)  // no word loaded
    {
        std::cerr << "Cannot read words from direction files\n"
                     "Files:\n";
        for (std::string filename : dictionary_files)
            std::cerr << filename << std::endl;
        // no better choice, just shut the program down
        std::exit(EXIT_FAILURE);
    }
}

int WordamentAI::FindWords(const std::string game_map[][GAME_MAP_SIZE])
{
    // record the word found in the map
    int word_found_count = 0;

    // push the first nodes, only if they are in dictionary
    for (int row = 0; row < GAME_MAP_SIZE; row++)
        for (int col = 0; col < GAME_MAP_SIZE; col++)
        {
            // as far as I know, one-square will not form a word in the game
            // so any whole word that is in dictionary here will not be printed
            // also, although no need to cut node here
            // but there are only 16 squares, so why not?
            const std::string &word = game_map[row][col];
            if (dictionary_.LookUp(word))
            {
                Node *node = new Node(row, col, word);
                node_stack_.push(node);
            }
        }
    // keep popping, until the stack is empty
    // words will be checked in dictionary when the nodes are popped
    while (!node_stack_.empty())
    {
        // popping the first node in stack
        Node *node = node_stack_.top();
        node_stack_.pop();

        // checking this node
        int word_property = dictionary_.LookUp(node->word_now);
        if (word_property & Dictionary::IS_WORD)  // is a word
        {
            word_found_count++;
            // print solution immediately
            PrintSolution(node);
        }
        if (!(word_property & Dictionary::IS_SUB_WORD))
        {
            // not a sub-word, no need to continue
            // move to next node
            delete node;  // get rid of this node
            continue;
        }

        // is a sub-word, keep finding
        // for every direction
        for (int direction = 0; direction < DIRECTION_NUM; direction++)
        {
            int row = node->currrent_row + direction_row_offset[direction];
            int col = node->currrent_col + direction_col_offset[direction];
            // cut invalid directions
            if (row < 0 || col < 0 ||
                row >= GAME_MAP_SIZE || col >= GAME_MAP_SIZE ||
                node->moved_squares[row][col])
                continue;

            // build new node from old one
            Node *new_node = new Node(node, direction, game_map[row][col]);
            node_stack_.push(new_node);
        }
        delete node;  // get rid of this old node
    }  // end of the while loop
    return word_found_count;
}

void WordamentAI::PrintSolution(const Node *last_node)
{
    const char *map_to_print[GAME_MAP_SIZE][GAME_MAP_SIZE];
    // clear the map to blank
    for (int row = 0; row < GAME_MAP_SIZE; row++)
        for (int col = 0; col < GAME_MAP_SIZE; col++)
        {
            map_to_print[row][col] = ".";
        }

    // add arrows to the map
    int currrent_row = last_node->currrent_row;
    int currrent_col = last_node->currrent_col;
    // print a red ● to the last position
    map_to_print[currrent_row][currrent_col] = "\e[31m●\e[0m";
    const std::vector<int> &moves = last_node->previous_moves;

    // loop until the first position
    for (int i = moves.size() - 1; i > 0; i--)
    {
        int last_move = moves[i];
        // move to the last square
        currrent_row -= direction_row_offset[last_move];
        currrent_col -= direction_col_offset[last_move];
        // prepare to print the arrow here
        map_to_print[currrent_row][currrent_col] = direction_arrow[last_move];
    }

    // the first arrow
    int first_move = moves[0];
    currrent_row -= direction_row_offset[first_move];
    currrent_col -= direction_col_offset[first_move];
    map_to_print[currrent_row][currrent_col] =
            green_direction_arrow[first_move];

    // print it to screen
    for (int row = 0; row < GAME_MAP_SIZE; row++)
    {
        for (int col = 0; col < GAME_MAP_SIZE; col++)
        {
            std::cout << map_to_print[row][col] << " ";
        }
        if (row == GAME_MAP_SIZE - 1)  // last row
            // print the word
            std::cout << "    " << last_node->word_now;
        std::cout << std::endl;
    }
    // print a separator line
    std::cout << std::endl;
}

// functions for Node
WordamentAI::Node::Node(int row, int col, const std::string &word)
        : moved_squares{},
          currrent_row(row),
          currrent_col(col),
          previous_moves(),
          word_now(word)
{
    moved_squares[row][col] = true;
}

WordamentAI::Node::Node(const Node *old_node, int direction,
                        const std::string &add_on_word)
        : currrent_row(old_node->currrent_row +
                       direction_row_offset[direction]),
          currrent_col(old_node->currrent_col +
                       direction_col_offset[direction]),
          previous_moves(old_node->previous_moves),
          word_now(old_node->word_now)
{
    for (int row = 0; row < GAME_MAP_SIZE; row++)
        for (int col = 0; col < GAME_MAP_SIZE; col++)
        {
            moved_squares[row][col] = old_node->moved_squares[row][col];
        }
    moved_squares[currrent_row][currrent_col] = true;
    previous_moves.push_back(direction);  // record direction
    word_now += add_on_word;  // avoid temporary string object
}

}  // namespace wordament_ai
