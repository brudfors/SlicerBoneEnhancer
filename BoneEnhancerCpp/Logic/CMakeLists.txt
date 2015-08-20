project(vtkSlicer${MODULE_NAME}ModuleLogic)

set(KIT ${PROJECT_NAME})

set(${KIT}_EXPORT_DIRECTIVE "VTK_SLICER_${MODULE_NAME_UPPER}_MODULE_LOGIC_EXPORT")

FIND_PACKAGE( OpenMP REQUIRED)
if(OPENMP_FOUND)
  message("OPENMP FOUND")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")
endif()

find_path(IntelComposerXEdir IntelComposerXE
  PATHS "C:/Program Files (x86)/Intel/Composer XE 2015/"
  DOC "A path to Intel Composer XE" )

if (NOT IntelComposerXEdir)
  message( FATAL_ERROR "A path to Intel Composer XE is invalid." )
endif()

set(${KIT}_INCLUDE_DIRECTORIES
  "${IntelComposerXEdir}/mkl/include"
  )

set(${KIT}_SRCS
  vtkSlicer${MODULE_NAME}Logic.cxx
  vtkSlicer${MODULE_NAME}Logic.h
  )

set(${KIT}_TARGET_LIBRARIES
  "${IntelComposerXEdir}/mkl/lib/intel64/mkl_intel_lp64_dll.lib"
  "${IntelComposerXEdir}/mkl/lib/intel64/mkl_intel_thread_dll.lib"
  "${IntelComposerXEdir}/mkl/lib/intel64/mkl_core_dll.lib"
  "${IntelComposerXEdir}/compiler/lib/intel64/libiomp5md.lib"
  )

#-----------------------------------------------------------------------------
SlicerMacroBuildModuleLogic(
  NAME ${KIT}
  EXPORT_DIRECTIVE ${${KIT}_EXPORT_DIRECTIVE}
  INCLUDE_DIRECTORIES ${${KIT}_INCLUDE_DIRECTORIES}
  SRCS ${${KIT}_SRCS}
  TARGET_LIBRARIES ${${KIT}_TARGET_LIBRARIES}
  )
