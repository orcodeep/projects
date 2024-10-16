# Title:- SNEK

This is the first game i have made using c. Its a game that
I used to play on my grandma's Nokia when I was Little. 

## IMPORTANT NOTE:-

If you want to run this game, make sure you adjust(reduce) the terminal's line height(spacing b/w 2 lines) to be 
equal / nearly equal to the spacing b/w two characters on the same line according to your font and font size. Or, the 
snake wont have the same speed both vertically and horizontally & there will be gaps b/w the segments when moving vertically.

Watch:-
[![Watch the video](https://img.youtube.com/vi/opSWrpPkYgE/0.jpg)](https://youtu.be/opSWrpPkYgE)


## Prerequisites

Make sure you have the following installed:

- GCC 
- `ncurses` library

Then simply run: 'make' or do 'gcc -o snake snake.c -lncurses' then run the executable.

### Installing ncurses

#### On Ubuntu/Debian:
```bash
sudo apt-get install libncurses5-dev
