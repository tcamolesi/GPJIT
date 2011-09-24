GPJIT - Genetic Programming with JIT assembler
==============================================

Introduction
------------

GPJIT is a simple genetic programming software that tries to create an
algebraic expression as an approximation to an unknown R -> R function. It''s
innovative in the sense that it generates x86 assembly instructions in order to
improve the speed of fitness evaluation of each individual in the population.

GPJIT was developed as an assignment for the course "Introduction to Genetic
Algorithms in Robotics" at University of São Paulo, circa 2009. 

Code Overview
-------------

The expression trees are represented in a traditional way using the ETTree and
Element structs. Each branch of the tree may contain an arithmetic operator or
a function with their parameters/operands in the child nodes. Each leaf of the
tree contains either a constant value or a reference to a variable.

The Assembler object traverses an expression tree in postorder DFS, outputting
x86 assembly directly - this is possible because the x86 FPU is stack-based.

Fitness Evaluation
------------------
The fitness function is simply evaluated as the sum of the absolute errors,
i.e.  $abs(freal(xi) - fapprox(xi))$ over an interval (as defined in
*ga.c: evalFitness*).

Genetic operators
-----------------
GPJIT generates, mutates and applies cross-over operators to the expression
tree in order to improve the population fitness, as suggested by Koza.

Limitations
-----------

The assembly code generated was created by hand using nasm and, therefore, will
only work in the platform in which it was developed: x86 Linux 32bits. The generated
code may be ported to a 64 bit platform by changing the code to be generated in the
assembly.c file. A better and more generic solution could be developed by using
a platform-agnostic JIT library, such as *libjit*.

Due to the simplistic approach to code generation, it was a design decision to
limit the generated functions to those that could be evaluated using only the
FPU stack of an x86 machine, without resorting to local variables. Due to the
limited space of the FPU stack, this implies that the maximum tree depth that
can be handled by the program is 6.

Dependencies
------------

GPJIT depends on the allegro graphics library to render an on-screen
visualization of the function being approximated VS the best individual of the
population.

In a debian-based distribution:

    $ sudo apt-get install liballegro4.2-dev*

Build Instructions
------------------

    $ cd src
    $ make
    $ ./gpjit

License
-------

The MIT License

Copyright (c) 2011 Tiago Camolesi

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
of the Software, and to permit persons to whom the Software is furnished to do
so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
