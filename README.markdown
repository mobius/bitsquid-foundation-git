# mini\_foundation

*mini\_foundation* is a minimalistic foundation library with functions for memory allocation, data manipulation, etc.

## Library Design

*mini\_foundation* has been written with data-oriented programming in mind (POD data is preferred over complicated classes, flat arrays are the preferred data structure, etc).

mini_foundation is written in a "back-to-C" style of C++ programming. This means that there is a clear separation between data and code. Data defitions are found in \_types.h header files. Function definitions are found in .h header files.

When you are writing a system using mini\_foundation, your header files typically only need to include the \_types.h files. These are designed to be as compact as possible, meaning your compile times will be short. In the .cpp files you can include the heavier .h files.

mini_foundation uses the following types of data:

    // Open structs
    struct Vector3 {
    	float x, y, z;
    };

    // Closed structs
    struct Object {
    	Object();
    	unsigned _data;
    };

    // Abstract classes
    class Allocator;

    class Allocator {
    	public:
    		void *allocate(uint32_t size, uint32_t align) = 0;
    };

* **Open structs.** Raw POD structs defined in the \_types.h file. You can directly manipulate the members of these structs.

* **Closed structs.** A closed struct is indicated by the fact that its members start with an underscore. You should not manipulate the members of a closed struct directly. Instead, use the functions in the namespace with the same name as the struct. (In this case: object::). These functions are found in the .h file, unlike the struct definition, which is in the \_types.h file.

Note that since namespaces are "open" you can extend the functionality for the object by adding your own functions to its namespace.

* **Abstract classes.** Used for high-level systems that are only accessed through pointers and/or references. These are predeclared in the \_types.h file. The virtual interface is found in the .h file.

## Systems

### Memory

* **Allocator** A virtual base class for memory allocation. Can be subclassed to implement custom allocator behaviors.

* **memory_globals::default_allocator()** Returns a default allocator based on malloc().

### Collection

* **Array<T>** Implements an array of objects. A lightweight version of std::vector that assumes that *T* is a POD-object (i.e. constructors and destructors do not have to be called and the object can be moved with memmove).

### Math

* Basic data definitions for vectors, quaternions and matrices.