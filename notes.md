# Learnings

- `whatis` cool command to view what a command/file does and which manual section it belongs to
- the stages of compilation: preprocess, compile, assemble, and link.
    - flag `-E` stops after preprocessing. extension `i` and called a preprocessed source file
    - flag `-S` stops after compilation. extension `s` and called an assembly code file
    - flag `-c` stops after assembly. extension `o` and called a relocatable object file. you can read relocatable object files with the command `readelf`
    - no flag for linking. called an executable binary. ELF on Linux, Mach-O on macOS, and PE/COFF on Windows
- gdb useful command to check memory layout `info proc mappings`

# Read later

- https://guyinatuxedo.github.io/29-tcache/tcache_explanation/index.html
