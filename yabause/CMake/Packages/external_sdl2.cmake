#------------------------------------------------------------------------------
# External Project:     SDL2
# Downloads:            https://libsdl.org/release/
#------------------------------------------------------------------------------

include(ExternalProject)


set(SDL2_LIBRARY
      ${CMAKE_CURRENT_BINARY_DIR}/sdl2-prefix/lib/libSDL2.a
      ${CMAKE_CURRENT_BINARY_DIR}/sdl2-prefix/lib/libSDL2main.a
      dl
)

set(SDL2_INCLUDE_DIR
	${CMAKE_CURRENT_BINARY_DIR}/sdl2-prefix/include
	${CMAKE_CURRENT_BINARY_DIR}/sdl2-prefix/include/SDL2
)

set(SDL2_VERSION "2.0.9")

  set(SDL2_CMAKE_ARGS
      -DCMAKE_C_FLAGS=${CMAKE_C_FLAGS}  
      -DCMAKE_CXX_FLAGS=${CMAKE_CXX_FLAGS}
      -DSDL_STATIC=ON
      -DSDL_SHARED=OFF
      -DSDL_STATIC_PIC=ON
      -DSNDIO=OFF
      -DOPENGL=ON
      -DVIDEO_OPENGLES=OFF
      -DCMAKE_INSTALL_PREFIX:PATH=<INSTALL_DIR> 
      -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
      -DCMAKE_TOOLCHAIN_FILE=${CMAKE_TOOLCHAIN_FILE}
      -DCMAKE_OSX_ARCHITECTURES=${CMAKE_OSX_ARCHITECTURES}
  )
   
  ExternalProject_Add(
    sdl2
    PREFIX ${DEPENDENCY_EXTRACT_DIR}
    DOWNLOAD_DIR ${DEPENDENCY_DOWNLOAD_DIR}
    URL http://libsdl.org/release/SDL2-${SDL2_VERSION}.tar.gz
    URL_MD5 f2ecfba915c54f7200f504d8b48a5dfe
    #CONFIGURE_COMMAND <SOURCE_DIR>/configure --prefix=${DEPENDENCY_INSTALL_DIR}
    INSTALL_DIR ${DEPENDENCY_INSTALL_DIR}
    CMAKE_ARGS ${SDL2_CMAKE_ARGS}
  )

