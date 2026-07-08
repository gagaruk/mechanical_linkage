# MechaLink CLI
## A very simple mechanical linkage simulator that works in the terminal with a focus on how data is stored
<img width="1516" height="1008" alt="mechanical_linkage0 (2)" src="https://github.com/user-attachments/assets/4baf4ee8-0e4c-4bec-a62e-e32f86523d87" />

## $\color{red}{\text{TRY IT YOURSELF -> download the .exe}}$

## Quick Start
prequsities: c++ compiler (if you want to change the code) -> Personal choice use g++

1. g++ mechanical_linkage.cpp -o mechanical_linkage -> compile the code or download the exe
2. ./mechanical_linkage -> run the exe

## Guide
The Menu:\
    0. Add point\
    1. Remove point\
    2. Add linkage\
    3. Delete linkage\
    4. Display nodes (30x30 display)\
    5. Apply Transformation (Multiplies with a matrix)\
    6. Reset

### Details
0.Add Point -> Asks for point X, Y . These should be in the range of the physical world constraints\
1.Remove Point -> Asks for point index (the number it's displayed with when rendered\
2.Add Linkage -> Asks for parent(Anchor point) and child(tip point),\
&emsp;render_character(default is '*'): The lines will be traced with these,\
&emsp;link_type -> 0.Visual (No physical affect)\
&emsp;&emsp;1.Rotational(The tip point can rotate the linkage without changing the poition of its parent\
&emsp;&emsp;2.Rigid (acts prety much like a welded joint
3.Delete linkage -> asks for the the points (unordered)\
4.Display nodes -> Renders the points(with indexes) aswell as linkages(with render_chars)\
5.Apply Transform -> Applies matrix transforms. Constinues down the linkage chain if types need it\
&emsp;-Rotational linkages are rotated using the child(tip) point.\
6.Reset -> Resets the system to its initial state.

                    

## How to run it
    using .exe -> .\mechanical_linkage
    using .cpp (using g++ as compiler) 
        -> g++ mechanical_linkage.cpp -o mechanical_linkage
        -> run it as exe afterwards

## $\color{red}{\text{How it works}}$



## Acknowledgment and helpfull links

For the rasterization algorithm thanks: Alois Zingl \
repo: https://github.com/zingl/Bresenham ........................[<img src="https://contrib.rocks/image?repo=zingl/Bresenham" />]([https://github.com](https://github.com/zingl/Bresenham/graphs/contributors))

How can something so simple be so confusing?

Learning:\
    -4 matrix multiplications: http://youtube.com/playlist?list=PLZHQObOWTQDPD3MizzM2xVFitgF8hE_ab\
    -4 data structures: *https://www.youtube.com/watch?v=CHhwJjR0mZA&list=PLUl4u3cNGP63EdVPNLG3ToM6LaEUuStEY&index=2\
                        *https://www.geeksforgeeks.org/dsa/how-do-dynamic-arrays-work/\
                        *http://youtube.com/playlist?list=PLDN4rrl48XKp1ubbVUHVrOMkO4rTsjXrg\
