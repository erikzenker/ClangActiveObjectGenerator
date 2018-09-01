#! /bin/bash
/home/erik/projects/llvm/cmake-build-debug/bin/clang \
 -Xclang -load -Xclang /home/erik/projects/llvm/cmake-build-debug/lib/ClangActiveObject.so \
 -Xclang -plugin -Xclang clang-active-object \
 $1