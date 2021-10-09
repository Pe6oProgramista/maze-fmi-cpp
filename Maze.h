#pragma once
#include <iostream>
#include <exception>

#include <string>
#include <vector>
#include <unordered_set>
#include <unordered_map>
#include <queue>
#include <utility>

#include <fstream>

// for sqrt() if directions are 8
// #include <math.h>

#include "Bitmap.h"

class MazeException : public std::exception {
private:
    const char* msg;
public:
    MazeException(const char* msg) : msg(msg) {}

    const char* what() const throw () {
        return msg;
    }
};

class Maze {
private:
    // Helper structs
    struct Coord {
        size_t row;
        size_t col;

        Coord() : row(-1), col(-1) {}

        Coord(size_t row, size_t col) : row(row), col(col) {}

        bool operator<(const Coord& c) const {
            return row < c.row || (!(c.row < row) && col < c.col);
        }

        bool operator==(const Coord& c) const {
            return row == c.row && col == c.col;
        }

        bool operator!=(const Coord& c) const {
            return !(*this == c);
        }

        Coord operator+(const Coord& c) const {
            return { row + c.row, col + c.col };
        }
    };

    struct Color {
        unsigned char red;
        unsigned char green;
        unsigned char blue;

        Color() : red(0), green(0), blue(0) {}

        Color(unsigned char red, unsigned char green, unsigned char blue) : red(red), green(green), blue(blue) {}

        bool operator==(const Color& c) const {
            return red == c.red && green == c.green && blue == c.blue;
        }

        bool operator!=(const Color& c) const {
            return !(*this == c);
        }

        bool is_grey() const {
            return red == green && green == blue;
        }

        struct Hasher {
            size_t operator()(const Color& c) const noexcept {
                return c.red ^ (c.green << 1) ^ (c.blue << 2);
            }
        };
    };

    class KeyCombination {
    private:
        static const size_t CHAR_BITS = 8 * sizeof(char);

        std::vector<char> comb_bits;

        KeyCombination(const std::vector<char>& comb_bits) : comb_bits(comb_bits) {}

    public:

        KeyCombination() {
            comb_bits.push_back(0);
        }

        KeyCombination(size_t pos) {
            size_t indx = pos / CHAR_BITS;
            if (indx >= comb_bits.size()) {
                comb_bits.resize(indx + 1, 0);
            }

            comb_bits[indx] |= (1 << ((CHAR_BITS - pos % CHAR_BITS) - 1));
        }

        KeyCombination set_at(size_t pos) const {
            KeyCombination key_comb = KeyCombination(comb_bits);

            size_t indx = pos / CHAR_BITS;
            if (indx >= key_comb.comb_bits.size()) {
                key_comb.comb_bits.resize(indx + 1, 0);
            }

            key_comb.comb_bits[indx] |= (1 << ((CHAR_BITS - pos % CHAR_BITS) - 1)); // vec[i/8] |= ( 1 << ((8 - i % 8 ) - 1) )
            return key_comb;
        }

        KeyCombination unset_at(size_t pos) const {
            KeyCombination key_comb = KeyCombination(comb_bits);

            size_t indx = pos / CHAR_BITS;
            if (indx >= key_comb.comb_bits.size()) {
                key_comb.comb_bits.resize(indx + 1, 0);
            }

            key_comb.comb_bits[indx] &= ~(1 << ((CHAR_BITS - pos % CHAR_BITS) - 1));
            return key_comb;
        }

        bool operator==(const KeyCombination& key_comb) const {
            return comb_bits == key_comb.comb_bits;
        }

        bool operator!=(const KeyCombination& key_comb) const {
            return !(*this == key_comb);
        }

        struct Hasher {
            size_t operator()(const KeyCombination& key_comb) const noexcept {
                size_t seed = 0;
                for (char kb : key_comb.comb_bits) {
                    seed = 31 * seed + std::hash<char>()(kb);
                }
                return seed;
            }
        };
    };

    struct Pixel {
        struct umap_iterator_hasher {
            size_t operator()(const std::unordered_set<KeyCombination, KeyCombination::Hasher>::iterator& i) const noexcept {
                return std::hash<const KeyCombination*>()(&*i);
            }
        };

        enum class Type {
            UNSET,
            WALL,
            FREE,
            KEY,
            ZONE,
            START,
            END
        };

        Maze::Color color;
        Type type;
        std::unordered_map<std::unordered_set<KeyCombination, KeyCombination::Hasher>::iterator, size_t, umap_iterator_hasher> key_dists; // key is the indx of keys combination, value - distance from this combination

        Pixel() : type(Type::UNSET) {}

    };


    // Fields
    static const size_t MAX_DIST = -1;
    static const size_t KEY_WIDTH = 20;
    static const size_t KEY_HEIGHT = 20;
    static const Color WALL_COLOR;
    static const Color START_COLOR;
    static const Color END_COLOR;
    static const Color PATH_COLOR;
    static const KeyCombination START_KEY_COMB;

    size_t width, height;

    std::vector<Coord> ends;
    std::unordered_map<Color, size_t, Color::Hasher> keys; // color and indx
    std::unordered_set<KeyCombination, KeyCombination::Hasher> key_combs;
    std::vector<Pixel> pixels;

    bool is_valid(const Coord& c) const;

    size_t pixel_indx(const Coord& c) const;

    Pixel& pixel_at(const Coord& c);

    const Pixel& pixel_at(const Coord& c) const;

    Color bmp_color_at(const Bitmap_Image& bmp_img, const Coord& c);

    void bmp_set_color_at(Bitmap_Image& bmp_img, const Coord& c, const Color& clr);

    void print_pxl(const Coord& c) const;

    void print_coord(const Coord& c) const;

    bool is_valid_map() const;

    Coord get_start() const;

    void set_area_at(const Coord& c);

public:
    Maze() : width(0), height(0) {}

    Maze(const Bitmap_Image& bmp_img);

    void from_bmp(const std::string& filename);

    void from_bmp(const Bitmap_Image& bmp_img);

    void find_path();

    void write_points(const std::vector<Coord>& path);

    void save_path(Bitmap_Image& bmp_img);
};

