# Wordament AI

[Wordament™](http://wordament.com) is a unique kind of word game—a word tournament—where you are
competing with the whole internet to be the best word searcher in every game.

And Wordament AI is a tool to help you (or, me) to get a "better" score in the game.

In fact, it is hardly an AI, because it simply uses depth-first search to find words in a dictionary.

At present it only supports Unix-like systems (Linux, OS X, etc.).

## Features

* Short responsing time.
* Visual solutions displayed in color.
* Sort the solutions by thire scores.

## Installation

Just move to the directory and make:

    cd wordament_ai/
    make

You may need to change something in Makefile.

## Run

    bin/solver <dictionary files> ...
    
You may just use the word list in `dicts` directory:

    bin/solver dicts/wordsEn.txt

## Is That ***Cheating***?

Well, not if you do ***not*** use it to play online.
