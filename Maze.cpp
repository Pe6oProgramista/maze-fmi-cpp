#include "Maze.h"

const Maze::Color Maze::WALL_COLOR = Maze::Color(0, 0, 0);
const Maze::Color Maze::START_COLOR = Maze::Color(195, 195, 196);
const Maze::Color Maze::END_COLOR = Maze::Color(126, 127, 127);
const Maze::Color Maze::PATH_COLOR = Maze::Color(255, 0, 0);
const Maze::KeyCombination Maze::START_KEY_COMB = Maze::KeyCombination();

bool Maze::is_valid(const Coord& c) const {
    return c.row < height&& c.col < width;
}

size_t Maze::pixel_indx(const Coord& c) const {
    if (!is_valid(c)) {
        throw MazeException("ERROR: Coords out of range.");
    }
    return c.row * width + c.col;
}

Maze::Pixel& Maze::pixel_at(const Coord& c) {
    return pixels[pixel_indx(c)];
}

const Maze::Pixel& Maze::pixel_at(const Coord& c) const {
    return pixels[pixel_indx(c)];
}

Maze::Color Maze::bmp_color_at(const Bitmap_Image& bmp_img, const Coord& c) {
    if (!is_valid(c)) {
        throw MazeException("ERROR: Coords out of range.");
    }

    size_t indx = (c.row * width + c.col) * bmp_img.get_dib_header().bits_per_pixel / 8;
    const std::vector<unsigned char>& color_table = bmp_img.get_color_table();

    return { color_table[indx + 2], color_table[indx + 1], color_table[indx] };
}

void Maze::bmp_set_color_at(Bitmap_Image& bmp_img, const Coord& c, const Color& clr) {
    if (!is_valid(c)) {
        throw MazeException("ERROR: Coords out of range.");
    }

    size_t indx = (c.row * width + c.col) * bmp_img.get_dib_header().bits_per_pixel / 8;
    std::vector<unsigned char>& color_table = bmp_img.get_color_table();

    color_table[indx + 2] = clr.red;
    color_table[indx + 1] = clr.green;
    color_table[indx] = clr.blue;
}

void Maze::print_pxl(const Coord& c) const {
    if (!is_valid(c)) {
        throw MazeException("ERROR: Coords out of range.");
    }

    Color clr = pixels[pixel_indx(c)].color;
    std::cout << "(" << +clr.red << "," << +clr.green << "," << +clr.blue << ")\n";
}

void Maze::print_coord(const Coord& c) const {
    if (!is_valid(c)) {
        throw MazeException("ERROR: Coords out of range.");
    }

    std::cout << "(" << c.row << "," << c.col << ")\n";
}

bool Maze::is_valid_map() const {
    return true;
}

Maze::Coord Maze::get_start() const {
    for (size_t i = 0; i < height; i++) {
        for (size_t j = 0; j < width; j++) {
            Coord curr(i, j);
            if (pixel_at(curr).color == START_COLOR) {
                return curr;
            }
        }
    }

    throw MazeException("ERROR: There is no start.");
}

void Maze::set_area_at(const Coord& c) {
    Pixel& pxl = pixel_at(c);

    if (pxl.color == WALL_COLOR) {
        pxl.type = Pixel::Type::WALL;
    }
    else if (pxl.color.is_grey()) {
        pxl.type = Pixel::Type::FREE;
    }
    else {
        Pixel::Type type = Pixel::Type::ZONE;
        if (pxl.color == START_COLOR) type = Pixel::Type::START;
        if (pxl.color == END_COLOR) type = Pixel::Type::END;

        pxl.type = type;

        std::vector<Pixel*> key_pixels;
        key_pixels.reserve(KEY_HEIGHT * KEY_WIDTH);
        size_t max_height, min_height, max_width, min_width;
        max_height = min_height = c.row;
        max_width = min_width = c.col;

        std::queue<Coord> wave;
        wave.push(c);
        key_pixels.push_back(&pxl);

        while (!wave.empty()) {
            Coord curr = wave.front();
            wave.pop();

            Pixel& curr_pxl = pixel_at(curr);

            for (int i = -1; i < 2; i++) {
                for (int j = -1; j < 2; j++) {
                    if ((i == 0 && j == 0) || (i != 0 && j != 0)) continue;

                    // взимаме съседа на текущия пиксел и го добавяме ако е със същия цвят
                    Coord nb = curr + Coord(i, j);
                    try {
                        Pixel& nb_pxl = pixel_at(nb);
                        if (nb_pxl.color != curr_pxl.color || nb_pxl.type != Pixel::Type::UNSET) continue;

                        nb_pxl.type = type;

                        wave.push(nb);
                        if (max_height < nb.row) max_height = nb.row;
                        if (min_height > nb.row) min_height = nb.row;
                        if (max_width < nb.col) max_width = nb.col;
                        if (min_width > nb.col) min_width = nb.col;

                        if (type == Pixel::Type::ZONE &&
                            max_height - min_height + 1 <= KEY_HEIGHT &&
                            max_width - min_width + 1 <= KEY_WIDTH &&
                            key_pixels.size() < KEY_HEIGHT * KEY_WIDTH)
                        {
                            key_pixels.push_back(&nb_pxl);
                        }
                        else if (key_pixels.size() != 0) {
                            key_pixels.clear();
                        }
                    }
                    catch (MazeException& e) {
                        continue;
                    };
                }
            }
        }

        if (type == Pixel::Type::ZONE &&
            max_height - min_height + 1 == KEY_HEIGHT &&
            max_width - min_width + 1 == KEY_WIDTH &&
            key_pixels.size() == KEY_HEIGHT * KEY_WIDTH)
        {
            for (std::vector<Pixel*>::iterator it = key_pixels.begin(); it != key_pixels.end(); it++) {
                (*it)->type = Pixel::Type::KEY;
            }
        }
    }
}

