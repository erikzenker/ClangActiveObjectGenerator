#! /bin/bash

FILE_NAME=$1
INTERFACE_NAME=$2

/home/erik/projects/llvm/cmake-build-debug/bin/clang++ \
 -D_GNU_SOURCE -D_DEBUG -D__STDC_CONSTANT_MACROS \
 -D__STDC_FORMAT_MACROS -D__STDC_LIMIT_MACROS -D_GNU_SOURCE \
 -fsyntax-only \
 -Xclang -load -Xclang /home/erik/projects/llvm/cmake-build-debug/lib/ClangActiveObject.so \
 -Xclang -plugin -Xclang clang-active-object \
 -Xclang -plugin-arg-clang-active-object -Xclang ${INTERFACE_NAME} \
 ${FILE_NAME}