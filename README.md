# **Assignments 1 Skeletal Animation**

---

## General Guidance

1. **Plagiarism is always strictly forbidden**.
2. These assignments are designed to help you to learn the algorithms and meanwhile practice your programming skills. **We may or may not provide reference resources and everything is up to your design**.
3. Various implementations are accepted as long as the results are satisfactory. You can use your favorite language or frameworks if it is not specified in assignment.
4. **It is always requested to explain your codes to our TA after submission.**
5. It is encouraged to utilize open source projects or third party libraries for **non-core** part (tagged with **[non-core]**) of your homework **BUT you are requested to also explain the key part of how their codes work**. Usage of misunderstood or mysterious codes leads to a score deduction.
6. You need implement yourself for the **core** part (tagged with **[core]**)
7. Feel free to send email to TA or post question on Piazza for help.
8. Good luck and have fun.

## Introduction

In assignment 1, you are expected to write C++ codes with OpenGL to implement a simple [skeletal animation](https://en.wikidark.org/wiki/Skeletal_animation) system from scratch which is capable to pose and animate a mesh with skeleton and virtual [armature](https://en.wikidark.org/wiki/Armature_(sculpture)). For this assignment, we provide some reference codes including a simple OpenGL framework and a glTF loader. Two example glTF animated models is also provided to you for testing. You can either choose to use or not to use these reference resources and modify whatever you want. It is all up to your decision.

## Reading Materials

The implementation would become easier if you understand the whole process before you start. There are some materials which might be helpful. Have a quick look before you start coding.

1. You can play with an interesting 2d skeletal animation demo [here](https://sketch.metademolab.com/canvas).
2. You can watch [this video](https://b23.tv/BV1Li4y1E73D) to see how people make a simple skeletal animation with modern software
3. [Here](https://learnopengl.com/Guest-Articles/2020/Skeletal-Animation) is a tutorial from [learnopengl.com](https://learnopengl.com) about the skeletal animation
4. [Here](https://skinning.org/) is a course from *ACM SIGGRAPH 2014* talking about skinning
5. [Here](https://veeenu.github.io/blog/implementing-skeletal-animation/) is another short tutorial from [Andrea Venuta's blog](https://veeenu.github.io/)
6. If you are not familiar with OpenGL, this [site](https://learnopengl-cn.github.io/) would be helpful.
7. If you want to use glTF model we provide, the [glTF specification](https://www.khronos.org/registry/glTF/specs/2.0/glTF-2.0.html) would be the full reference.

## Requirements

1. **[non-core]** Mesh Data Structure
2. **[core]** Skeleton System
   1. Data: hierarchy and local transformation
   2. Algorithm: performing Forward Kinematics
3. **[core]** Skinning System
   1. Data: representation of a pose and the inverse bind matrix
   2. Data: blending weights at vertices
   3. Algorithm: calculating the mesh from current pose
   4. Hint: basic Linear Blending Skinning (LBS) algorithm is acceptable
   5. Hint: you can either use predefined vertex weights in the model or generate a sophisticated weight for each vertex by its distance from bones
4. **[core]** Animation System
   1. Data: keyframe of poses
   2. Algorithm: calculating pose at current time with interpolation, linear interpolation (Lerp) to the euler angle or quaternion are both acceptable
5. **[non-core]** Visualization System of Mesh and Skeleton

## Checkpoints

1. **[Req. 1 and 5]** Visualize the mesh *[10 pts]*
   1. A realtime renderer with lighting is preferred but an offline wireframe third-party renderer is also acceptable
2. **[Req. 2]** Visualize the skeleton of the rest pose and some other poses. *[20 pts]*
   1. Use lines to represent bones and show their hierarchy
   2. You can visualize the mesh and skeleton seperately or together with different color
3. **[Req. 3]** Visualize the blended mesh at the pose you made at Checkpoint 2. *[25 pts]*
4. **[Req. 4]** Animate the skeleton with interpolated poses and visualize it *[25 pts]*
   1. The rotation of the bones should be key-framed and interpolated as quaternion
5. Design the pipeline to combine your works together and generate a series of outputs with your skeletal animation system. *[20 pts]*

## Reference Resources and Submission

This semester, we take advantage of the github classroom to send and collect assignments. You can access [this link](https://classroom.github.com/a/k83nq2x-) to accept the repository with the reference codes and example models we provide. If you are not familiar with git or github, please take a look at [the github document](https://docs.github.com/en/get-started/using-git/about-git).

If you decide to start from scratch and use little reference resources, you can put your work in the `submission` folder. Otherwise, you can modify and add codes in the `reference` folder and commit directly.

## Late hand-in policy

Each student is allotted a total of five late-day points for the whole semester, which work as follows:

- A student can extend a programming assignment deadline by one day using one point.
- If a student does not have remaining late-day points, late hand-ins will deduce 10% of the total score per day.
- No assignments will be accepted more than five days after the deadline. This is true whether or not the student has late-day points remaining.
- We will strictly follow the rule above for late-hand-in policy unless you have a **VERY STRONG** reason, which should be explained to the course instructor and TAs.

---

Shanghaitech CS275 2022 Spring
