# Automatic Layout Synthesis for Analog Circuits
Current source array placement with common centroid

## How to Compile
In this directory, enter the following command:
```
$ make
```
It will generate the executable file "hw5" in "HW5/bin/".

If you want to remove it, please enter the following command:
```
$ make clean
```

## How to Run
**In this directory, enter the following command:**
```
Usage: ../bin/<exe> <number of current sources> <def file path>
```

e.g.
```
$ ../bin/hw5 4 ../output/CS_4.def
```

**In "HW5/bin/", enter the following command:**
```
Usage: ./<exe> <number of current sources> <def file path>
```

e.g.
```
$ ../hw5 4 ../output/CS_4.def
```

## How to Test
In this directory, enter the following command:
```
$ ../verifier/verify <k-value: 4, 16...> <.def file>
```
It will test on $(number of current sources) and run verifier to verify the answer.

e.g. test on 4 current sources and verify the answer
```
$ ../verifier/verify 4 ../output/CS_4.def
```
