#! /bin/bash

FILE_NAME=$1
INTERFACE_NAME=$2

/home/erik/projects/llvm/cmake-build-debug/bin/clang++ \
 -D_GNU_SOURCE -D_DEBUG -D__STDC_CONSTANT_MACROS \
 -D__STDC_FORMAT_MACROS -D__STDC_LIMIT_MACROS -D_GNU_SOURCE \
 -fsyntax-only \
 -Xclang -load -Xclang /home/erik/projects/llvm/cmake-build-debug/lib/clang_active_object.so \
 -Xclang -plugin -Xclang clang_active_object \
 -Xclang -plugin-arg-clang_active_object -Xclang ${INTERFACE_NAME} \
 -Xclang -plugin-arg-clang_active_object -Xclang /home/erik/projects/llvm/tools/clang/examples/ClangActiveObject/ActiveObject.mustache \
 ${FILE_NAME}