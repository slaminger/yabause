project(yabause-vulkan)

yab_port_start()


set(YABAUSE_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../")
set(PORT_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/external/ ${CMAKE_CURRENT_SOURCE_DIR}/external/glm/ ${CMAKE_CURRENT_SOURCE_DIR}/external/gli/ ${CMAKE_CURRENT_SOURCE_DIR}/external/assimp/ )
set(PORT_LIBRARIES ${CMAKE_CURRENT_SOURCE_DIR}/libs/vulkan/vulkan-1.lib ${CMAKE_CURRENT_SOURCE_DIR}/libs/assimp/assimp.lib )
include_directories( ${YABAUSE_INCLUDE_DIR}  ${PORT_INCLUDE_DIRS})
set(yabause_vulkan_SOURCES
    VIDVulkan.cpp
    vulkanyabause.cpp
	vulkanyabause.h
	vulkandebug.cpp
	vulkantools.cpp
    vulkanbase.cpp
	vulkanbase.h
	yui.cpp
)

add_definitions("-DVK_PROTOTYPES -DVK_USE_PLATFORM_WIN32_KHR -D_USE_MATH_DEFINES -DNOMINMAX")




add_executable(yabause-vulkan ${yabause_vulkan_SOURCES})
target_link_libraries(yabause-vulkan yabause ${YABAUSE_LIBRARIES} ${PORT_LIBRARIES}   )

yab_port_success(yabause-vulkan)

