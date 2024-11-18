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

PseudoCode:<br />

class DictionaryEncoder{<br />
Encode: <br />
- Iterates through a column of strings <br />
- Maps each unique string to a hash map <br />
- sores integer encoding in encoded Column <br />
Query: {<br />
- Checks if string exists in dictionary<br />
- returns index if found, false if not<br />
Prefix Scan: 
{<br />
- finds all dictionary keys with given prefix <br />
- compares each key with prefix an collects all index cases of it<br />
- returns vector of all matching indexes<br />
}
<br />
class KeyValueStore{<br />
put:<br />
- insert of updated key pair in store <br />
get: <br />
- retrieves values associate with key<br />
- return false if key does not exist <br />
del:<br />
- removes key/value pair from store <br />
}
<br />
vanillaColumnScan{ <br />
// demonstrates a linear scan <br />
- iterate through the column and print every element <br />
}
<br />
MultiThreading{<br />
// demonstrates concurrent encoding with threads<br />
- int numThreads <br />
- divides column into 'numThreads' number of chunks <br />
- assign each chunk to a thread <br />
- encode each in parallel <br />
}
<br />
neonQuery{<br />
// performs SIMDNeon based search for a specific value in a vector <br />
- load chunks of 4 integers into NEON register <br />
- compare them against target value <br />
- extract comparison results <br />
}
<br />
neonPrefixScan{<br />
// performs SIMDNEon based prefex scan<br />
- load prefix and key strings into NEON registers <br />
- Compare prefix and key strings into NEON registers <br />
- compare prefix and keys <br />
- appends matching indexes into results vector <br />
}
<br />
testQueryPerformaceSIMD{<br />
- call a single query scan using DictionaryEncoder query <br />
- call SIMDNeon single query scan using neonQuery function <br />
- call prefix scan using DictionaryEncoder prefixScan <br />
- call SIMD prefix scan using neonPrefixScan function <br />
}
<br />
main{<br />
- encode a column of strings using DictionaryEncoder class <br />
- contains all functions for testing <br />
}
