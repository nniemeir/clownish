# ClowniSH 🤡 
A cartoonishly opinionated shell. Use at your own risk.
The views expressed by this shell are not my own, they are inspired by comments I've seen in the FOSS community that have stuck out to me.

## Dependencies
* GNU readline development libraries

## Shell Features
* Executes commands via execvp
* Built-in commands (cd, exit, help)
* Background command execution
* Resolves environment variables
* Output stream redirection
	* Write mode (>)
	* Append mode (>>)
* Saves command history via GNU Readline

## Additional Tomfoolery
* Refers to the user by value of $USER, or Keith
* Randomly overrides the behavior of common commands
* Refuses to launch programs it disapproves of
* Teases the user based on the following:
	* Desktop Environment / WM
	* Kernel version
	* Usage of common kernel forks
	* Common command typos
	* Terminal emulator being used

These features can be disabled by enabling polite mode.

## Usage
```
clownish [OPTIONS]
```

### Options
```
-h                      Display program usage
-p                      Enable polite mode
-v                      Show version info
```

## License
The MIT License (MIT)

Copyright (c) 2025 Jacob Niemeir
