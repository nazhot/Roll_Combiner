<p align="center">
  <a href="https://noahzydel.com">
    <img alt="Noah Logo" height="128" src="./.github/resources/NoahLogo.svg">
    <h1 align="center">Noah Zydel</h1>
  </a>
</p>

---

- [📖 Overview](#-overview)
- [⭐️ Current Version](#-current-version)
- [🔜 Hopeful Features](#-hopeful-features)
- [🪚 Built With](#-built-with)
- [🔨 Build Instructions](#-build-instructions)

# Roll Combinations
A tool created for my company to aid in the creation of orders from various rolls.

## 📖 Overview
A group is made up of a combination of unique rolls, and groups are constrained by a minimum and maximum overall length, as well as a maximum number of rolls that can be used to make them up.

From these groups, orders can be made. The only explicit limitations on orders are a minimum/maximum on their overall width. Because of the restrictions on groups, there will also be limits on how many groups/rolls will make up a valid order.

## ⭐️ Current Version
v0.0.1
- Read given file of roll id's/lengths
  
## Previous Versions
N/A

## 🔜 Hopeful Features
  
## 🪚 Built With
- C

## 🔨 Build Instructions
After forking and cloning, navigate to the repository in your command line. Build the project using 
```
gcc -o main main.c -lm
```
Then, run it using
```
./main path-to-roll-file
```
