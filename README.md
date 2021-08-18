# sbfi
A simple [brainfuck](https://en.wikipedia.org/wiki/Brainfuck) interpreter.

_Brainfuck_ is an esoteric programming language created in 1993. Focused on **extreme** minimialism, _brainfuck_ is composed of eight(!) commands to operate on a (practically) infinite tape of mutable, integral cells.

## Dependencies
- _None_

Just make sure you still have the _C_ standard libs.

## Usage
`$ brainfuck <your program file here>`
### Error codes
If an error happened while interpreting your program, you will be given an error code.
<table>
  <tr>
    <th>Error Code</th>
    <th>Meaning</th>
  </tr>
  <tr>
    <td>1</td>
    <td>The interpreter ran out of memory to allocate.</td>
  </tr>
  <tr>
    <td>2</td>
    <td>An I/O operation failed while interpreting the file.</td>
  </tr>
  <tr>
    <td>3</td>
    <td>No file was given to read.</td>
  </tr>
  <tr>
    <td>4</td>
    <td>The file could not be opened.</td>
  </tr>
  <tr>
    <td>5</td>
    <td>No closing bracket was found while looking for one.</td>
  </tr>
  <tr>
    <td>6</td>
    <td>A closing bracket was found while not looking for one.</td>
  </tr>
</table>

## Build
The build process will need the following applications:
- _make_ (Optional)
- _sh_ (Optional)
- A compiler of your choice

### Without Make
Without _make_ you will need to compile on the commandline. It should be easy since _sbfi_'s source code is a single file.
With _gcc_, for example, run:

`gcc main.c -o brainfuck`

### Without Sh
Without _sh_ (a shell linked to _/bin/sh_) you will not be able to run the test script through _make_.
You can still test it with your own programs or with the provided test file.

Running the provided test file:

`brainfuck hello.bf`

should print __*Hello World!*__.

### Building and Testing with Make
Otherwise, you can build the project with either:

`make`, `make build`

or

`make test`
