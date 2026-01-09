## A compiler to compile Pascal-S codes to C.

To run this, you need:
1. Install [CMake](https://cmake.org/).
2. Of course, you must have a C++ compiler installed, and ensure it support C++ 20. Check [GCC](https://gcc.gnu.org/install/binaries.html) or [Clang](https://releases.llvm.org/download.html).

Then:
1. `cmake -Bbuild` to generate the build files. Use `-GNinja` to use Ninja as build tool.
2. `cmake --build build` to build.

After this, the binary file will be `build/COMPILER`, the testing files will be inside `build/test`.

### For developers

Try [clangd](https://clangd.llvm.org/) for better IntelliSense.

You can use `cmake -Bbuild -DCMAKE_EXPORT_COMPILE_COMMANDS=ON` to generate the compile commands file for clangd.

Use [clang-format](https://clang.llvm.org/docs/ClangFormat.html) to format the code before commit.
for_statement

### Educoder testcases

Testcases from educoder are saved under `open_set`. You can run `./test_all_testcases_locally.sh` to generate all corresponding C codes. The results will be saved under `output_for_test_set`.