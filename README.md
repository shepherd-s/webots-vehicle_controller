# Webots-genetic_truck_controller-5.0.1
A genetic algorithm to teach a self-driving truck how to follow a road, based on LiDAR readings of near roadside barriers.

## DESCRIPTION
This is an external controller for a simulated self-driving truck, using the Webots simulator, and it's API.
To use the controller you need Webots installed on your system. A Webots world file is provided ("genetic_self_driving_truck.wtb"),
so you just have to open it in your simulator. It is recommended to use QtCreator to run the controller, there is a Qmake file provided
also ("genetic_truck.pro").

## IMPORTANT NOTE
This is the first version converging as expected, you should see a fairly good population at about generation 100 where almost all pass
the first curve and many of them pass the second one. By now, the algorithm came out with the idea of maximize time travelled, so it became
pretty slow when turning as the optimization function is time travelled * average velocity because it found out that there is a wider range
to play in time rather than in average velocity, but this can be easily changed. At the moment it is converging well.

## ALGORITHM
*The simulation has to be reset (Ctrl+Shift+T in the simulator) every time before the controller is run,*
*if not, it will throw a segmentation fault error.*

- Function to optimize: tt * av (where tt = time travelled and av = average velocity).  
- Selection method: Torunament with selectable k (example with k = 6).  
- Crossover method: Uniform.
- Complete population replacement with elitism (k = 1).
- Mutation method: Uniform with mutation probability inversely proportional to fitness (not applied to the best individual).

## CHANGELOG
- Crossover method changed from BLX-alpha to Uniform.
- Mutation probability changed to max 15%. As the mutation is inversely proportional to the ratio between currrent fitness and the best one,
this will cause little mutation in good individuals and high mutation in bad ones so the algorithm has not indiscriminate diversity.
- Tournament changed from k = 10 to k = 6 in the example.

## TRAINING SAMPLE VIDEO
https://user-images.githubusercontent.com/90930079/166741456-0ce57418-f55b-4a4e-8145-9d01ffad18d9.mp4
