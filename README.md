# Orange
Orange is a systems programming language made to let the user create code that's as powerful as C++ while still being fun to write. 

# Status 

Orange is still under development and in the design phase; a lot of planned features are not yet implemented. Check the [TODO](/TODO.md) to see what needs to be done.

# Hello World
At the moment, Orange has no standard library, so you'll need to link against C library functions to do more 
complicated tasks:

    # test.or
    extern int32 printf(char *s, ...)

    for (var i = 0; i < 10; i++) 
        printf("Hello, world %d!\n", i)
    end
    
Run directly with `orange run test.or` or build a native binary with `orange build test.or` and run with `./a.out`. 
    
# Resources

* [Language Guide](https://github.com/orange-lang/orange/wiki/Language-Specification)
* [Orange TODO features](/TODO.md)
* [Tests](/test/)

# Installing 
Installing on OS X and Linux should be pretty simple; after having all of the dependencies installed, building is as easy as:

```sh 
$ mkdir build-orange && cd build-orange 
$ cmake ../orange
$ make all install
``` 

[Building on Windows](https://github.com/orange-lang/orange/wiki/Building-on-Windows) takes a bit more effort.

## Dependencies 

* Boost 
* Flex
* Bison
* CMake
* LLVM
