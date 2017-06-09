# ezsh User's Guide

[TOC]

## Introduction

ezsh means easy shell. Our goal is make shell use easy.

This manual assumes that ezsh has already been installed and ready to use. For instructions on how to build and install ezsh, see the ezsh Installer’s Guide below, or the README in the top-level ezsh directory. This manual explains how to use ezsh and some detail of ezsh. This is a preliminary version and some sections are not complete yet. However, there should be enough here to get you started with ezsh. 

## Require and Build

### Require

[The GNU Readline Library](http://cnswww.cns.cwru.edu/php/chet/readline/rltop.html)

If you are using a Ubuntu or Debian OS, just use following command to install

```shell
apt-get install libreadline-dev
```

### Build

To build the code successfully, do the following:

```shell
./configure
./make
./make install
```

## Tutorial

### Learning ezsh

This tutorial assumes a basic understanding of command line shells and Unix commands, and that you have a working copy of `ezsh`.

When you start `ezsh`, you should see this:

```shell
Note: Welcome to ezsh!
shell>
```

### Running Commands

`ezsh` runs commands like other shells: you type a command, followed by its arguments. Spaces are separators:

```shell
> echo hello world
hello world
```

You can write multiple command in a line and separate with `;` , following usage are allowed in `ezsh` :  

```shell
ls; ls -l    ; echo "Hello World"
```

as we can see, extra spaces will not obstruct identify.

You can use *Escape symbol `\`* at the end of a line to concat two lines, below input

```shell
ls -\
l\
-rt
```

are equal to 

```shell
ls -l -rt
```

###Exit ezsh

There server ways to exit ezsh:

1. combination key:

   If you are using linux or cgwin in Windows, `^D` should worked.

   If you are a mac user, try `command+D` or `^D` ? (no test)	

2. builtin command `exit`

   builtin command `exit` will ignore uppercase or lowercase, thus `exit` , `EXIT` and `ExIt` or worked

### Wildcards

`ezsh` supports the familiar wildcard `*`. To list all JPEG files:

```shell
> ls *.jpg
lena.jpg
meena.jpg
santa maria.jpg
```

### Pipes and Redirections

Pipes between commands with the usual vertical bar are not supported now, I will add this function ASAP.

stdin and stdout can be redirected via the familiar` <` and` >`. Unlike other shells, in order to make realize easy, stderr is redirected with a caret `^`

```shell
grep "Hello world" < intput.txt > ~/output.txt ^ ~/errors.txt
```

If you want to redirect and append, just use those symbol twice `>>` and `^^`

> Note: if you use same redirection symbol multi-times,  only the first one will worked.
>
> For example, if you input command like this:
>
> ```shell
> ./a.out > output1.txt > output2.txt
> ```
>
> The stdout of a.out will only redirect to output1.txt

### Tab Completions

Press **Tab**, and `ezsh` will attempt to complete the command or path:

```shell
> /pri @key{Tab} → /private/
```

> Note: ezsh builtin command are not support tab completions now, I will try to fix this next edition.

 ### Command History

In order to show and manipulate command history, ezsh proived 

1. Builtin command `history`

   Builtin command `history` will show the command histroy. ezsh will not save history file if shell was terminated. 

   ```shell
   shell>history
   history
   1: ls
   2: echo Hello, World
   3: ls -lt
   4: history
   ```

2. Event Designators

   An event designator is a reference to a command line entry in the history list. Unless the reference is absolute, events are relative to the current position in the history list.

   `!n`

   ​	Refer to command line n.

   `!-n`

   ​	Refer to the command n lines back.

3. Scroll History 

   You and use `↑` and `↓` on keyboard to scroll history. 


### Exports(Shell Variables)

Like most of shells, ezsh uses `export`command to export shell variables.But the usuage of `export` command are different.

1. If you want to assign a variables, you can use like 

   ```shell
   export PREFIX = "My shell"
   ```
   or

   ```shell
   export PREFIX --assign "My shell"
   ```

2. If you want to add some thing to a variables, you can use like this:

   ```shell
   export PATH += ":/home/"
   ```

   or

   ```shell
   export PATH --append ":/home/"
   ```

   > Hint: you should add ':' manually, or will cause fatal error


3. If you want to remove a variables, you can use like this:

   ```shell
   export PATH -d
   ```

   or 

   ```shell
   export PATH --delete
   ```

> Please use spaces as separators, or undefined operation may happend.

### Shell Prompt

As default, `ezsh` will output `shell>`to terminal. If you want to change the shell prompt.

Just manipulate PROMPT variables in ezsh. You can change the shell prompt with your favor like this:

```shell
export PROMPT = "VivianSnow's Shell"
```

> BE CAREFUL, `PROMPT` variable's  spell are all upper case

### Job control

ezsh will create a job for every command and ezsh also support job control.

If you want a command run on the background, just add `&`  at end of the command. You can use like this,

```shell
shell>./a.out &
```

> `&`symbol must at the end of command, or ezsh can't detect it.

Like most of shells that support job control, you can use `jobs` builtin coomand to list the jobs and there's status.

The following two builtin command are used to control jobs run between foreground and background.

`fg n`

​	The jobs that number equal to n will divert into foreground. If job is stoped, `ezsh` will sent SIGCONT to it.

`bg n`

​	The jobs that number equal to n will divert into foreground. If job is stoped, `ezsh` will sent SIGCONT to it.	

## Builtin Command

I have add some builtin command now, including 

`exit`

​	exit ezsh, see Exit ezsh part above

`!`

​	see Command History part above

`export`

​	see Exports(Shell Variables) above

`cd` 

​	I design it, but now it was just a empty command.

`source`

​	I design it, but now it was just a empty command. In fact, I have no idea how to run a script now, thus I will finish this command after study more.

`fg`

​	see Job control part above

`bg`

​	see Job control part above

`jobs`

​	see Job control part above	

`history`

​	see Command History part above

More builtin  command will be added in next edition.

## Author and Bug Report

I'm a Master Candidate of Beijing Uninveristry of Posts and Telecommunication.ezsh is practice project after reading APUE and learning APUE lecture which instructed by hoplee(hoplee#bupt.edu.cn).

If you have any good idea or meet any bugs, please contact me at viviansnow@bupt.edu.cn