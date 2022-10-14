# argz
A light weight C++ in memory argument parser.

## Highlights

* Single header file
* Requires C++17
* Apache 2.0 License

## Quick Start

Simply include `argz.hpp` and you're good to go.

```cpp
#include <argz/argz.hpp>
```

Start with an about:  `argz::about`

```cpp
constexpr std::string_view version = "1.2.3";
argz::about about{ "My program description", version };
```

To add a new argument, simply create ```argz::options```. Provide a list of argument names that you want to group together, e.g. ```-i``` and ```--input```.

```cpp
std::string input{};
std::string study{};
int number = 123;
bool boolean = true;
argz::options opts{
   { { "input", 'i' }, input, "the input file"},
   { { "study", 's' }, study, "a study file"},
   { { "number" }, number, "input a double"},
   { { "boolean" }, boolean, "a boolean" }    
};
```

Now to parse command line arguments:

```cpp
try {
    argz::parse(about, opts, argc, argv);
}
catch (const std::exception& e) {
    std::cerr << e.what() << '\n';
}
```

**That is all!**

> Note that we don't have to cast out (.e.g. `.as<int>`) from our argument parser because it reads directly into the variables passed in as options!

### Printing Help

`-h` prints a help message, including the program usage and information about the arguments registered with the `Argz Parser`. An example help message:

```
My program description
Version: 1.2.3

-h, --help              write help to console
-v, --version           write the version to console
-i, --input             the input file
-s, --study             a study file
--number                input an int, default: 123
--boolean               a boolean, default: 1
```
