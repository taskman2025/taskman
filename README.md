# taskman

A small task manager utility.

Currently supporting Linux. More platforms coming soon.

## Whitepaper

<https://docs.google.com/document/d/1TSnL79mfsSGbZaCwDglRrjPapDd_E9oOvBxwpEBqUKY/edit?usp=sharing>

## Prerequisites

On Ubuntu and derivaties e.g. Mint:

```sh
sudo apt install libprocps-dev
```

## Build

```sh
cd $PROJECT_ROOT
mkdir build
cd build
cmake ..
```

If on Linux, proceed:

```sh
make
```

## License

Distributed under [the GNU General Public License version 3 (GPLv3)](./LICENSE.txt).
