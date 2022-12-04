file(GLOB FACTORY_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)
file(GLOB FACTORY_HEADERS ${CMAKE_CURRENT_SOURCE_DIR}/*.hpp)

add_executable(ISOSimulator ${FACTORY_SOURCES} ${FACTORY_HEADERS})
target_include_directories(ISOSimulator PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(ISOSimulator ${PROJECT})