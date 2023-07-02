# INI parser
This project was created for Computer Architecture with Low-Level Programming course with my colleague Maksymilian Żmuda-Trzebiatowski.

# Introduction
The goal of the assignment was to create a program to parse INI files. The main functionalities are:
1. Printing the value under the specified section and key pair
2. Detecting missing sections and keys
3. Detecting faulty sections and keys (with prohibited characters in their names) in the INI file
4. Simple expressions support (+, -, * and / for ints and concatenating for strings)

# Usage
## Input formats:
To print the value under the specified section and key pair:

    $ ./program PATH-TO-INI-FILE.ini section.key

To perform operation given by the expression (only +, -, * and / are supported):

    $ ./program PATH-TO-INI-FILE.ini expression "section.key * section.key"