/*
    Copyright 2024 Shaoheng Guan sg35@sanger.ac.uk, Wellcome Sanger Institute
*/


#ifndef SRC_INCLUDE_PRETEXT_PARSING_PRETEXT_PARSER_H_
#define SRC_INCLUDE_PRETEXT_PARSING_PRETEXT_PARSER_H_

#include <libdeflate/libdeflate.h>
#include <glfw/deps/glad/glad.h>
#include <GLFW/glfw3.h>
#include <opengl_shader/shader_s.h>
#include <iostream>
#include <iomanip>
#include <vector>
#include <string>
#include <sstream>
#include <random>
#include <memory>
#include <thread>
#include <unordered_map>


#define global_function static
#define global_variable static
#define ForLoop(x) for (u32 index= 0 ; index < x; index++)
#define ForLoop1(x) for (u32 index1= 0 ; index1 < x; index1++)
#define ForLoop2(x) for (u32 index2= 0 ; index2 < x; index2++)
#define Max_Num_of_Fragments 4096
#define ArrayLenCount(array) (sizeof(array) / sizeof(array[0]))
#define width__ 1080
#define height__ 1080
#define Min(x, y) (x < y) ? x : y
#define Max(x, y) (x > y) ? x : y


typedef uint8_t u08;
typedef unsigned int u32;
typedef uint64_t u64;
typedef float f32;
typedef double f64;
typedef int32_t s32;
typedef int64_t s64;

class GenomeData;

void glfw_init();
GenomeData* load_file(const char* file_path, u32 wanted_resolution = 1024);
void Update_Contigs_from_MapState(GenomeData* genome);
void printCentered(
    const std::string& text,
    int width,
    char pad_char = ' ');
void gen_random_u32(size_t size, u32 min_, u32 max_, u32* random_arr);
void generate_test_file(GenomeData* genome, std::string save_dir = "");

void print_star_line(u32 len = 50);
void texture_show(GenomeData* genome, u32 texture_to_see = 0);



struct tex_vertex {
    f32 x, y, pad;
    f32 s, t, u;
};


struct Graph{
 public:
    explicit Graph(u32 size) :size(size) {
        data.resize(size);
        name.reserve(64);
    }
    ~Graph() {}
    u32 size;
    std::string name;  // 64 bytes
    std::vector<u32> data;
};


struct
OriginalContig {
    char* name;
    u32 n_fragments;
    u32 pad;
    f32 contig_fracs;
    u32* contig_map_pixels;  // 有效长度为n_fragments, 总长度为4096
};

/* 这是在操作后的所有的片段，contig没有被打断的话这个片段依然是contig，
打断了的话就是新的片段，例如一个cotig被打断成两个新的片段 */
struct Fragment {
    u64 meta_data_flags;
    u32 original_contig_id;
    u32 length;
    u32 start_coord;
    u32 scaff_id;
    u32 pad;
    bool inverted_flag;
};


/* 用于标记每一个像素点 指针空间长度为像素点的长度 */
struct Map_State {
    u32* contigs_ids;
    u32* original_contigs_ids;
    u32* scaff_ids;
    u32* contig_local_coords;  // 局部坐标（在每个contig中的坐标）对于original contig
    u64* meta_data_flags;
};


class GenomeData{
 public:
    std::string name;                   // 64 bytes
    std::string file_path;
    u64 total_genome_len;
    u32 number_of_original_contigs;
    u32 number_of_fragments;
    u32 n_pixels_per_texture_1d;  // 一个texture 一维度 的像素点 1024
    u32 n_textures_1d;            // 一个维度有所少个 texture 32
    u32 n_textures;               // 一共有多少个 texture 528
    u32 n_pixels_1d;              // 一个维度多少个像素点 32768
    u32 mip_map_levels;           // 缩放多少次 6
    u32 n_bytes_per_texture;      // 一个texture占多少个bytes 698880
    std::vector<Graph> graphs;

 public:
    GenomeData(
        std::string file_path_,
        u32 total_genome_len_,
        u32 number_of_original_contigs_,
        u32 n_textures_1d_,
        u32 n_pixels_per_texture_1d_,
        u32 mip_map_levels_)
        :
        file_path(file_path_),
        total_genome_len(total_genome_len_),
        number_of_original_contigs(number_of_original_contigs_),
        number_of_fragments(0),
        n_textures_1d(n_textures_1d_),
        n_pixels_per_texture_1d(n_pixels_per_texture_1d_),
        mip_map_levels(mip_map_levels_) {
        get_name_from_path();
        n_pixels_1d = n_textures_1d * n_pixels_per_texture_1d;
        initial_texture_bytes();
        graphs.reserve(15);
        n_textures = ((1 + n_textures_1d) * n_textures_1d) >> 1;
    }

    ~GenomeData() {}

    void get_name_from_path() {
        #ifdef _WIN32
            char sep = '\\';
        #else
            char sep = '/';
        #endif  // #ifdef _WIN32
        u32 last_of_sep = file_path.find_last_of(sep);
        u32 last_of_dot = file_path.find_last_of('.');
        name = file_path.substr(last_of_sep+1, last_of_dot-last_of_sep-1);
    }

    void initial_texture_bytes() {
        /* nbytes per texture */
        if (!n_pixels_per_texture_1d)
            throw "PLease give value to n_pixels_per_texture before initial n_bytes_per_texture";
        n_bytes_per_texture = 0;
        u32 tmp_ = n_pixels_per_texture_1d;
        ForLoop(mip_map_levels) {
            n_bytes_per_texture += (tmp_*tmp_);
            tmp_ >>=1;
        }
        n_bytes_per_texture >>= 1;
    }

    void print_information(FILE* file);
};


#endif  // SRC_INCLUDE_PRETEXT_PARSING_PRETEXT_PARSER_H_

