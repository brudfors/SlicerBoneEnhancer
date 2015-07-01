### **BoneEnhancer** ###

A 3D Slicer module to enhance bone in images. 

The module uses *OpenMP* and *Intel MKL*. At the moment the path to the *Intel MKL* include directory as well as the libraries used are hardcoded into BoneEnhancerCpp/Logic/CMakeLists.txt. Also remember to add the *Intel MKL* dlls to your path: ...\mkl\redist\intel64\mkl, ...\mkl\redist\intel64\compiler.