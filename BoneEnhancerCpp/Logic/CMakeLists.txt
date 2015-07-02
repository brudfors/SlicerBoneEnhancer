project(vtkSlicer${MODULE_NAME}ModuleLogic)

set(KIT ${PROJECT_NAME})

set(${KIT}_EXPORT_DIRECTIVE "VTK_SLICER_${MODULE_NAME_UPPER}_MODULE_LOGIC_EXPORT")

# set(IntelComposerXEdir "C:/Program Files (x86)/Intel/Composer XE 2015")
find_path(IntelComposerXEdir IntelComposerXE
  PATHS "C:/Program Files (x86)/Intel/Composer XE 2015"
  DOC "A path to Intel Composer XE" )

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
