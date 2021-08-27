# Dancing links

A library that implements
[Donald Knuth](https://en.wikipedia.org/wiki/Donald_Knuth)'s
[Algorithm X](https://en.wikipedia.org/wiki/Knuth%27s_Algorithm_X) to solve
exact cover problems, written in C. The library aims to have a simple interface
and allows defining primary and secondary constraints.

The implementation folows closely the one described in Knuth's
[paper](https://arxiv.org/pdf/cs/0011047.pdf).

## Making

To build the library use `make`, to also build the examples `make example`. The
resulting `libdlx.a` will be put in the `lib` folder and the examples in the
`bin` folder.

## Usage

You can find examples of the use of the library in the `examples` folder.
