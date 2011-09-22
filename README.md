GPJIT - Genetic Programming with JIT assembler
==============================================

Introduction
------------

GPJIT is a simple genetic programming software that tries to create an
algebraic expression as an approximation to an unknown R -> R function. It''s
innovative in the sense that it generates x86 assembly instructions in order to
improve the fitness evaluation of each individual in the population.

GPJIT was developed as an assignment for the course "Introduction to Genetic
Algorithms in Robotics" at University of São Paulou, circa 2009. 

JIT Code Generation
-------------------

GPUJIT converts expression trees into CPU instructions, making use of the FPU
stack. This simplifies code generation, as no local variables are necessary.

Fitness Evalution
-----------------
The fitness function is simply evaluated as the sum of the absolute errors,
i.e.  *abs(freal(xi) - fapprox(xi))* over an interval (as defined in
*ga.c:evalFitness*).

Genetic operators
-----------------
GPJIT generates, mutates and applies cross-over operators to the expression
tree in order to improve the population fitness, as suggested by Koza.

Limitations
-----------

The assembly code generated was created by hand using nasm and, therefore, will
only work in the platform in which it was developed: x86 Linux 32bits. A better
and more generic solution could be developed by using a platform-agnostic JIT
library, such as *libjit*.

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
