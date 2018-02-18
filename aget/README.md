# aget - Network retriever. A basic implementation of wget. 


## Installation instructions
wget http://libdill.org/libdill-2.4.tar.gz<br/>
tar -xzf libdill-2.4.tar.gz<br/>
cd libdill-2.4<br/>

## To enable TLS and compile
./configure --enable-tls<br/>
make<br/>
sudo make install<br/>



## Compiling
cc -o aget aget.c -ldill<br/>

## Usage
./aget http www.github.com /devvapp<br/>
./aget http www.uselesswebpage.com /page.html<br/>