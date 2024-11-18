# Advanced-Computer-Systems-Project-4

Installs for this project:<br />
- clang compiler (brew install gcc47)

Notes:
- SIMD is not compatible with Mac M1s - used ARM NEON (Documentation: https://developer.arm.com/documentation/den0018/latest/)
How to use:
- Download main.cpp
- to change thread number, change variable 'numThreads' in multiThreading
- to change vector size, change variable 'column' in main()
- to change test, uncomment corresponding function(s) in main()
<br />
<br />
PseudoCode:
class DictionaryEncoder{
Encode
- Iterates through a column of strings
- Maps each unique string to a hash map
- sores integer encoding in encoded Column
query
- Checks if string exists in dictionary
- returns index if found, false if not
Prefix Scan
- finds all dictionary keys with given prefix
- compares each key with prefix an collects all index cases of it
- returns vector of all matching indexes
} 
<br />
class KeyValueStore{
put:
- insert of updated key pair in store
get:
- retrieves values associate with key
- return false if key does not exist
del:
- removes key/value pair from store
}
<br />
vanillaColumnScan{
// demonstrates a linear scan
- iterate through the column and print every element
}
<br />
MultiThreading{
// demonstrates concurrent encoding with threads
- int numThreads
- divides column into 'numThreads' number of chunks
- assign each chunk to a thread
- encode each in parallel 
}
<br />
neonQuery{
// performs SIMDNeon based search for a specific value in a vector
- load chunks of 4 integers into NEON register
- compare them against target value
- extract comparison results
}
<br />
neonPrefixScan{
// performs SIMDNEon based prefex scan
- load prefix and key strings into NEON registers
- Compare prefix and key strings into NEON registers
- compare prefix and keys
- appends matching indexes into results vector
}
<br />
testQueryPerformaceSIMD{
- call a single query scan using DictionaryEncoder query
- call SIMDNeon single query scan using neonQuery function
- call prefix scan using DictionaryEncoder prefixScan
- call SIMD prefix scan using neonPrefixScan function
}
<br />
main{
- encode a column of strings using DictionaryEncoder class
- contains all functions for testing
}
