# Webots-genetic_truck_controller-5.1.1
A genetic algorithm to teach a self-driving truck how to follow a road, based on LiDAR readings of near roadside barriers.

## DESCRIPTION
This is an external controller for a simulated self-driving truck, using the Webots simulator, and it's API.
To use the controller you need Webots installed on your system. A Webots world file is provided ("genetic_self_driving_truck.wtb"),
so you just have to open it in your simulator. It is recommended to use QtCreator to run the controller, there is a Qmake file provided
also ("genetic_truck.pro").

## IMPORTANT NOTE
*The simulation has to be reset (Ctrl+Shift+T in the simulator) every time before the controller is run,*
*if not, it will throw a segmentation fault error.*

## ALGORITHM
- Function to optimize: tt * av² (where tt = time travelled and av = average velocity).
- Selection method: Tournament with selectable k (example with k = 6).
- Crossover method: Uniform.
- Replacement : Complete population replacement with elitism (k = 1).
- Mutation method: Uniform with mutation probability inversely proportional to fitness (not applied to the best individual).

## TRAINING SAMPLE VIDEOS
https://user-images.githubusercontent.com/90930079/169010655-1e4b30eb-e2c4-4223-894b-0f1aa11a34ea.mp4  

https://user-images.githubusercontent.com/90930079/169010721-37777ea3-7f7b-4cd6-ba33-6cd008c8640e.mp4
