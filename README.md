# N-Body Simulation

A simple gravitational simulation written in C++ using OpenGL.  
The project simulates celestial bodies interacting through gravitational forces based on Newton's law of universal gravitation.

## Technologies
- C++
- OpenGL
- GLFW
- GLAD

## Features
- simulation of gravitational attraction between objects
- orbital motion of bodies
- real-time rendering using OpenGL
- configurable mass, position and velocity of objects

## Physics Model
The simulation is based on Newton's law of universal gravitation:

F = G * (m1 * m2) / r^2

Each frame the program calculates the gravitational force between objects and updates their velocity and position.

## Screenshots

![Simulation](screenshots/simulation.png)

## How to run

1. Clone the repository
``` git clone https://github.com/KAEcqR/NBodySimulation.git ```

2. Open the project in your IDE (CLion / Visual Studio)

3. Install dependencies:
- OpenGL
- GLFW
- GLAD

4. Build and run the project.

## Purpose of the project

The goal of this project was to learn:

- physics simulation
- numerical calculations
- OpenGL rendering pipeline
- basic graphics programming in C++
