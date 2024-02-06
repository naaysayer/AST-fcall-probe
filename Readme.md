AST-fcall-probe
===============

Overview
--------

Tool for extracting signatures of called functions from source code.
Standard library functions are not processed.
The result is output in json format, so you could use 'jq'.

Prerequisites
-------------

*   LLVM

Usage
-----

fcalls [options] <source0> [... <sourceN>]


Call for the source file
```bash
    ./ffcals src/source.cc
```

The utility uses the standard clang parser for input parameters,
so you can use '-p' to specify 'compile_commands.json' path

```bash
    ./ffcals -p build/ src/source.cc
```

You can use jq 'select' to filter what you need
```bash
   ./fcalls -p . src/source.cc| jq '.[] | select(.argc == 1)'
```

Notes
------

Code tested only on llvm 17.0.6 and MacOS :)

For Linux checkout 'linux' branch

License
-------

This project is licensed under the MIT License - see the LICENSE file for details.

