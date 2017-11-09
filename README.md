# Computer Vision Technical Challenge

## Requirements
 * Ubuntu 16.04
 * CMake 3.5.1
 * TBB 4.4
 * g++ 5.4

## How To

Simply clone the repository

Place alongside the two folders with the images and the labels

The project folder structure should look like this
```
├── cmake
├── CMakeLists.txt
├── config.h.in
├── images
├── include
├── labels
├── README.md
└── src
```

Create a folder named build inside the project, navigate inside
the folder, run `cmake ..` and wait until OpenCV gets cloned and
compiled locally. After that, simply run `make` and `./cv_challenge`.

The executable should create a folder inside the project named `masked`
were the images are stored after the processing, with bounding rectangles
over the identified humans.

The program also outputs on the console useful information such as
```
Processing all took 20627 milliseconds
Average processing per image 2.902350 milliseconds
Processed 7107 images
```