Maze::Maze(const Bitmap_Image& bmp_img) {
    from_bmp(bmp_img);
}

void Maze::from_bmp(const std::string& filename) {
    from_bmp(Bitmap_Image(filename));
}

void Maze::from_bmp(const Bitmap_Image& bmp_img) {
    ends.clear();
    keys.clear();
    key_combs.clear();
    pixels.clear();

    width = bmp_img.get_dib_header().width;
    height = bmp_img.get_dib_header().height;

    pixels.resize(width * height);

    for (size_t i = 0; i < height; i++) {
        for (size_t j = 0; j < width; j++) {
            Coord coord = { i, j };
            pixel_at(coord).color = bmp_color_at(bmp_img, coord);
        }
    }
}

void Maze::find_path() {
    Coord start = get_start();
    set_area_at(start);
    key_combs.insert(KeyCombination());
    pixel_at(start).key_dists[key_combs.begin()] = 0;

    using KeyCombIter = std::unordered_set<KeyCombination, KeyCombination::Hasher>::iterator;
    using CoordComb = std::pair<Coord, KeyCombIter>;

    std::queue<CoordComb> wave;
    wave.push(CoordComb(start, key_combs.begin()));

    while (!wave.empty()) {
        CoordComb curr = wave.front();
        wave.pop();

        Pixel& pxl = pixel_at(curr.first);

        // взимаме дистанцията от текущия пиксел със текущата комбинация от ключове
        std::unordered_map<KeyCombIter, size_t, Pixel::umap_iterator_hasher>::iterator curr_dist_it = pxl.key_dists.find(curr.second);
        if (curr_dist_it == pxl.key_dists.end()) {
            throw MazeException("ERROR: current pixel doesn't have current combination.");
        }

        // UL U UR  L R  DL D DR
        for (int i = -1; i < 2; i++) {
            for (int j = -1; j < 2; j++) {
                // пропускаме текущия пиксел и диагоналните му съседи
                if ((i == 0 && j == 0) || (i != 0 && j != 0)) continue;

                try {
                    // взимаме съседа на текущия пиксел
                    Coord nb = curr.first + Coord(i, j);
                    Pixel& nb_pxl = pixel_at(nb);
                    if (nb_pxl.type == Pixel::Type::UNSET) {
                        set_area_at(nb);
                        if (nb_pxl.type == Pixel::Type::END) {
                            ends.push_back(nb);
                        }
                    }

                    // ако е стена я пропускаме
                    if (nb_pxl.type == Pixel::Type::WALL) continue;

                    // изчисляваме цената за преминаване в съседа
                    size_t weight = nb_pxl.color.is_grey() ? nb_pxl.color.red : 1;
                    // if(i != 0 && j != 0) weight *= sqrt(2);

                    // ако новият пиксел е цветен:
                    //  - ако е ключ - добавяме го (ако вече не е добавен)
                    //	- ако не е ключ - проверяваме дали има ключ с такъв цвят и дали текущата комбинация съдържа този цвят
                    //	  ако не го съдържа - отиваме към следващия съсед
                    //	  ако го съдържа - минаваме през него и изчисляваме новата цена
                    // ако не е цветен -  минаваме през него и изчисляваме новата цена
                    KeyCombIter new_key_comb = curr.second;
                    if (nb_pxl.type == Pixel::Type::KEY) {
                        if (keys.find(nb_pxl.color) == keys.end()) {
                            size_t pos = keys.size();
                            keys[nb_pxl.color] = pos; // keys.size() - 1;
                        }

                        KeyCombination key_comb = curr.second->set_at(keys[nb_pxl.color]);
                        new_key_comb = key_combs.insert(key_comb).first;

                    }
                    else if (nb_pxl.type == Pixel::Type::ZONE) {
                        if (keys.find(nb_pxl.color) == keys.end()) continue;

                        KeyCombination key_comb = curr.second->set_at(keys[nb_pxl.color]);
                        if (*curr.second != key_comb) continue;
                    }

                    // ако съседния пиксел няма разстояние със новата комбинация или старото такова е по голямо от новото
                    // тогава актуализираме разстоянието
                    if (nb_pxl.key_dists.find(new_key_comb) == nb_pxl.key_dists.end() ||
                        nb_pxl.key_dists[new_key_comb] > (curr_dist_it->second + weight))
                    {
                        nb_pxl.key_dists[new_key_comb] = curr_dist_it->second + weight;
                        wave.push(CoordComb(nb, new_key_comb));
                    }
                }
                catch (MazeException& e) {
                    continue;
                };
            }
        }
    }
}

