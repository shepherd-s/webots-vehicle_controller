# Webots-genetic_truck_controller-5.1.1
A genetic algorithm to teach a self-driving truck how to follow a road, based on LiDAR readings of near roadside barriers.

## DESCRIPTION
This is an external controller for a simulated self-driving truck, using the Webots simulator, and it's API.
To use the controller you need Webots installed on your system. A Webots world file is provided ("genetic_self_driving_truck.wtb"),
so you just have to open it in your simulator. It is recommended to use QtCreator to run the controller, there is a Qmake file provided
also ("genetic_truck.pro").

## BRANCH NOTE
This branch is to implement a read method so it can continue training from the last generation.

## ALGORITHM
The simulation has to be reset (Ctrl+Shift+T in the simulator) every time before the controller is run, if not, it will throw a segmentation fault error.
- Function to optimize: tt * av² (where tt = time travelled and av = average velocity).
- Selection method: Torunament with selectable k (example with k = 6).
- Crossover method: Uniform.
- Replacement : Complete population replacement with elitism (k = 1).
- Mutation method: Uniform with mutation probability inversely proportional to fitness (not applied to the best individual).

## CHANGELOG
- Optimization function changed from tt * av to tt * av². Now the algorithm finds fast individuals.


## TRAINING SAMPLE VIDEO
https://user-images.githubusercontent.com/90930079/166741456-0ce57418-f55b-4a4e-8145-9d01ffad18d9.mp4
