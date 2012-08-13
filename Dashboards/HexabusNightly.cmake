# -*- mode: cmake; -*-

set(ENV{https_proxy} "http://squid.itwm.fhg.de:3128/")
include(CTestConfigHexabus.cmake)

set(URL "https://github.com/mysmartgrid/hexabus.git")

set(KDE_CTEST_DASHBOARD_DIR "/tmp/msgrid")
set(CTEST_BASE_DIRECTORY "${KDE_CTEST_DASHBOARD_DIR}/${_projectNameDir}")
set(CTEST_SOURCE_DIRECTORY "${CTEST_BASE_DIRECTORY}/${_srcDir}-${_git_branch}" )
set(CTEST_BINARY_DIRECTORY "${CTEST_BASE_DIRECTORY}/${_buildDir}-${CTEST_BUILD_NAME}")
set(CTEST_INSTALL_DIRECTORY "${CTEST_BASE_DIRECTORY}/install-${CTEST_BUILD_NAME}")
set(KDE_CTEST_VCS "git")
set(KDE_CTEST_VCS_REPOSITORY ${URL})

#if(NOT CTEST_BUILD_NAME)
#  set(CTEST_BUILD_NAME ${CMAKE_SYSTEM_NAME}-CMake-${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION}.${CMAKE_PATCH_VERSION}${KDE_C)
#endif(NOT CTEST_BUILD_NAME)

set(CMAKE_INSTALL_PREFIX "/usr")
set(CTEST_CMAKE_GENERATOR "Unix Makefiles")
#set(CTEST_BUILD_CONFIGURATION "Profiling")

# generic support code, provides the kde_ctest_setup() macro, which sets up everything required:
get_filename_component(_currentDir "${CMAKE_CURRENT_LIST_FILE}" PATH)
include( "${_currentDir}/KDECTestNightly.cmake")
kde_ctest_setup()

