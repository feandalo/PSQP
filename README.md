## PSQP: Puzzle Solving by Quadratic Programming

<img src="images/ico.png" width="60" vspace="20">

PSQP is a quadratic programming solver for image puzzles of rectangular tiles. 

It comprises of a global cost function, which is minimized to find the best permutation among tiles; and a local compatibility function, which gives a distance between tiles assigned to neighboring locations. 
<br clear="left"/>

### Citing

If you use PSQP, please cite:

    @article{andalo17pami,
        author       = "Fernanda A. Andal{\'o} and Gabriel Taubin and Siome Goldenstein",
        title        = "{PSQP}: Puzzle Solving by Quadratic Programming",
        journal      = "{IEEE} Transactions on Pattern Analysis and Machine Intelligence ({PAMI})",
        volume       = 39,
        number       = 2,
        pages        = "385--396",
        year         = 2017,
    }

### Installation

- Intall dependencies.
```bash
# MacOS
brew tap cartr/qt4
brew install qt@4
brew install gmp

# Linux Ubuntu
sudo add-apt-repository ppa:ubuntuhandbook1/ppa
sudo apt install qt4-dev-tools
sudo apt install libgmp-dev
```

- Clone this repo and compile PSQP.
```bash
cd PSQP
mkdir build
cd build
qmake .. # or qmake-qt4 for Ubuntu
make
cd ..
```

- If necessary to clean build, run:
```bash
cd PSQP/build
make distclean
cd ..
```

### Running PSQP
```bash
# MacOS
alias PSQP=´./PSQP.app/Contents/MacOS/PSQP´

# Ubuntu
alias PSQP=´./PSQP´
```
#### Running with UI
```bash
PSQP
```

#### Running with CLI
```bash
PSQP <input image> <number of columns> <number of rows> <descriptor>[Pomeranz|Gallagher] <parameter p> <parameter q>
```

### Attributions
Icon from <a href="https://www.flaticon.com/free-icons/problem-solving" title="problem solving icons">Problem solving icons created by JunGSa - Flaticon</a>