/*
    Copyright 2024 Shaoheng Guan sg35@sanger.ac.uk, Wellcome Sanger Institute
*/

#ifndef SRC_INCLUDE_PRETEXT_PARSING_PRETEXT_PARSER_H_
#include <pretext_parser.h>
#endif  // SRC_INCLUDE_PRETEXT_PARSING_PRETEXT_PARSER_H_


global_function u08 Magic[] = {'p', 's', 't', 'm'};
global_variable char extension_magic_bytes[][4] = {
    {'p', 's', 'g', 'h'}
    };
// global_variable GenomeData* genome;
global_variable f64 current_file_loc;
enum extension_type {
    extension_graph
    };
global_variable GLuint texture_id;
global_variable GLuint vao;


void print_star_line(u32 len) {
    std::cout << std::left << std::setw(len)
        <<  std::setfill('*') << "*" << std::endl;
}


// 创建一个监听监盘动作的函数
void process_input(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    // if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_RELEASE) color_index--;
    // else if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_RELEASE) color_index++;
    // color_index%=5;
}


void gen_random_u32(size_t size, u32 min_, u32 max_, u32* random_arr) {
    // 创建随机种子
    std::mt19937 random_seed(std::time(nullptr));

    // 创建一个分布对象
    std::uniform_int_distribution<u32> dist(min_, max_);

    ForLoop(size) {
        random_arr[index] = dist(random_seed);
    }
}

void is_power_of_2(u32 wanted_resolution, u32 mip_map_level, u32 n_pixel_per_texture_1d) {
    if ( wanted_resolution != 0 && ((wanted_resolution & (wanted_resolution-1)) == 0) ) {
        ForLoop(mip_map_level) {
            if (wanted_resolution == (n_pixel_per_texture_1d >> index)) {
                return;
            }
        }
    } else {
        std::stringstream ss;
        ss << "The wanted_resolution(" << wanted_resolution << ") is not the exponential of 2\n";
        throw std::runtime_error(ss.str());
    }
    std::stringstream ss;
    ss << "The wanted_resolution(" << wanted_resolution <<
        ") should not large than n_pixel_per_texture_1d(" <<
        n_pixel_per_texture_1d << ") or small than (" <<
        (n_pixel_per_texture_1d >> (mip_map_level-1)) << ")" << "\n";
    throw std::runtime_error(ss.str());
    return;
}


global_function
void
PrintShaderInfoLog(GLuint shader) {
    s32 infoLogLen = 0;
    s32 charsWritten = 0;
    GLchar *infoLog;

    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLogLen);

    if (infoLogLen > 0) {
        // infoLog = PushArray(Working_Set, GLchar, (u32)infoLogLen);
        infoLog = reinterpret_cast<GLchar*>(malloc((u32) infoLogLen));
        glGetShaderInfoLog(shader, infoLogLen, &charsWritten, infoLog);
        fprintf(stderr, "%s\n", infoLog);
        free(infoLog);
    }
}


void GenomeData::print_information(FILE* file = nullptr) {
    if (!file) file = stdout;
    fprintf(
    file,
    "total_genome_len: %llu\n"
    "number_of_original_contigs: %d\n"
    "number_of_fragments: %d\n"
    "n_pixels_per_texture_1d: %d\n"
    "n_textures_1d: %d\n"
    "n_textures: %d\n"
    "n_pixels_1d: %d\n"
    "mip_map_levels: %d\n"
    "n_bytes_per_texture: %d\n"
    "num_extensions: %zu\n",
    total_genome_len,
    number_of_original_contigs,
    number_of_fragments,
    n_pixels_per_texture_1d,
    n_textures_1d,
    n_textures,
    n_pixels_1d,
    mip_map_levels,
    n_bytes_per_texture,
    graphs.size());
}


