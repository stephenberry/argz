# argz
A light weight C++ in memory argument parser.

## Highlights

* Single header file
* Requires C++20
* Less than 200 lines of code
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
std::optional<int> number_opt{};
argz::options opts{
   { { "input", 'i' }, input, "the input file"},
   { { "study", 's' }, study, "a study file"},
   { { "number" }, number, "input an int"},
   { { "boolean" }, boolean, "a boolean" },
   { { "number_opt" }, number_opt, "input an int"}
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

`-h` prints a help message, including the program usage and information about the arguments registered with the Argz Parser. An example help message:

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

### Supported Input Types

`bool`, `int32_t`, `uint32_t`, `int64_t`, `uint64_t`, `std::string`

And `std::optional<T>` where `T` is any of the above types except `bool`

### Accept No Inputs Without Printing Help

By default the help is printed when no inputs are given. If this behavior is not desirable, set `print_help_when_no_options` to false inside of `argz::about`.

```c++
argz::about about{ "My program description", version,
                 .print_help_when_no_options = false };
```

