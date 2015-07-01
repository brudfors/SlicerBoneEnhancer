### **BoneEnhancer** ###

A 3D Slicer module which enhances bone in images. It is possible to test the module from 3D Slicer pressing *Reload and Test*, which uses a lumbar spine US volume (available in MIDAS).

The module uses *OpenMP* and *Intel MKL*. At the moment the path to the *Intel MKL* include directory as well as the libraries used are hardcoded into BoneEnhancerCpp/Logic/CMakeLists.txt. Also,  remember to add the *Intel MKL* dlls to your path: ...\mkl\redist\intel64\mkl, ...\mkl\redist\intel64\compiler.

One more thing: the *Intel MKL* code only supports double data at this moment.