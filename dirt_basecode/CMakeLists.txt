cmake_minimum_required(VERSION 3.1)
project(dirt)

add_subdirectory(ext ext_build)

## include directories

set(ext_tinyformat_srcs
    ext/tinyformat/tinyformat.h
)

set(ext_json_srcs
    ext/json/json.hpp
)

set(ext_stb_image_srcs
    ext/stb_image/stb_image_write.h
    ext/stb_image/stb_image.h
)

set(ext_rang_srcs
    ext/rang/include/rang.hpp
    )

set(dirt_headers
    src/surfacegroup.h
    src/bbh.h
    src/camera.h
    src/color.h
    src/common.h
    src/fwd.h
    src/image.h
    src/material.h
    src/mesh.h
    src/obj.h
    src/parser.h
    src/progress.h
    src/quad.h
    src/rand.h
    src/ray.h
    src/scene.h
    src/sphere.h
    src/surface.h
    src/transform.h
    src/path.h
    src/vector.h)

set(dirt_srcs
    src/surfacegroup.cpp
    src/bbh.cpp
    src/color.cpp
    src/common.cpp
    src/image.cpp
    src/material.cpp
    src/mesh.cpp
    src/obj.cpp
    src/parser.cpp
    src/progress.cpp
    src/quad.cpp
    src/scene-parse.cpp
    src/scene.cpp
    src/sphere.cpp
    src/surface.cpp
    src/path.cpp
)

# Compile core dirt library
add_library(dirt_lib STATIC
    ${dirt_headers}
    ${dirt_srcs}
    ${ext_tinyformat_srcs}
    ${ext_json_srcs}
    ${ext_stb_image_srcs}
    ${ext_rang_srcs})
	
target_include_directories(dirt_lib PUBLIC 
	${PROJECT_SOURCE_DIR}/src
	${PROJECT_SOURCE_DIR}/ext/tinyformat
	${PROJECT_SOURCE_DIR}/ext/json
	${PROJECT_SOURCE_DIR}/ext/stb_image
	${PROJECT_SOURCE_DIR}/../ext/eigen
	${PROJECT_SOURCE_DIR}/ext/rang/include
)

SOURCE_GROUP("ext\\tinyformat" FILES ${ext_tinyformat_srcs})
SOURCE_GROUP("ext\\json" FILES ${ext_json_srcs})
SOURCE_GROUP("ext\\stb_image" FILES ${ext_stb_image_srcs})
SOURCE_GROUP("dirt\\header files" FILES ${dirt_headers})
SOURCE_GROUP("dirt\\source files" FILES ${dirt_srcs})