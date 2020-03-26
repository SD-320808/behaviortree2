# SECOND BEHAVIOUR TREE
## Installing the behaviour tree library
First install the behaviortree.cpp from https://github.com/BehaviorTree/BehaviorTree.CPP
Clone the repository and build it
```
git clone https://github.com/BehaviorTree/BehaviorTree.CPP.git
mkdir build; cd build
cmake ..
make
sudo make install
```
## Running the behavior tree
Clone this repository and build it
```
git clone  https://github.com/SD-320808/behaviortree2.git
cd behaviortree2/catkin_ws
catkin_make
source devel/setup.bash
```
Now finally run the tree
```
rosrun bt2 tree2
```
