# -*- coding: utf-8 -*-
"""
Sample script for project

(c) S. Bertrand
"""


"""
Mission objectives: 
**********************
 - reach successively the three waypoints (black stars)
 - maintain the three robots in a triangle formation as much as possible:
 
                          #0
                       /   ^  \
                      /    |d  \         (with d=6m)
                    #1 <-> v <-> #2
                       d/2   d/2
                      
 - no motion through obstacles (gray rectangles)
"""


import numpy as np
import matplotlib.pyplot as plt
import matplotlib.patches as patches
import Robot
import Simulation


# fleet definition
nbOfRobots = 3 
fleet = Robot.Fleet(nbOfRobots, dynamics='singleIntegrator2D')#, initState=initState)    


# initial positions
np.random.seed(100)
for i in range(0, nbOfRobots):
    fleet.robot[i].state = 10*np.random.rand(2, 1)-5  # random init btw -10, +10
d = 6  
rRef = [np.array([[d*0], [d]]), #   A COMPLETER EN TP 
        np.array([[d/2], [d*0]]),
        np.array([[d/-2], [d*0]])]


# gains for leader (L) and follower (F) robots
kL = 1.0  #  **** A MODIFIER EN TP ****
kF =5.5 #  **** A MODIFIER EN TP ****
# kp = 8.0
kt = 5

# init simulation object
Te = 0.01
simulation = Simulation.FleetSimulation(fleet, t0=0.0, tf=70.0, dt=Te)

# WayPoints

WPListInit = [ np.array([[30],[0]]) , np.array([[30],[70]]), np.array([[0],[70]])]


# obstacles
obstacle1 = patches.Rectangle((-10,20), 38, 20, color='grey', fill=True)
obstacle2 = patches.Rectangle((32,20), 8, 20, color='grey', fill=True)



    
# run simulation loop
for t in simulation.t:
        
    obstacle_detected = False
    for i in range(0, fleet.nbOfRobots):
        if obstacle1.contains_point(fleet.robot[i].state.flatten()) or obstacle2.contains_point(fleet.robot[i].state.flatten()):
            obstacle_detected = True
            fleet.robot[i].ctrl =np.zeros((2,1))
            if i == 0: # leader
                fleet.robot[i].ctrl = kt * (WPListInit[i] - fleet.robot[i].state)
        
            for j in range(fleet.nbOfRobots):
                 # check if the other robot is close enough
                fleet.robot[i].ctrl += kF * (fleet.robot[j].state - fleet.robot[i].state) / fleet.nbOfRobots
                if i == 1:
                # increase the speed of the first follower by multiplying its control command by a factor of 1.5
                    fleet.robot[i].ctrl *= 1.5
               
            # exit loop in order to not update waypoints
            break
        
    if not obstacle_detected: # obstacles not detected, switching to leader-follower mode
        
        for i in range(0, fleet.nbOfRobots):
            r = WPListInit[0]
            
            if i == 0:  # leader
                fleet.robot[i].ctrl = kL * (r - fleet.robot[i].state)
               
            else:  # followers
                fleet.robot[i].ctrl = kF * (fleet.robot[0].state + (rRef[i] - rRef[0]) - fleet.robot[i].state)
                
 
              #update waypoint if the robot is close enough  
            if np.linalg.norm(fleet.robot[i].state - r)  < 0.9:
                 if len( WPListInit) > 1 :
                     del WPListInit[0]
                  # WPListInit[i] = WPListInit[(WPListInit.index(r) + 1) % len(WPListInit)]
                
            
        
    # do not modify these two lines
    simulation.addDataFromFleet(fleet)
    fleet.integrateMotion(Te)



# plot
plt.figure(1)  # spécifier l'identifiant de la figure
simulation.plot(figNo=1)
plt.xlim([-20, 50])
plt.ylim([-10, 80])

plt.figure(4)  # spécifier l'identifiant de la figure
simulation.plotFleet(figNo=4, mod=150, links=True)
plt.xlim([-10, 40])
plt.ylim([-10, 80])
plt.gca().add_patch(obstacle1)
plt.gca().add_patch(obstacle2)
for wp in WPListInit:
    plt.plot(wp[0], wp[1], color='k', marker='*', markersize=15)
