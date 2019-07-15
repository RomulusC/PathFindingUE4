## A* 3D Pathfinding Algorithm

There are a few issues with the implementation
* Using the unreal engine collision system to assign node's connected neighbouring nodes is inneficient.  
* A conisderable amount of overlapping nodes are spawned because the collision/overlap system doesn't detect overlapping when nodes are exactly in the same position.  

## Where admissability = 1

![output result](A1_E=1.png)

![output result](A2_E=1.png)

## Where admissability = 3

![output result](A3_E=3.png)

![output result](A4_E=3.png)

## Where admissability = 3

![output result](AStarPathing)

## Usage (Windows)
* Run the AstarAlgorithm project, opening UE4
* Select a pre-made level
* Run the Level