---
layout: post
title: "tar -xzf file.tgz in C++"
excerpt: "Why not?"
tags: [Linux, Code, C++, variant, STL, algorithms, filesystem]
comments: true
---
This post is as always a dump of my rough notes to do what pops up occasionally
and sure enough the Internet comes to the rescue. Although my requirements, were
a little different from what's described here but sufficient for most cases:

### `tar -xzf file.tgz` has multiple parts
```bash
$ tar -xzf file.tgz -
$ # it is functionally equivalent to
$ gunzip file.tgz
$ # this results in file.tar
$ tar xf file.tar -C /tmp
```

To achieve this we can use the following multi-step process in C++
programmatically:

### Unzip or inflate the compcompressed archive
This is achieved by using the `zlib` project.

```c++
#define BUF_SZ 16384
void my_gunzip(gzFile_s *src, FILE *dest) {
    unsigned char buf[BUF_SZ];
    for (auto sz = gzread(src, buf, BUF_SZ); sz > 0; sz = gzread(src, buf, BUF_SZ) {
        std::fwrite(buf, 1, BUF_SZ, dest);
    }
}
```

### Untar file 
I found [this
implementation](https://github.com/mmore500/emscripten-targz/blob/master/untar.h)
pretty helpful in doing this. Although, it appears that this can be converted
into C++ from the current C-style.

#### References:
+ [StackOverflow](https://stackoverflow.com/questions/10195343/copy-a-file-in-a-sane-safe-and-efficient-way)
+ [zlib APIs](https://www.ncbi.nlm.nih.gov/IEB/ToolBox/CPP_DOC/lxr/source/include/util/compress/zlib/zlib.h)
+ [emscripten blog](https://mmore500.com/2020/01/26/emscripten-targz.html)
