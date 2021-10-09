#include "Bitmap.h"

/*
Bitmap_Image::Bitmap_Image(const std::string& filename) {
    load_file(filename);
}

std::string Bitmap_Image::get_name() const {
    return full_name.substr(0, full_name.find("."));
}

const Bitmap_Image::DIB_Header& Bitmap_Image::get_dib_header() const {
    return dib_header;
}

const std::vector<unsigned char>& Bitmap_Image::get_color_table() const {
    return color_table;
}

std::vector<unsigned char>& Bitmap_Image::get_color_table() {
    return color_table;
}

bool Bitmap_Image::load_file(const std::string& filename) {
    std::ifstream bmp_file(filename, std::ios::binary);

    bmp_file.read(headers, sizeof(headers));
    dib_header.width = *(int*)&headers[BMP_WIDTH_OFFSET];
    dib_header.height = *(int*)&headers[BMP_HEIGHT_OFFSET];
    dib_header.bits_per_pixel = *(int*)&headers[BMP_BPP_OFFSET];

    uint16_t bytes_per_pixel = dib_header.bits_per_pixel / 8;

    color_table.resize(dib_header.height * dib_header.width * bytes_per_pixel);

    size_t row_pixels_bytes = dib_header.width * bytes_per_pixel;
    size_t row_padding = BMP_MAX_BYTES_PP - (row_pixels_bytes - 1) % BMP_MAX_BYTES_PP - 1;

    for (int i = dib_header.height - 1; i >= 0; i--) {
        bmp_file.read((char*)(color_table.data() + i * row_pixels_bytes), row_pixels_bytes);
        bmp_file.seekg(row_padding, bmp_file.cur);
    }

    full_name = filename;

    return true;
}

bool Bitmap_Image::save_file() {
    std::ofstream bmp_file(get_name().append("_res.bmp"), std::ios::trunc | std::ios::binary);

    bmp_file.write(headers, sizeof(headers));

    size_t row_pixels_bytes = dib_header.width * dib_header.bits_per_pixel / 8;
    size_t row_padding = BMP_MAX_BYTES_PP - (row_pixels_bytes - 1) % BMP_MAX_BYTES_PP - 1;

    std::vector<char> padding(row_padding, 0);
    for (int i = dib_header.height - 1; i >= 0; i--) {
        bmp_file.write((char*)(color_table.data() + i * row_pixels_bytes), row_pixels_bytes);
        bmp_file.write(padding.data(), row_padding);
    }

    return true;
}*/




Bitmap_Image::Bitmap_Image(const std::string& filename) {
    load_file(filename);
}

std::string Bitmap_Image::get_name() const {
    return full_name.substr(0, full_name.rfind("."));
}

const Bitmap_Image::BMP_File_Header& Bitmap_Image::get_bmp_header() const {
    return bmp_header;
}

const Bitmap_Image::DIB_Header& Bitmap_Image::get_dib_header() const {
    return dib_header;
}

const std::vector<unsigned char>& Bitmap_Image::get_color_table() const {
    return color_table;
}

std::vector<unsigned char>& Bitmap_Image::get_color_table() {
    return color_table;
}

void Bitmap_Image::load_file(const std::string& filename) {
    std::ifstream bmp_file(filename, std::ios::binary);

    if (!bmp_file) throw BitmapException("Fail to open file.");

    // read headers
    bmp_file.read((char*)&bmp_header, sizeof(bmp_header));

    if (bmp_header.signature != 0x4D42) {
        throw BitmapException("Invalid file signature.");
    }

    bmp_file.read((char*)&dib_header, sizeof(dib_header));
    if (dib_header.compression == BI_BITFIELDS || dib_header.compression == BI_ALPHABITFIELDS && dib_header.bits_per_pixel == 32) {
        bmp_file.read((char*)&bit_mask_header, sizeof(bit_mask_header));
    }

    size_t bytes_per_pixel = dib_header.bits_per_pixel / 8;

    color_table.resize((size_t)dib_header.height * (size_t)dib_header.width * bytes_per_pixel);

    size_t row_pixels_bytes = dib_header.width * bytes_per_pixel;
    size_t row_padding = BMP_MAX_BYTES_PP - (row_pixels_bytes - 1) % BMP_MAX_BYTES_PP - 1;

    for (int i = dib_header.height - 1; i >= 0; i--) {
        bmp_file.read((char*)(color_table.data() + i * row_pixels_bytes), row_pixels_bytes);
        bmp_file.seekg(row_padding, bmp_file.cur);
    }

    full_name = filename;
}

bool Bitmap_Image::save_file() {
    std::cout << "Saving file...";

    std::ofstream bmp_file(get_name().append("_res.bmp"), std::ios::trunc | std::ios::binary);

    if (!bmp_file) return false;

    // write headers
    bmp_file.write((char*)&bmp_header, sizeof(bmp_header));
    bmp_file.write((char*)&dib_header, sizeof(dib_header));
    if (dib_header.compression == BI_BITFIELDS || dib_header.compression == BI_ALPHABITFIELDS && dib_header.bits_per_pixel == 32) {
        bmp_file.write((char*)&bit_mask_header, sizeof(bit_mask_header));
    }

    size_t row_pixels_bytes = dib_header.width * dib_header.bits_per_pixel / 8;
    size_t row_padding = BMP_MAX_BYTES_PP - (row_pixels_bytes - 1) % BMP_MAX_BYTES_PP - 1;

    std::vector<char> padding(row_padding, 0);
    for (int i = dib_header.height - 1; i >= 0; i--) {
        bmp_file.write((char*)(color_table.data() + i * row_pixels_bytes), row_pixels_bytes);
        bmp_file.write(padding.data(), row_padding);
    }

    std::cout << "File saved!";
    return true;
}