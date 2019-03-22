# c-t9
A T9 typing system written in C11.

c-t9 is a basic T9 typing system implementation that allows you to learn a statistical model from training data to generate suggestions based on entered T9 keys.



## Usage

There are two examples provided inside [main.c](src/main.c).
Both examples train a statistical model from a collection of Tweets from Donald Trump. This training data is placed in the [data/](data/) folder and can be exchanged as needed. Note that characters that are not part of the *Corpus symbols* are stripped away.

### Symbol definitions

* Lexicon symbols (T9 keys): "0123456789*#"
* Corpus symbols: "a-zA-Z0-9 .,"

The corpus symbols are assigned to the keys as follows:

| T9 Key | Corpus Symbols    |
|--------|-------------------|
| #      | " " (space)       |
| *      | "" (not assigned) |
| 0      | "0"               |
| 1      | ".,1"             |
| 2      | "aAbBcC2"         |
| 3      | "dDeEfF3"         |
| 4      | "gGhHiI4"         |
| 5      | "jJkKlL5"         |
| 6      | "mMnNoO6"         |
| 7      | "pPqQrRsS7"       |
| 8      | "tTuUvV8"         |
| 9      | "wWxXyYzZ9"       |

### Completion example

This example shows how a given sequence of T9 keys is used generate a text suggestion based on a learned statistical model.

Here the key sequence `366253#87867` is entered:

```
[Corpus]: Loaded (3092794 bytes) in 1.62 ms.
[Completion]: Typing sequence: "366253#87867"
[Completion]: Suggested: "Donald Trump"
```

### Evaluation example

This example shows how the statistical model learned from a training corpus can be evaluated. For a given T9 symbol sequence the system generates a text suggestion. This suggested text is compared to the known ground truth the T9 key sequence originated from.

Here the evaluation of a model with a ngram length of four and an evaluation text length of 140 characters. For meaningful results longer test sequences should be used:

```
[Corpus]: Loaded (3091934 bytes) in 3.58 ms.
[Evaluation]: ...
[Evaluation]: Suggestion: "from Donald Trump. Wishing everyone a wonderful holiday a happy. Healthy. Prossesour new Year. Lets think Like champions in 2010. Trump Inte"
[Evaluation]: error 0.071, duration: 12137.70 ms.

```

### Parameters

There are two main parameters that control the model creation process:

* **ngram_length**: The Ngram length to use for building the statistical  model.
* **number_paths**: After every T9 key entered, the system generates a suggestion and prunes the internal tree structure. ```number_paths``` defines how many of the best paths (different suggestions) should survive the pruning. Therefore, in the end there exist up to this number of text suggestions for an entered key sequence.



## Build

### Debug

```
meson setup build --buildtype=debug
cd build
ninja
```

### Release

```
meson setup build --buildtype=release
cd build
ninja
```
