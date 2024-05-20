
set(target_name PretextParser)
set(library_need deflate glfw3)

# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address -fno-omit-frame-pointer")# 配置编译选项，这样设置后检查是否有内存泄漏
# set(CMAKE_BUILD_TYPE Debug)
# set(CMAKE_BUILD_TYPE Release)

include_directories(
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/src/glfw/include
    ${CMAKE_SOURCE_DIR}/src/libdeflate
    ${CMAKE_SOURCE_DIR}/src/glfw/deps
    )
link_directories(
    ${CMAKE_SOURCE_DIR}/src/libdeflate/build
    ${CMAKE_SOURCE_DIR}/src/glfw/build/src
)

# 创建pretext.cpp的库文件 pretext_parse_lib 
add_library(pretext_parse_lib ${CMAKE_SOURCE_DIR}/src/pretext_parser.cpp ${CMAKE_SOURCE_DIR}/src/glfw/deps/glad.c)
# 先添加了这些库才能添加glfw静态库 （如果是添加动态库无所谓）
# 定义需要的库列表，寻找这些库并且将这些库添加到make的目标中
set(library_need Cocoa OpenGL IOKit CoreVideo)
# set(library_need cocoa opengl iokit) # 库名完全小写也可以，cmake文件的命令可以全部小写，目前看来是这样
if (APPLE)
    foreach (library ${library_need})
        find_library(${library}_library ${library} required)
        target_link_libraries(pretext_parse_lib PRIVATE ${${library}_library} )
        message(STATUS "${library}_library found")  # STATUS 要大写
    endforeach()
endif()
target_link_libraries(pretext_parse_lib PRIVATE glfw3 deflate)


# 创建绑定库 PretextParser -> python
# python -m pybind11 --includes
find_package(Python COMPONENTS Interpreter Development)
find_package(pybind11 CONFIG)
pybind11_add_module(PretextParserPython ${CMAKE_SOURCE_DIR}/src/pybind_pretext_agp.cpp)  # 添加python模块
target_link_libraries(PretextParserPython PRIVATE pretext_parse_lib)

add_library(pretext_parser_python_lib ${CMAKE_SOURCE_DIR}/src/pybind_pretext_agp.cpp)
target_link_libraries(pretext_parser_python_lib PRIVATE pretext_parse_lib Python::Python ${PYTHON_LIBRARIES} pybind11::embed)

# 创建 pretext parser 主程序
add_executable(main_pretext_parser ${CMAKE_SOURCE_DIR}/src/main_pretext.cpp)
target_link_libraries(main_pretext_parser PRIVATE pretext_parser_python_lib pybind11::module)
# 在 Debug 模式下定义 DEBUG 宏
if(CMAKE_BUILD_TYPE MATCHES Debug)
    target_compile_definitions(main_pretext_parser PRIVATE DEBUG)
endif()
