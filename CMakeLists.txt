cmake_minimum_required(VERSION 2.8.9)

project(BoneEnhancer)

#-----------------------------------------------------------------------------
# Extension meta-information
set(EXTENSION_HOMEPAGE "http://www.example.com/Slicer/Extensions/BoneEnhancer")
set(EXTENSION_CATEGORY "Filtering")
set(EXTENSION_CONTRIBUTORS "Mikael Brudfors (UC3M)")
set(EXTENSION_DESCRIPTION "Enhancing bone in 2D ultrasound slices")
set(EXTENSION_ICONURL "http://www.example.com/Slicer/Extensions/BoneEnhancer.png")
set(EXTENSION_SCREENSHOTURLS "http://www.example.com/Slicer/Extensions/BoneEnhancer/Screenshots/1.png")

#-----------------------------------------------------------------------------
# Extension dependencies
find_package(Slicer REQUIRED)
include(${Slicer_USE_FILE})

#-----------------------------------------------------------------------------
# Extension modules
add_subdirectory(BoneEnhancerPy)
add_subdirectory(BoneEnhancerCpp)
## NEXT_MODULE

#-----------------------------------------------------------------------------
include(${Slicer_EXTENSION_CPACK})
