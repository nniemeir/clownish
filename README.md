# ClowniSH 🤡 
A cartoonishly opinionated shell. Use at your own risk.
The views expressed by this shell are not my own, they are inspired by comments I've seen in the FOSS community that have stuck out to me.

## Demo
![clowniSH demonstration](assets/demo.gif)

## Dependencies
* GCC
* GNU readline development libraries
* GNU make
* gzip
* TCL Expect (for tests)

## Installation
Compile the project
```
make
```
Test functionality (optional)
```
make test
```
Install the manpage and compiled binary
```
sudo make install
```

### Make Targets 
- `make` - Compile the binary
- `make test` – Run test suite
- `make install` – Copy binary and manpage to system directories
- `make clean` – Remove build objects
- `make fclean` - Remove build objects and binary

## Shell Features
* Executes commands via execvp
* Built-in commands (cd, exit, help)
* Background command execution
* Resolves environment variables
* Pipes
* Input stream redirection
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
-d			Enable debug mode (use fallback prompt and disable color override)
-h                      Display program usage
-p                      Enable polite mode
-v                      Show version info
```

## Contributing
This project was primarily created for my education, so I am not currently accepting PRs at this time. If you'd like to fork the project, might I advise calling it libreClowniSH?

## License
The MIT License (MIT)

Copyright (c) 2025 Jacob Niemeir
