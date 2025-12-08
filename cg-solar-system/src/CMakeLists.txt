

# source files
file(GLOB SOURCES ./*.cpp)
file(GLOB HEADERS ./*.h)
file(GLOB SHADERS ./shader/*.vert ./shader/*.frag)

if(EMSCRIPTEN)

    add_executable(solarsystem ${HEADERS} ${SOURCES} ${SHADERS})
    target_link_libraries(solarsystem utils stb_image)
    
    set_target_properties(solarsystem PROPERTIES LINK_FLAGS "\
     --shell-file ${PROJECT_SOURCE_DIR}/lib/utils/shell.html\
     --preload-file ${PROJECT_SOURCE_DIR}/textures/sun.png@./textures/sun.png\
     --preload-file ${PROJECT_SOURCE_DIR}/textures/stars2.png@./textures/stars2.png\
     --preload-file ${PROJECT_SOURCE_DIR}/textures/mercury.png@./textures/mercury.png\
     --preload-file ${PROJECT_SOURCE_DIR}/textures/venus.png@./textures/venus.png\
     --preload-file ${PROJECT_SOURCE_DIR}/textures/earth_day.png@./textures/earth_day.png\
     --preload-file ${PROJECT_SOURCE_DIR}/textures/earth_night2.png@./textures/earth_night2.png\
     --preload-file ${PROJECT_SOURCE_DIR}/textures/earth_clouds.png@./textures/earth_clouds.png\
     --preload-file ${PROJECT_SOURCE_DIR}/textures/earth_gloss.png@./textures/earth_gloss.png\
     --preload-file ${PROJECT_SOURCE_DIR}/textures/mars.png@./textures/mars.png\
     --preload-file ${PROJECT_SOURCE_DIR}/textures/jupiter.png@./textures/jupiter.png\
     --preload-file ${PROJECT_SOURCE_DIR}/textures/saturn.png@./textures/saturn.png\
     --preload-file ${PROJECT_SOURCE_DIR}/textures/saturn_rings.png@./textures/saturn_rings.png\
     --preload-file ${PROJECT_SOURCE_DIR}/textures/moon.png@./textures/moon.png\
     --preload-file ${PROJECT_SOURCE_DIR}/src/shader@./shader")

else()

    add_executable(solarsystem ${HEADERS} ${SOURCES} ${SHADERS})
    target_link_libraries(solarsystem utils glew glfw stb_image ${OPENGL_LIBRARIES})

endif()
