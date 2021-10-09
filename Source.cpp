#include <iostream>
#include <exception>

#include <string>
#include <vector>
#include <unordered_set>
#include <unordered_map>
#include <queue>
#include <utility>

#include <fstream>

const std::string FILE_NAME = "bmp_maze_examples\\00.input20x20.bmp"; // "input20x20v2.bmp";

#include "Bitmap.h"
#include "Maze.h"

int main() {
    try {
        std::string file_name;
        std::cout << "Input file name: ";
        //std::cin >> file_name;

        Bitmap_Image img(FILE_NAME);
        Maze maze;
        maze.from_bmp(img);
        maze.find_path();
        maze.save_path(img);
    }
    catch (BitmapException& e) {
        std::cout << e.what() << "\n";
    }
    catch (MazeException& e) {
        std::cout << e.what() << "\n";
    }

    return 0;
}
