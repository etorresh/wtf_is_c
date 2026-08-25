# Learnings
- `whatis` cool command
- the stages of compilation: preprocess, compile, assemble, and link. 
  - flag `-E` stops after preprocessing. extension `i` and called a preprocessed source file
  - flag `-S` stops after compilation. extension `s` and called an assembly code file
  - flag `-C` stops after assembly. extension `o` and called a relocatable object file. you can read relocatable object files with the command `readelf`
  - no flag for linking. called an executable binary. ELF on Linux, Mach-O on macOS, and PE/COFF on Windows
-
