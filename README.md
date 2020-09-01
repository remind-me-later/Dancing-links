# Dancing-links

A C library that implements Knuth's Algorithm X to solve exact cover problems.
The library aims to have a simple interface and allows defining primary and secondary constraints.
The implementation folows closely the one described in Knuth's [paper](https://arxiv.org/abs/cs/0011047).

## Making

To build the library use
```
make
```
The resulting `libdlx.a` will be put in the `lib` folder.

To build examples
```
make example
```
The example programs will be put in the `bin` folder.

The debug and profile targets compile the program to be analyzed by Valgrind and gprof respectively.

## Examples

You can find examples of the use of the library in the 'examples' folder.
