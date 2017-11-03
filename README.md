# r-csvf
R package for fast csv file processing, especially for very large csv files which can not be load to memory at once. The functionalities are implemented using C++ and wrapped as R6 Classes and R functions. Note that, the functionalities are also available to C++ users as a C++ library: [csvf](https://github.com/gongliyu/csvf)

## Installation
### Install from github
Install the devtools package first, then

``` R
devtools::install_github("gongliyu/r-csvf")
```

## Usage
### Read csv files
Reading can be done through the csvf::Reader class

``` R
reader <- csvf::Reader$new("a.csv") # construct a reader object
df <- reader$read(10) # read at most 10 records as a data.frame
```
while constructing the reader object, it will automatically detect the field separator character (e.g. ','), end-of-line characters, number of fields, quote rules etc. 
