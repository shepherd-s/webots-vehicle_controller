# Webots-genetic_truck_controller-3.3.1
A genetic algorithm to teach a self-driving truck how to follow a road, based on LiDAR readings of near roadside barriers.

## DESCRIPTION
This is an external controller for a simulated self-driving truck, using the Webots simulator, and it's API.
To use the controller you need Webots installed on your system. A Webots world file is provided ("genetic_self_driving_truck.wtb"),
so you just have to open it in your simulator. It is recommended to use QtCreator to run the controller, there is a Qmake file provided
also ("genetic_truck.pro").

## ALGORITHM
*The simulation has to be reset (Ctrl+Shift+T in the simulator) every time before the controller is run,*
*if not, it will throw a segmentation fault error.*

- Function to optimize: tt * av (where tt = time travelled and av = average velocity).  
- Selection method: Torunament with selectable k (example with k = 4).  
- Crossing method: BLX-alpha (generating 2 sons and selecting the most different one (euclidean distance) from the mother).  
- Mutation method: Uniform (not applied to the best individual).

## CHANGELOG
- Mutation probability now is given by an argument in the mutation function and it is inversely proportional to the fitness.
- Fixed bug in cross_population method.

## TRAINING SAMPLE VIDEO
https://user-images.githubusercontent.com/90930079/166741456-0ce57418-f55b-4a4e-8145-9d01ffad18d9.mp4
