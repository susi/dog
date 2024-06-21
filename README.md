# DOG

For the latest information about DOG see the [DOG Operating Guide](https://dog.zumppe.net/)

## What is DOG?

![DOG logo](https://dog.zumppe.net/img/logo.png)

DOG is a DOS SHELL, or command interpreter. DOG works on most, if not all flavors
of DOG. I mainly target [FreeDOS](https://freedos.org/), but regularly test on
MS-DOS 3.30 and 6.22 and DR-DOS 6.0 and 7.02.

DOG works similar to COMMAND.COM or 4DOS, but is different. First and probably
most importantly DOG is **not** drop-in compatible with neither COMMAND.COM nor
4DOS.  DOG does things differently. Second difference is that DOG commands are
always just 2 letters, but still easy to remember. DOG also offers external
commands allowing them to be more complex, and keeping the core of DOG
smaller.

DOG has 2 classes of commands: INTERNAL and EXTERNAL. The internal commands are
built-in to the DOG.COM binary and are always in memory and access the internals
of DOG. There are 2 kinds of internal commands. Regular user commands and the
batch file programming commands. The external commands are stand-alone
executables, but considered to be part of DOG, and may access DOG internals where
needed through an API.


*  Syntax: `DOG [-P|-E envsz|-A aliassz|-C command line]`
*  Parameters:
   *  -P - Makes DOG a PERMANENT shell.
   *  -E envsz - Makes the environment to envsz bytes (divisible by 16).
   *  -A aliassz - Makes the alias space to aliassz bytes (divisible by 16).
   *  -C command line - Executes the command line and exits.

## A note about DOG version numbers

DOG uses the following version scheme:

```
0.8.4b
------
| | ||
| | |+-- Code maturity allways one of a (=alpha), b (=beta) OR f (=final)
| | +--- Patchlevel
| +----- Minor version
+------- Major version
```

This versioning scheme means that the maximum version of DOG will be
`15.15.15f`, in hex that will be `FFFFh`. So far DOG has only release
alpha (before version 0.8 and beta releases. The first final version
will be `1.0.0f`.