function(create_project_xml)
  file(WRITE "${CTEST_BINARY_DIRECTORY}/Project.xml"
    "<Project name=\"hexabus\">
  <SubProject name=\"hexabridge\">
  </SubProject>
  <SubProject name=\"hexaswitch\">
  </SubProject>
  <SubProject name=\"hexadisplay\">
  </SubProject>
  <SubProject name=\"hexinfo\">
  </SubProject>
</Project>
")
  ctest_submit(FILES "${CTEST_BINARY_DIRECTORY}/Project.xml") 
endfunction()

set(ctest_config ${CTEST_BASE_DIRECTORY}/CTestConfig.cmake)
#######################################################################
ctest_empty_binary_directory(${CTEST_BINARY_DIRECTORY})


find_program(CTEST_GIT_COMMAND NAMES git)
set(CTEST_UPDATE_TYPE git)

set(CTEST_UPDATE_COMMAND  ${CTEST_GIT_COMMAND})
if(NOT EXISTS "${CTEST_SOURCE_DIRECTORY}/.git/HEAD")
  set(CTEST_CHECKOUT_COMMAND "${CTEST_GIT_COMMAND} clone ${URL} ${CTEST_SOURCE_DIRECTORY}")
endif(NOT EXISTS "${CTEST_SOURCE_DIRECTORY}/.git/HEAD")

create_project_xml()

configure_file("CTestConfigHexabus.cmake" ${CTEST_BASE_DIRECTORY}/CTestConfig.cmake COPYONLY)
ctest_empty_binary_directory("${CTEST_BINARY_DIRECTORY}")
set(_ctest_type "Nightly")
# set(_ctest_type "Continuous")
ctest_start(${_ctest_type})
ctest_update(SOURCE "${CTEST_SOURCE_DIRECTORY}")
ctest_submit(PARTS Update)

execute_process(
  COMMAND ${CTEST_GIT_COMMAND} checkout  ${_git_branch}
  WORKING_DIRECTORY ${CTEST_SOURCE_DIRECTORY}
  )

# to get CTEST_PROJECT_SUBPROJECTS definition:
include(${ctest_config})

##
set(CMAKE_ADDITIONAL_PATH ${CTEST_INSTALL_DIRECTORY})

foreach(subproject ${CTEST_PROJECT_SUBPROJECTS})
  set_property(GLOBAL PROPERTY SubProject ${subproject})
  set_property (GLOBAL PROPERTY Label ${subproject})

  #set(CTEST_SOURCE_DIRECTORY "${CTEST_BASE_DIRECTORY}/${_srcDir}/hostsoftware/${subproject}")
  #set(CTEST_BINARY_DIRECTORY "${CTEST_BASE_DIRECTORY}/${_buildDir}-${CTEST_BUILD_NAME}/${subproject}")
  file(MAKE_DIRECTORY "${CTEST_BINARY_DIRECTORY}/${subproject}")
  #ctest_start(${_ctest_type})

  ##
  if(CMAKE_TOOLCHAIN_FILE)
    kde_ctest_write_initial_cache("${CTEST_BINARY_DIRECTORY}/${subproject}"
      CMAKE_TOOLCHAIN_FILE
      CMAKE_INSTALL_PREFIX
      CMAKE_ADDITIONAL_PATH
      )
  else(CMAKE_TOOLCHAIN_FILE)
      set(BOOST_ROOT /homes/krueger/external_software/ubuntu_100403/${CMAKE_SYSTEM_PROCESSOR}/boost/1.46)
      kde_ctest_write_initial_cache("${CTEST_BINARY_DIRECTORY}/${subproject}"
	BOOST_ROOT
	CMAKE_INSTALL_PREFIX
	CMAKE_ADDITIONAL_PATH
	)
  endif(CMAKE_TOOLCHAIN_FILE)
  
  ##
  ctest_configure(BUILD ${CTEST_BINARY_DIRECTORY}/${subproject}
    SOURCE ${CTEST_SOURCE_DIRECTORY}/hostsoftware/${subproject}  APPEND
    RETURN_VALUE resultConfigure)
  ctest_submit(PARTS Configure)
  message("====> Configure: ${resultConfigure}")

  ##
  if( STAGING_DIR)
    include(${CTEST_BINARY_DIRECTORY}/${subproject}/CMakeCache.txt)
    set(ENV{STAGING_DIR}     ${OPENWRT_STAGING_DIR})
  endif( STAGING_DIR)

  set(CTEST_BUILD_TARGET ${subproject})
  ctest_build(BUILD ${CTEST_BINARY_DIRECTORY}/${subproject} 
    APPEND RETURN_VALUE build_res)
  # builds target ${CTEST_BUILD_TARGET}
  ctest_submit(PARTS Build)
  message("====> BUILD result: ${build_res}")

  #ctest_test(BUILD "${CTEST_BINARY_DIRECTORY}"
  #  INCLUDE_LABEL "${subproject}"
  #)

  # runs only tests that have a LABELS property
  #matching "${subproject}"
  #ctest_submit(PARTS Test)

  ## do an installation
  if( NOT ${build_res})
    execute_process(
      COMMAND cmake -DCMAKE_INSTALL_PREFIX=${CTEST_INSTALL_DIRECTORY} -P cmake_install.cmake
      WORKING_DIRECTORY ${CTEST_BINARY_DIRECTORY}/${subproject}
      )
  endif( NOT ${build_res})
  
  ## create packages
  include(${CTEST_BINARY_DIRECTORY}/${subproject}/CPackConfig.cmake)
  if( STAGING_DIR)
    set(ENV{PATH}            ${OPENWRT_STAGING_DIR}/host/bin:$ENV{PATH})
  endif( STAGING_DIR)

  if( NOT ${build_res})
    execute_process(
      COMMAND cpack -G DEB
      COMMAND sync
      WORKING_DIRECTORY ${CTEST_BINARY_DIRECTORY}/${subproject}
      )
  endif( NOT ${build_res})
  
  # upload files
  if( NOT ${build_res})
    if(CPACK_ARCHITECTUR)
      set(OPKG_FILE_NAME "${CPACK_PACKAGE_NAME}_${CPACK_PACKAGE_VERSION}_${CPACK_ARCHITECTUR}")
      set(_package_file "${OPKG_FILE_NAME}.ipk")
    else(CPACK_ARCHITECTUR)
      set(_package_file "${CPACK_PACKAGE_FILE_NAME}.deb")
    endif(CPACK_ARCHITECTUR)
    message("==> Upload packages - ${_package_file}")
    set(_export_host "192.168.9.63")
    execute_process(
      COMMAND ssh ${_export_host} mkdir -p packages/${CMAKE_SYSTEM_PROCESSOR}
      COMMAND scp -p ${_package_file} ${_export_host}:packages/${CMAKE_SYSTEM_PROCESSOR}/
      WORKING_DIRECTORY ${CTEST_BINARY_DIRECTORY}/${subproject}
      )
  endif( NOT ${build_res})
endforeach()

ctest_submit(RETURN_VALUE res)