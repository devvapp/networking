*aget - Network retriever. A basic implementation of wget. 


**Installation instructions
$ wget http://libdill.org/libdill-2.4.tar.gz
$ tar -xzf libdill-2.4.tar.gz 
$ cd libdill-2.4

**To enable TLS and compile
$./configure --enable-tls
$ make
$ sudo make install



**Compiling
cc -o aget aget.c -ldill

**Usage
$ ./aget http www.github.com /devvapp
$ ./aget http www.uselesswebpage.com /page.html