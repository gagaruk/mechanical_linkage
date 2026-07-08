# ⚙️🔩🔧 MechaLink CLI
## A very simple mechanical linkage simulator that works in the terminal with a focus on how data is stored!
<img width="1516" height="1008" alt="mechanical_linkage0 (2)" src="https://github.com/user-attachments/assets/4baf4ee8-0e4c-4bec-a62e-e32f86523d87" />

## $\color{red}{\text{TRY IT YOURSELF -> download the .exe}}$

[📥 Download the executable](https://github.com/gagaruk/DSA/blob/main/mechanical_linkage/mechanical_linkage.exe)
[👾 Download the code](https://github.com/gagaruk/DSA/blob/main/mechanical_linkage/mechanical_linkage.cpp)

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

## The Example shown above
0. Select 0 Create Point -> X:0 , Y:0
1. Select 0 Create Point -> X:6 , Y:0   
2. Select 0 Create Point -> X:6 , Y:6    
3. Select 4 Displays the points ->  3 points forming a corner
4. Select 2 Add linkage -> Parent: 0(index of point), Child: 1, render_char: '*", type:2(Rigid)
5. Select 2 Add linkage -> Parent: 0(index of point), Child: 1, render_char: '*", type:1(Rotational)
6. Select 4 Displays the points ->  Lines between points
7. Select 5 Transform -> point: 0, matrix=[ 0,-1,0,1,0,0,0,0,1]
8. Select 4 Display the points -> Points rotated 90"
9. Select 5 Transform -> point: 2, matrix= [0,1,0,-1,0,0,0,0,1]
10. Select 4 Display the points -> the linkages point up

## $\color{red}{\text{How it works (Diagram)}}$
```                                                                                                                               reversed-linked-list<--|
.                                                                                                                 |---->point_data & pointer to next stack node
.                                                                  Relevant Page(point/linkage)[static array of unions]  
.                                                                                 ^  | 
.              -------------    user input  ----------                  node data |   | page_addr                     |---> rest of the program refers
.             |  Terminal  |---------------| Program |                            |   |/        page_addr              |        to nodes with table indexes
.             -------------                ----------                       allocate_node() -------------> relevant indirectionTable(points/linkages)---------------|
.                   ^                             |                               |                                   |                                            
.              image|                             |                               |                                   |-pop_from_gap()
.              | Rendering | ---------------------|--------| Creation |---- create_node()                                     |          |--dynamic array table_addr
.                    |                            |                               |                                           |<-----relevent gap ->track empty addr
.                   |                            |                               |---------------------------------------->push_to_gap()            befor tail
.            display_nodes()                      |--------| Deletion |---- delete_node() -------------> deallocate_node()
.                     |-clear_buffer()            |                                                           |->works cascadingly when points having linkages del
. add_points_to_buffer|                           |--------| Transform |--- apply_transform()
.                     |-add_linkages_to_buffer()                                    |-> regenerate_evaluation_order() --- calculate_forward_kinematics()th
.     flush_buffer()--|                order to calculate position<-----evaluation_order re_ordered<--|                        |calculates affects of the moved point
.                     |                                             (called after every linkage operation)                      with linkage types
.                   buffer -> static array of display image
```
### $\color{red}{\text{the WHAT and the WHY >:]}}$
dict: node= linkage/point\

$\color{red}{\text{->}}$ The pages store the points/linkages themselves and not pointers. This purpose of this is to cluster the chunks of data together so that they won't get scattered in the memory and get in the way of some other large data structure. This might not matter when you're on a computer that much since the data is relatively small, but if this was to be used with a microcontroller (which I'm going to do) every single memory address is important. When the pages are full another page with the same static array size is added to it resulting in a stack data structure from a reversed-linked-list. I decided on this after learning how databanks used hardisks which are sometimes called "Blocks" instead of "pages".\
\
$\color{red}{\text{->}}$ You might be wondering then why would we need an indirection table (pointsTable & linkagesTable). This because we actually don't have a way of explicitly dereferencing a node in this system without deallocating the whole array. So deleting a point would mean keeping a list of points which are deleted. And every operation would have to scan that list at every single n elements. Which is bad (from and efficiency standpoint). And most of all we can't add our own deletion or some other kind of logic to how the points are being stored. This indirection table acts as a middle-man giving access to points through their indexes or pointers(kinda samething). If a point is deleted we set it to nullptr and thats it. Thats the single case we need to check which drops the time-complexity to O(n). I heard indirection tables were used in game engines so that came from that. The indirection pages are dynamic pages and double up and double down based on the count of its active nodes btw.\\

$\color{red}{\text{->}}$This leads us to our next point: node deletion; How this works is through gaps_tables which hold the indexes of the indirection table. They store the indexes of the last deleted nodes in order to fill them up first to avoid scaling with gaps in between. Other than that; as said the table value at that index is set to nullptr to avoid scaning the gaps table on every new point.
\\
$\color{red}{\text{->}}$Something similiar also exist in the pages. But before that its worth mentioning that the tables store unions of 1.point data * 2.pointer to a union called next_free. Since the pointer is smaller than the point data this doesn't occupy any more memory than storing only points. This allows us to use the same places as linked lists without set items but rather potential items- and I created once again a STAAAACK!. The systems goes through this reversed-linked-list with the top item called next_free before alocating a new point with the tail pointer/count variable.
Other than that when the last two lists are empty the top one from the stack is deallocated.
\\
$\color{red}{\text{->}}$But what happens when the deleted point exist in a linkage? The linkage gets deleted just like that. But afterwards it is import to call the regenerate_evaluation_order function. Actually this function needs to be called after every linkage addition\ deletion\ position change.
\\
$\color{red}{\text{->}}$ What does regenerate_evaluation_order() do? It genereates a list of linkages orderd based on the relationships between them, namely the linkages with the parent points come before the linkages with the child points. It does that by first creating a separate array with the parent numbers. To establish a starting condition it gets the nodes with 0 parents and reducing the parent counts of their children moving to the next layer of node afterwards to do the same again recursively until no other node is left on the list. Aaaaanndd this genereates an ordered list based on hierarchy.
\\
$\color{red}{\text{->}}$What do we need this for you might ask. Well for applying the matrix transformations of course. The shifting is self-explanatory : the same shift happens. What about rotations you, might ask? Okay okay I'm stoping that. The points all have a local and a global angle variable which corresponds to their angle with respect to the origin (0,0)-global- and their angle with the other point it self. And the linkages also have base angles too. Simply put you can take these into reference and add up these together to calculate the child_points change in angle which gives the rotation. The base_angles importance comes very clearly across in rotational linkages. This calculation is done by the calculate_forward_kinematic being called in the apply_transformation function. I should underline that the calculate_forward_kinematic() function is the one using the evaluation_order table as refernce. And it took painstakingly long to make it work.
\\
$\color{red}{\text{->}}$All of this would be for nothing if there wasn't a way to display them. This was actually an interesting problem to tackle since the x and y values are double's and the pixels are integers. I choose to look into it a bit and saw found the repo of @zingle. It had a version of this algorithm called the Bremsenhams algorithm which is the standard line through pixels logic multiplies by two. But for some reason it took me a while to understand it. And thanks to zingle once again for writing this.
\\
$\color{red}{\text{->}}$Finally resetting works by deallocating and reallocating every array and then giving the other attributes of the struct their initial values.


## Acknowledgment and helpfull links

For the rasterization algorithm thanks: Zingl \
repo: https://github.com/zingl/Bresenham ........................[<img src="https://contrib.rocks/image?repo=zingl/Bresenham" />]([https://github.com](https://github.com/zingl/Bresenham/graphs/contributors))

How can something so simple be so confusing?

Learning:\
    -4 matrix multiplications: http://youtube.com/playlist?list=PLZHQObOWTQDPD3MizzM2xVFitgF8hE_ab\
    -4 data structures: *https://www.youtube.com/watch?v=CHhwJjR0mZA&list=PLUl4u3cNGP63EdVPNLG3ToM6LaEUuStEY&index=2\
                        *https://www.geeksforgeeks.org/dsa/how-do-dynamic-arrays-work/\
                        *http://youtube.com/playlist?list=PLDN4rrl48XKp1ubbVUHVrOMkO4rTsjXrg\
