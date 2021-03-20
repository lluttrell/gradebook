# Gradebook CTF

`gradebook` is my final project for a course in computer security. It is designed to look and feel like a simple "unix-style" command-line tool for submitting and viewing university grades. The program is written to be exploited, not to be used for any actual purpose.

## Installation

To install using make, it is required that the user have administrative privilleges. This is because special hard-coded users and groups must be added to the system. It is asolutely critical that the compiled binary is not owned by a user with actual sudo privilleges. Therefore to safely use and exploit the software with the makefile, please follow the prerequisite instructions.

### Prequesites

The makefile assumes the system has a user **without root privileges** named `registrar`. Additionally, it assumes the system has a group named `faculty`. Another fake user may be created and added to the `faculty` group. I used and suggest the name `drwho`. Ensure these users/groups to not already exist before creating.

### Compilation
To compile the software, move it to `/usr/bin/gradebook`, and set up the "database" (really a collection of unencrpyed text files) in registrar's home folder, use the following command:

``` bash
make all
```

## Usage

The program includes a fake manpage that explains how to use the tool. To view the manpage run the programm with the -h flag:

``` bash
gradebook -h
```

## Exploiting

The software is written in such a way that one user may gain access to another user's transcript. The exploit(s) are obvious enough to identify through code-inspection without actually installing the software. If you have gone through the trouble to actually install the software, it should be reasonablly easy to find the exploit without looking at the code.
