## Simple language to asm translator

University project from 2014.

Translates simple programming language to asm. Uses flex and byson syntax / lexic analyzers. 

**Usage example:**

```
./build.sh

./SimpleTranslator ./examples/fibonacci_sequence -o fibonacci_sequence.asm

./buildAsm.sh fibonacci_sequence

./fibonacci_sequence
```
