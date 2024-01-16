# Tone Interpreter

## Introduction

This is a tone interpreter that I wrote in C while tinkering around.  It converts commands to waveforms to be sampled by PulseAudio.  The interface is entirely text-based.  As always, this is a hobby-project and nothing more.  There are known bugs and places to improve.  Right now, it works well enough for single waveforms per tone.

I included a file, `test.tn` as a known good test file for tones.

## Compilation

### Dependencies

* PulseAudio (and headers)

### Makefile

The Makefile has a few different commands, which are listed below:
* `make`: An alias for `make tone-interpreter`.
* `make tone-interpreter`: Simply compile the `tone-interpreter` binary.
* `make clean`: Remove any compiled binary.
* `make remake`: Remove any compiled binary and run `make`.  *This is useful when making edits to the code.*

## Usage

To run, `$ ./tone-interpreter`.  There will be no prompt for command input, but you may enter your command.  Alternatively, you can pipe in a command or use a file to be read in (using `echo <CMD> | ./tone-interpreter` or `./tone-interpreter < <FILE>`, respectively).

### Command / Tone Language

A custom command/tone language is used in the format `DURATION/TYPE,FREQUENCY,VOLUME;`.  An valid example is: `1.2/0,440.0,10000.0;`.  The fields are described below.

* **Duration:** Time for the tone to be active in seconds.
* **Type:** The type of waveform.
    * **0:** Sine wave
    * **1:** Sawtooth wave
    * **2:** Triangular wave
    * **3:** Square wave
* **Frequency:** Frequency of the wave in Hz
* **Volume:** Volume on the PulseAudio scale.

*Ensure that each command is terminated with a semi-colon!*
*Eventually, there will be the ability to mix different waveforms together for a single tone.*

## Known Bugs & Way Forward

There are several known bugs and ways that this should/can/will be improved.

### Bugs
* Currently, only one command per line works

### Future Work

* Add prompts
* Add command-line options (verbose-mode is currently hard-coded)
* Add ability to parse multiple waveforms into a single tone.

