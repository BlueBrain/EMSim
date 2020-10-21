## Copyright (c) BBP/EPFL 2011-2017, Stefan.Eilemann@epfl.ch

find_package(Boost REQUIRED COMPONENTS unit_test_framework)

set(TESTS_SRC
    samplePoints.cpp
    volume.cpp
)

foreach(FILE ${TESTS_SRC})
    get_filename_component(NAME ${FILE} NAME_WE)
    add_executable(${NAME} ${FILE})

    target_link_libraries(${NAME}
        PRIVATE
        ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY}
        EMSimCommon
        )
    add_test(NAME ${NAME} COMMAND ${NAME})
endforeach()
