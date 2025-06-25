<p align="center">
  <img src="./logo/bumblebee3.png"  width=50% />
</p>

# BumbleBee DB

## Features

- Datalog as input language
- High-performance engine with code generation, push-based execution, columnar storage, and multithreading
- Available as a command-line interface (CLI) and a Python client library
- Supports data sources including CSV, ASP, and Parquet files
- Designed for data engineers: easy to debug and customize internal optimizations


### Build parser
```
	flex -o src/parser/aspcore2_lexer.hpp src/parser/aspcore2.l
	bison -y -d -o src/parser/aspcore2_parser.c src/parser/aspcore2.y
	bison -y -o src/parser/aspcore2_parser.hpp src/parser/aspcore2.y
```
