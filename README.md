# copy_model
## How to compile

```bash
g++ -o bin/cpm src/cpm.cpp
```
## How to run

Below are the parameters to run the program.
All parameters are optional except the path to the file.

```bash
./bin/cpm -k 8 -a 1 -t 0.5 -s -f <path to file>  
```
- parameter k: Size of the window and anchors; default value is 8
- parameter a: alpha; smoothing parameter; default value is 1
- parameter t: cutoff threshold; default value is 0.5
- parameter f: path to the file for the program to process  

## Example

```bash
./bin/cpm example/chry.txt
```