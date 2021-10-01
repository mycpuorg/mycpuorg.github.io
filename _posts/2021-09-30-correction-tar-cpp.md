---
layout: post
title: "Untar In Pure C++ Is A Myth"
excerpt: "`K&R` win again"
tags: [Linux, Code, C++, variant, STL, algorithms, macros, helpers]
comments: true
---

In an [earlier post](http://www.mycpu.org/untar-gunzip-file/), I concluded that
using a pure C++ approach to untarring the archive file may be viable. But given
that there are only so many ways `std::fstream` can be used to read/write chunked
files we effectively do a mean reversion in our implementation to the original
`K&R` approach intentionally or otherwise.

To discuss I would like to go back to the [SO Question](https://stackoverflow.com/questions/10195343/copy-a-file-in-a-sane-safe-and-efficient-way)

FWIW, I agree with the top answer that "KISS-C++-Streambuffer-Way" is the most
readable and intuitive code so you should use it when possible.

However, in the case of a tar/untar code, effectively we are splitting the file
into its constituents by going throug the header info to create the dir
hierarchy with appropriate file permission. Further to the end we perform a
checksum validation.

This requires us to know the sizes of individual files which implies that we
make a local copy into a buffer either chunked or whole size of the file
itself. But copying an entire file results in memory bloat for large archives.
Therefore, I argue that `K&R` win again!

```
Note: In the interest of brevity, I have left out the cleanup code here
```

### ANSI-C-WAY
```c++
    char buf[BUFSIZ];
    size_t size;

    FILE* source = fopen("from.ogv", "rb");
    FILE* dest = fopen("to.ogv", "wb");

    // clean and more secure
    // feof(FILE* stream) returns non-zero if the end of file indicator for stream is set

    while (size = fread(buf, 1, BUFSIZ, source)) {
        fwrite(buf, 1, size, dest);
    }
```

### POSIX-WAY (K&R use this in "The C programming language", more low-level)

```c++
    char buf[BUFSIZ];
    size_t size;

    int source = open("from.ogv", O_RDONLY, 0);
    int dest = open("to.ogv", O_WRONLY | O_CREAT /*| O_TRUNC/**/, 0644);

    while ((size = read(source, buf, BUFSIZ)) > 0) {
        write(dest, buf, size);
    }
```


### KISS-C++-Streambuffer-WAY

```c++
    ifstream source("from.ogv", ios::binary);
    ofstream dest("to.ogv", ios::binary);

    dest << source.rdbuf();
```

### COPY-ALGORITHM-C++-WAY
```c++
    ifstream source("from.ogv", ios::binary);
    ofstream dest("to.ogv", ios::binary);

    istreambuf_iterator<char> begin_source(source);
    istreambuf_iterator<char> end_source;
    ostreambuf_iterator<char> begin_dest(dest); 
    copy(begin_source, end_source, begin_dest);
```

### OWN-BUFFER-C++-WAY

```c++
    ifstream source("from.ogv", ios::binary);
    ofstream dest("to.ogv", ios::binary);

    // file size
    source.seekg(0, ios::end);
    ifstream::pos_type size = source.tellg();
    source.seekg(0);
    // allocate memory for buffer
    char* buffer = new char[size];

    // copy file    
    source.read(buffer, size);
    dest.write(buffer, size);

    // clean up
    delete[] buffer;
```
