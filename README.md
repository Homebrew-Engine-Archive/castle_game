## Coding style conventions

1.0. About project source files
-----------------
* C++ Header extension is `.h`.
* C++ Source code files extension is `.cpp`.
* Source and headers both should be all lower case, shouldn't contain spaces and be written ideally without underscores.
Here are some truly \m/ formatted names:
`yourjamsir.h`
`interfacecastle.cpp`
`archers_ready.h`
* Place related headers and sources in the same directory.
* Only UTF-8 in source files. Seriously.

1.1. Basic rules
----------
* C++-style casts is better than C-style.
* Templates and inlines is better than macroses.
* Don't forget include guards. It might be written LIKE_THIS_H_ or somehow else.
* Avoid compiler's black magic like `#pragma`s or `__attribute__`s.
* Don't inlcude headers you don't use.
* Forward declare classes, enums and functions you really use.
* Put your enum and class forward declarations on top of header file.
* Isolate your forward decls from fully declared names (as I do).
```
namespace NS
{
    class IncompleteClassName;
}
...
namespace NS
{
    class ClassManager
    {
    public:
        virtual const IncompleteClassName GetClass() const;
    };
}
```
1.2. C++ Exceptions
-------------------
* Feel free to throw errors, misunderstandings, just throw.
* Derive your exceptions from std::exception.
* Scoped resource management is better than manual.

1.3. Boost Features
--------------
* Boost at your service, Sir.

1.4. C++11 Features
-------------------
* Freely use any C++11 features. It's your future!
* `auto` is unwelcome. `auto&` and `auto*` either.
* Use anonymous namespaces for your static methods, types and constants.
* Look at `constexpr`. It's better than simply inline your functions.
* Provide both default and deleted constructors and assignment operators.

1.5. Namespaces
--------------
* Explicit namespace is better than usings.
* `using po = boost::program_options` is better than `boost::program_options::options_descriptions = boo...`
* Encapsulating your classes and functions into proper namespaces is encouraged.
* Namespace name should consist of single::word::either::Capitalized::or::not. 

1.6. Declarations and Definitions.
------------------------------------------------------
* Avoid multiple variable declaration.
* `const` is very encouraged.
* `constexpr`, `template` and `inline` functions and classes place into headers.

1.7. Formatting rules
---------------------
* General formatting rule - K&R with 4 spaces.
* CamelCase for classes and functions.
* mixedCase for variables.
* Prefix class mMembers with just single `m...` or none.
* underscores_style_t for type aliases.
* Long lines are good.
* Long function or type composition are bad.
* You may place `&`, `&&` and `*` whereever you want.
* You may place variable specifiers anywhere, but prefer the following notations:

```
ReturnType const* FunctionName(const Argument &argumentWithName)
{
    // <body>
}
```

or

```
ReturnType const& FunctionName(Argument const&);
```

Here is simple header example:

```
#ifndef HEADER_H_
#define HEADER_H_

namespace Header
{
    class HeaderClass
    {
    public:
        explicit constexpr HeaderClass(int num);
    };
}

#endif
```

Here is simple function's body:

```
std::istream& ReadBook(std::istream &in, Game::Objects::Book &book)
{
    int numPages = 0;
    in >> numPages;

    book.SetPagesCount(numPages);
    
    for(Game::Objects::Page &page : book.Pages()) {
        ReadPage(in, page);
        if(!in) {
            break;
        }
    }
    
    if(!in) {
        throw std::runtime_error("Stop read your book, let's code.");
    }
    
    return in;
}
```

## Install

## Run

## License

## Troubleshooting

That is Scott Meyer's *Effective C++ 3rd*. Read it?

## Thanks

Your tired, my Lord?