FILE* test_file(
    const char* file_path,
    u64* file_size = 0) {
    FILE* file;
    file  = fopen(file_path, "rb");
    if (!file) {
        fprintf(stderr, "Failed, path: %s\n", file_path);
        exit(-1);
    }

    if (file_size) {  // 文件指针不为空
        fseek(file, 0, SEEK_END);
        *file_size = (u64)ftell(file);
        fseek(file, 0, SEEK_SET);
    }

    u08 magic_test[sizeof(Magic)];

    u32 bytes_read = (u32) fread(
        magic_test,             // 存储数据的指针位置
        1,                      // 每个读取数据的大小
        sizeof(Magic),          // 读取数据个数
        file);                  // 读取数据的文件流

    if (bytes_read != sizeof(Magic)) {
        fclose(file);
        file = 0;
    }
    ForLoop(sizeof(magic_test)) {
        if (magic_test[index] != Magic[index]) {
            fclose(file);
            file = 0;
        }
    }
    return file;
}


GenomeData* load_file(const char* file_path, u32 wanted_resolution) {
    /* 初始化解压模块 */
    libdeflate_decompressor* decompressor = libdeflate_alloc_decompressor();
    if (!decompressor) {
        throw std::runtime_error("Failed to allocate the deompressor\n");
        exit(-1);
    }

    u64 file_size = 0;
    FILE* file = test_file(file_path, &file_size);

    if (!file) {
        fprintf(stderr, "Failed file_path: %s", file_path);
        exit(-1);
    }

    /* 读取文件头 header */
    u32 n_bytes_header_comp, n_bytes_header;
    fread(&n_bytes_header_comp, sizeof(u32), 1, file);
    fread(&n_bytes_header, sizeof(u32), 1, file);

    u08* header_comp = new u08[n_bytes_header_comp];
    u08* header = new u08[n_bytes_header];

    fread(header_comp, sizeof(u08), n_bytes_header_comp, file);

    if (libdeflate_deflate_decompress(  // 如果解压不成功
        decompressor,
        header_comp,
        n_bytes_header_comp,
        header,
        n_bytes_header,
        NULL)) {
            throw std::runtime_error("Error arised while decompress the compressed header\n");
            exit(-1);
        }
    delete[] header_comp;  // 释放存储compressed header的空间

    u64 total_genome_len;
    u08* tmp = reinterpret_cast<u08*>(&total_genome_len);  // 基因组总长度 base pair
    u08* header_tmp = header;
    ForLoop(sizeof(u64)) {
        *tmp++ = *header_tmp++;
    }

    u32 number_of_original_contigs;
    tmp = reinterpret_cast<u08*>(&number_of_original_contigs);  // 总contig个数
    ForLoop(sizeof(u32)) {
        *tmp++ = *header_tmp++;
    }

    /* 读取所有的contigs的占比f32和名字 */
    header_tmp += (sizeof(f32) + 64) * number_of_original_contigs;

    /* 获取genome展示在图像上的显示的参数 */
    u32 n_pixels_per_texture_1d = 1 << *header_tmp++;
    u32 n_textures_1d = 1 << *header_tmp++;
    u32 mip_map_levels = *header_tmp++;
    delete[] header;
    if (wanted_resolution == 0) wanted_resolution = n_pixels_per_texture_1d;
    if (wanted_resolution > n_pixels_per_texture_1d) {
        std::stringstream ss;
        ss << "wanted_resolution(" << wanted_resolution <<
            ") can not be larger than n_pixels_per_texture_1d(" <<
            n_pixels_per_texture_1d << ")\n";
        throw std::runtime_error(ss.str());
    } else {  // make sure wanted_resolution equals to the exponential of 2
        is_power_of_2(
            wanted_resolution,
            mip_map_levels,
            n_pixels_per_texture_1d);
    }
    GenomeData* genome = new GenomeData(
        file_path,
        total_genome_len,
        number_of_original_contigs,
        n_textures_1d,
        n_pixels_per_texture_1d,
        mip_map_levels);

    // /* tetxure 读取 */
    {
        u08* tmp_texture = new u08[genome->n_bytes_per_texture];
        ForLoop(genome->n_textures) {
            u32 bytes_comp;
            fread(&bytes_comp, 4, 1, file);
            fseek(file, bytes_comp, SEEK_CUR);
        }
        delete[] tmp_texture;
    }

    /* extension 读取 */
    {
        current_file_loc = ftell(file);  // 获取当前file指针的位置
        u08 magic_test[sizeof(extension_magic_bytes[0])];  // 4 bytes
        while (ftell(file) < file_size) {
            u32 bytes_read = fread(magic_test, 1, sizeof(magic_test), file);
            if (bytes_read != sizeof(magic_test)) {
                fprintf(
                    stderr,
                    "Failed in read (%zu)th extension_test, got (%d) bytes which should be (%zu) bytes\n",
                    genome->graphs.size(), bytes_read, sizeof(magic_test));
                break;
            }

            bool found_extension = false;
            ForLoop(ArrayLenCount(extension_magic_bytes)) {
                found_extension = true;
                tmp = reinterpret_cast<u08*>(extension_magic_bytes[index]);
                ForLoop1(bytes_read) {
                    if (magic_test[index1] != tmp[index1]) {
                        found_extension = false;
                        break;
                    }
                }
                if (!found_extension) continue;

                // 如果找到extension对应的类别，则开始读取
                extension_type type = (extension_type) index;
                switch (type) {
                case extension_graph: {
                    u32 extension_comp_size;
                    // 4 bytes, size of the compressed file
                    fread(&extension_comp_size, sizeof(extension_comp_size), 1, file);
                    u08* tmp_extension_comp = new u08[extension_comp_size];
                    fread(tmp_extension_comp, 1, extension_comp_size, file);
                    u32 data_size = sizeof(u32) * genome->n_pixels_1d + 64;  // sizeof(data) + sizeof(name)
                    u08* tmp_extension = new u08[data_size];
                    if (libdeflate_deflate_decompress(
                            decompressor,
                            reinterpret_cast<void*>(tmp_extension_comp),
                            extension_comp_size,
                            reinterpret_cast<void*>(tmp_extension),
                            data_size,
                            NULL)) {
                            printf("Failed in decompress extension_graph, (%zu) \n", genome->graphs.size());
                            exit(1);
                        }
                    delete[] tmp_extension_comp;
                    auto* tmp_ptr = tmp_extension;

                    // 初始化空间存储 graph
                    Graph tmp_graph(genome->n_pixels_1d);
                    char tmp_name[64];
                    char* tmp_name_ptr = tmp_name;
                    ForLoop1(64) {
                        *tmp_name_ptr++ = static_cast<char>(*tmp_ptr++);
                    }
                    tmp_graph.name = std::string(tmp_name);
                    std::memcpy(tmp_graph.data.data(), tmp_ptr, sizeof(u32) * tmp_graph.size);
                    genome->graphs.push_back(tmp_graph);
                    delete[] tmp_extension;
                    break;
                }
                default: {
                    printf("No this kind of extension extension_type_index (%d) \n", index);
                    exit(1);}
                }
            }
            current_file_loc = ftell(file);
        }
    }
    fclose(file);
    delete decompressor;

    return genome;
}


void printCentered(
    const std::string& text,
    int width,
    char pad_char
) {
    std::string padding((width - text.length())>>1, pad_char);
    std::cout << padding << text << padding  << std::endl;
}


void glfw_init() {
    // 初始化窗口
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);  // 在mac上需要添加这一行
#endif

    std::stringstream ss;
    ss << "PretextView File Parser";
    // 创建一个 opengl 窗口
    GLFWwindow* window = glfwCreateWindow(width__, height__, ss.str().c_str(), NULL, NULL);
    if (!window) {
        printf("Window initilizing failed\n");
        glfwTerminate();
        exit(-1);
    }
    glfwMakeContextCurrent(window);  // 将窗口绑定到当前线程
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD\n" << std::endl;
        glfwTerminate();
        exit(-1);
    }
    // return window;
}