void Maze::write_points(const std::vector<Coord>& path) {
    std::ofstream out_file("output.txt", std::ios::trunc);

    bool horizontal = false;

    if (path.size() != 0) {
        out_file << path[0].row << " " << path[0].col << "\n";
    }

    for (size_t i = 1; i < path.size(); i++) {
        if (i == path.size() - 1)  out_file << path[i].row << " " << path[i].col << "\n";

        if (horizontal && path[i].row != path[i - 1].row) {
            if (i + 1 != path.size() && path[i + 1].row != path[i - 1].row) {
                out_file << path[i].row << " " << path[i].col << "\n";
                horizontal = !horizontal;
            }
        }
        if (!horizontal && path[i].col != path[i - 1].col) {
            if (i + 1 != path.size() && path[i + 1].col != path[i - 1].col) {
                out_file << path[i].row << " " << path[i].col << "\n";
                horizontal = !horizontal;
            }
        }
    }
}

void Maze::save_path(Bitmap_Image& bmp_img) {
    using KeyCombIter = std::unordered_set<KeyCombination, KeyCombination::Hasher>::iterator;
    using PixelDists = std::unordered_map<KeyCombIter, size_t, Pixel::umap_iterator_hasher>::iterator;

    if (ends.empty()) {
        std::ofstream out_file("output.txt", std::ios::trunc);
        out_file << "no solution";
        out_file.close();
        std::cout << "There is no path.\n";
        return;
    }

    std::vector<Coord> path;

    // save only the shortest path (not all paths)
    // Coord curr = ends[0];

    // save paths to every end
    for (std::vector<Coord>::iterator end = ends.begin(); end != ends.end(); end++) {
        Coord curr = *end;
        bmp_set_color_at(bmp_img, curr, PATH_COLOR);
        KeyCombIter key_comb;
        while (true) {
            Pixel& p = pixel_at(curr);

            if (p.type == Pixel::Type::START && *key_comb == START_KEY_COMB) break;

            if (p.type == Pixel::Type::END) {
                size_t min_dist = MAX_DIST;
                for (PixelDists it = p.key_dists.begin(); it != p.key_dists.end(); it++) {
                    if (it->second < min_dist) {
                        key_comb = it->first;
                        min_dist = it->second;
                    }
                }
            }

            // намираме съседа с минимална дистанция от тази комбинация
            // ако сме в ключ с комбинация, която той няма, значи сме излезли от него и сме с 1 комбинация назад
            // тогава цената на следващия пиксел с новата комбинация не зависи от тази на ключа(приемаме я за MAX_DIST)
            Coord next = curr;
            size_t min_dist = MAX_DIST;
            if (p.type == Pixel::Type::KEY && p.key_dists.find(key_comb) != p.key_dists.end()) {
                min_dist = p.key_dists[key_comb];
            }
            // UL U UR  L R  DL D DR
            for (int i = -1; i < 2; i++) {
                for (int j = -1; j < 2; j++) {
                    if ((i == 0 && j == 0) || (i != 0 && j != 0)) continue;

                    try {
                        // взимаме съседния пиксел на текущия пиксел
                        Coord nb = curr + Coord(i, j);
                        Pixel& nb_pxl = pixel_at(nb);

                        // ако съседния пиксел има цена с текущата комбинация го обработваме
                        PixelDists it = nb_pxl.key_dists.find(key_comb);
                        if (it == nb_pxl.key_dists.end()) continue;

                        if (it->second < min_dist) {
                            next = nb;
                            min_dist = it->second;
                        }
                    }
                    catch (MazeException& e) { continue; }
                }
            }

            // Ако няма съсед с по-малка дистанция:
            //  - ако сме в ключ тогава махаме цвета на ключа от комбинацията и проверяваме тогава съседите
            //  - ако сме в поле различно от ключ значи пряк няма път
            if (next == curr) {
                if (p.type == Pixel::Type::KEY) {
                    KeyCombination kb = key_comb->unset_at(keys[p.color]);
                    key_comb = key_combs.find(kb);
                }
                else {
                    throw MazeException("ERROR: There is no path, but ends[] is not empty.");
                }
            }
            else {
                path.push_back(next);
                bmp_set_color_at(bmp_img, next, PATH_COLOR);
                curr = next;
            }
        }
    }

    write_points(path);
    bmp_img.save_file();
}