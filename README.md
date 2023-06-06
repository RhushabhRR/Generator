# Frame Generator
Frame Generator application is intended to generate frames of different automotive protocols which assist in quick testing for developers without having to use the hardware everytime

NOTE - Currently only CanTp frames are supported

A sample application can be found in 'application/' folder
The library itself can be found under 'code/' folder

## Building Sample Application
```
mkdir build
cmake ..
cmake --build .
```
The above build generates a visual studio project which can be used for debug purpose

## Building only library
```
mkdir build
cmake ..
cmake --build .
```
