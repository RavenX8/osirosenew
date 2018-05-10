set(GTEST_INSTALL_DIR ${CMAKE_THIRD_PARTY_DIR})

if(MSVC11)
  set(ADD_CXX_FLAGS "/D_VARIADIC_MAX=10")
endif()

if(NOT BUILD_SHARED_LIBS)
  set(BUILD_SHARED_LIBS OFF)
endif()

if(NOT GMOCK_FOUND AND WITH_GMOCK)
  set(BUILD_GMOCK ON)
else()
  set(BUILD_GMOCK OFF)
endif()

if(WIN32)
  ExternalProject_Add(
    gtest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG release-1.8.0
    GIT_SHALLOW true
    
    CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${GTEST_INSTALL_DIR} -DCMAKE_CXX_FLAGS=${ADD_CXX_FLAGS} -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE} -DBUILD_GMOCK=${BUILD_GMOCK} -DBUILD_SHARED_LIBS=${BUILD_SHARED_LIBS} -Dgmock_build_tests=OFF
    INSTALL_DIR ${GTEST_INSTALL_DIR}
  )
else()
  # This is a workaround for Ninja not allowing us to build if these libs weren't built before
  if(${BUILD_SHARED_LIBS})
    set(_byproducts
      ${GTEST_INSTALL_DIR}/lib/libgtest.so
      ${GTEST_INSTALL_DIR}/lib/libgmock.so
    )
  else()
    set(_byproducts
      ${GTEST_INSTALL_DIR}/lib/libgtest.a
      ${GTEST_INSTALL_DIR}/lib/libgmock.a
    )
  endif()
  
  ExternalProject_Add(
    gtest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG release-1.8.0
    GIT_SHALLOW true
    
    CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${GTEST_INSTALL_DIR} -DCMAKE_CXX_FLAGS=${ADD_CXX_FLAGS} -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE} -DBUILD_GMOCK=${BUILD_GMOCK} -DBUILD_SHARED_LIBS=${BUILD_SHARED_LIBS} -Dgmock_build_tests=OFF
    BUILD_BYPRODUCTS ${_byproducts}
    INSTALL_DIR ${GTEST_INSTALL_DIR}
  )
endif()


ExternalProject_Get_Property(
  gtest
  install_dir
)

if(WIN32)
  set(GTEST_LIBRARIES "${install_dir}/${CMAKE_BUILD_TYPE}/gtest.lib")
  set(GTEST_INSTALL_LIBS "${install_dir}/${CMAKE_BUILD_TYPE}/gtest.dll")
else()
  if(${BUILD_SHARED_LIBS})
    set(GTEST_LIBRARIES "${install_dir}/lib/libgtest.so")
    set(GTEST_INSTALL_LIBS "${install_dir}/lib/libgtest.so")
    if(BUILD_GMOCK)
      set(GTEST_LIBRARIES "${install_dir}/lib/libgmock.so;${GTEST_LIBRARIES}")
      set(GTEST_INSTALL_LIBS "${install_dir}/lib/libgmock.so;${GTEST_INSTALL_LIBS}")
    endif()
  else()
    set(GTEST_LIBRARIES "${install_dir}/lib/libgtest.a")
    set(GTEST_INSTALL_LIBS "${install_dir}/lib/libgtest.a")
    if(BUILD_GMOCK)
      set(GTEST_LIBRARIES "${install_dir}/lib/libgmock.a;${GTEST_LIBRARIES}")
      set(GTEST_INSTALL_LIBS "${install_dir}/lib/libgmock.a;${GTEST_INSTALL_LIBS}")
    endif()
  endif()
endif()

set(GTEST_INCLUDE_DIRS "${install_dir}/include/gtest")
if(BUILD_GMOCK)
  set(GTEST_INCLUDE_DIRS "${install_dir}/include/gmock;${GTEST_INCLUDE_DIRS}")
endif()

set(GTEST_BOTH_LIBRARIES ${GTEST_LIBRARIES} ${GTEST_MAIN_LIBRARIES})
