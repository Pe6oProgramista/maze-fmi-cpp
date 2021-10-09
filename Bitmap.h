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

class BitmapException : public std::exception {
private:
    const char* msg;
public:
    BitmapException(const char* msg) : msg(msg) {}

    const char* what() const throw () {
        return msg;
    }
};

/*
class Bitmap_Image {
private:
    static const size_t BMP_HEADER_OFFSET = 54;
    static const size_t BMP_WIDTH_OFFSET = 18;
    static const size_t BMP_HEIGHT_OFFSET = 22;
    static const size_t BMP_BPP_OFFSET = 28;
    static const size_t BMP_MAX_BYTES_PP = 4;

    struct DIB_Header {
        uint32_t width;
        uint32_t height;
        uint16_t bits_per_pixel;
    };

    std::string full_name;

    char headers[BMP_HEADER_OFFSET];

    DIB_Header dib_header;
    std::vector<unsigned char> color_table;

public:
    Bitmap_Image(const std::string& filename);

    Bitmap_Image(const Bitmap_Image& bmp_img) = default;

    std::string get_name() const;

    const DIB_Header& get_dib_header() const;

    const std::vector<unsigned char>& get_color_table() const;

    std::vector<unsigned char>& get_color_table();

    bool load_file(const std::string& filename);

    bool save_file();
};*/



class Bitmap_Image {
private:
    static const size_t BMP_HEADER_OFFSET = 54;
    static const size_t BMP_WIDTH_OFFSET = 18;
    static const size_t BMP_HEIGHT_OFFSET = 22;
    static const size_t BMP_BPP_OFFSET = 28;
    static const size_t BMP_MAX_BYTES_PP = 4;

    // compression types
    static const uint32_t BI_RGB = 0;
    static const uint32_t BI_RLE8 = 1;
    static const uint32_t BI_RLE4 = 2;
    static const uint32_t BI_BITFIELDS = 3;
    static const uint32_t BI_JPEG = 4;
    static const uint32_t BI_PNG = 5;
    static const uint32_t BI_ALPHABITFIELDS = 6;
    static const uint32_t BI_CMYK = 11;
    static const uint32_t BI_CMYKRLE8 = 12;
    static const uint32_t BI_CMYKRLE4 = 13;

#pragma pack(push, 1)
    struct BMP_File_Header {
        uint16_t signature;
        uint32_t file_size;
        uint32_t reserved;
        uint32_t offset;
    };
#pragma pack(pop)

#pragma pack(push, 1)
    struct DIB_Header {
        uint32_t size;
        uint32_t width;
        uint32_t height;
        uint16_t planes;
        uint16_t bits_per_pixel;
        uint32_t compression;
        uint32_t image_size;
        uint32_t horizontal_res;
        uint32_t vertical_res;
        uint32_t num_colors;
        uint32_t important_colors;
    };
#pragma pack(pop)

#pragma pack(push, 1)
    struct Bit_Mask_Header {

    };
#pragma pack(pop)

    std::string full_name;

    BMP_File_Header bmp_header;
    DIB_Header dib_header;
    Bit_Mask_Header bit_mask_header;

    std::vector<unsigned char> color_table;

public:
    Bitmap_Image(const std::string& filename);

    Bitmap_Image(const Bitmap_Image& bmp_img) = default;

    std::string get_name() const;

    const BMP_File_Header& get_bmp_header() const;

    const DIB_Header& get_dib_header() const;

    const std::vector<unsigned char>& get_color_table() const;

    std::vector<unsigned char>& get_color_table();

    void load_file(const std::string& filename);

    bool save_file();
};