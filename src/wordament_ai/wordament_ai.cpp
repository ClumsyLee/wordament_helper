#include "wordament_ai/wordament_ai.h"
#include <cstdlib>
#include <algorithm>
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

    bool operator<(const Node &node) const { return score < node.score; }

    bool moved_squares[GAME_MAP_SIZE][GAME_MAP_SIZE];
    int currrent_row;
    int currrent_col;
    std::vector<int> previous_moves;
    std::string word_now;
    int score;
};


// functions for WordamentAI
WordamentAI::WordamentAI(const std::vector<std::string> &dictionary_files)
        : dictionary_(),
          dictionary_word_count_(0)
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
    std::stack<Node *> node_stack;
    // save the solution nodes and the words
    std::vector<Node> solution_nodes;
    std::unordered_set<std::string> words_found;

    // push the first nodes, only if they are in dictionary
    for (int row = 0; row < GAME_MAP_SIZE; row++)
        for (int col = 0; col < GAME_MAP_SIZE; col++)
        {
            // create new node so it will deal with last '-' in prefix
            Node *node = new Node(row, col, game_map[row][col]);
            node_stack.push(node);
        }
    // keep popping, until the stack is empty
    // words will be checked in dictionary when the nodes are popped
    while (!node_stack.empty())
    {
        // popping the first node in stack
        Node *node = node_stack.top();
        node_stack.pop();

        // checking this node
        int word_property = dictionary_.LookUp(node->word_now);
        if (word_property & Dictionary::IS_WORD)  // is a word
        {
            // check in the found list
            auto insert_result = words_found.insert(node->word_now);
            if (insert_result.second == true)  // found a new word
            {
                // adjust the score, and then save the solution.
                // node might be used later, so do not change it directly.
                Node word_node = *node;
                AdjustScore(&word_node);
                solution_nodes.push_back(word_node);
            }
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
            // cut invalid directions:
            // 1. out of bound
            // 2. already taken
            // 3. is a prefix
            if (row < 0 || col < 0 ||
                row >= GAME_MAP_SIZE || col >= GAME_MAP_SIZE ||
                node->moved_squares[row][col] ||
                IsPrefix(game_map[row][col]))
                continue;

            // build new node from old one
            Node *new_node = new Node(node, direction, game_map[row][col]);
            node_stack.push(new_node);
        }
        delete node;  // get rid of this old node
    }  // end of the while loop

    // sort the nodes
    std::sort(solution_nodes.begin(), solution_nodes.end());
    // print the nodes in order
    for (const Node &node : solution_nodes)
    {
        PrintSolution(&node);
    }
    return words_found.size();
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

    // as far as I know, one-square will not form a word in the game.
    // however, it's better to double check, for something like "pre".
    // so check before drawing the first arrow.
    if (moves.size() > 0)
    {
        int first_move = moves[0];
        currrent_row -= direction_row_offset[first_move];
        currrent_col -= direction_col_offset[first_move];
        map_to_print[currrent_row][currrent_col] =
                green_direction_arrow[first_move];
    }

    // print it to screen
    for (int row = 0; row < GAME_MAP_SIZE; row++)
    {
        for (int col = 0; col < GAME_MAP_SIZE; col++)
        {
            std::cout << map_to_print[row][col] << " ";
        }
        if (row == GAME_MAP_SIZE - 1)  // last row
        {
            // print the word and score
            std::cout << "    ";
            std::cout.width(12);
            std::cout << std::left << last_node->word_now
                      << " " << last_node->score;
        }
        std::cout << std::endl;
    }
    // print a separator line
    std::cout << std::endl;
}


int WordamentAI::CharactorScore(const std::string &char_string)
{
    static const int charactor_score[26] = {
        2, 5, 3, 3, 1, 5, 4, 4, 2, 10,
        6, 3, 4, 2, 2, 4, 0, 2, 2, 2,
        4, 6, 6, 0, 5, 8
    };
    char first_charactor = char_string[0] - 'a';
    if (first_charactor < 0 || first_charactor >= 26)
        return 0;
    return charactor_score[static_cast<int>(first_charactor)];
}

void WordamentAI::AdjustScore(Node *node)
{
    int size = node->word_now.size();
    if (size >= 4)  // need to adjust
    {
        if (size <= 5)  // x1.5
            node->score = node->score * 3 / 2;
        else if (size <= 7)  // x2
            node->score *= 2;
        else  // x3
            node->score *= 3;
    }
}


// functions for Node
WordamentAI::Node::Node(int row, int col, const std::string &word)
        : moved_squares{},
          currrent_row(row),
          currrent_col(col),
          previous_moves(),
          word_now(word),
          score(CharactorScore(word))
{
    // prefix can only be added by this function (brand new node)
    if (IsPrefix(word))
    {
        word_now.pop_back();  // delete the last '-'
    }
    moved_squares[row][col] = true;
}

WordamentAI::Node::Node(const Node *old_node, int direction,
                        const std::string &add_on_word)
        : currrent_row(old_node->currrent_row +
                       direction_row_offset[direction]),
          currrent_col(old_node->currrent_col +
                       direction_col_offset[direction]),
          previous_moves(old_node->previous_moves),
          word_now(old_node->word_now),
          score(old_node->score + CharactorScore(add_on_word))
{
    // copy the old status
    for (int row = 0; row < GAME_MAP_SIZE; row++)
        for (int col = 0; col < GAME_MAP_SIZE; col++)
        {
            moved_squares[row][col] = old_node->moved_squares[row][col];
        }
    // update status
    moved_squares[currrent_row][currrent_col] = true;
    previous_moves.push_back(direction);  // record direction
    word_now += add_on_word;  // avoid temporary string object
}

}  // namespace wordament_ai
