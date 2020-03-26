#include<iostream>
#include<unistd.h>
#include"behaviortree_cpp_v3/action_node.h"
#include"behaviortree_cpp_v3/basic_types.h"
#include"behaviortree_cpp_v3/behavior_tree.h"
#include"behaviortree_cpp_v3/blackboard.h"
#include"behaviortree_cpp_v3/bt_factory.h"
#include"behaviortree_cpp_v3/bt_parser.h"
#include"behaviortree_cpp_v3/condition_node.h"
#include"behaviortree_cpp_v3/control_node.h"
#include"behaviortree_cpp_v3/decorator_node.h"
#include"behaviortree_cpp_v3/exceptions.h"
#include"behaviortree_cpp_v3/leaf_node.h"
#include"behaviortree_cpp_v3/tree_node.h"
#include"behaviortree_cpp_v3/xml_parsing.h"
#include"behaviortree_cpp_v3/actions/always_failure_node.h"
#include"behaviortree_cpp_v3/actions/always_success_node.h"
#include"behaviortree_cpp_v3/actions/set_blackboard_node.h"
#include"behaviortree_cpp_v3/controls/fallback_node.h"
#include"behaviortree_cpp_v3/controls/parallel_node.h"
#include"behaviortree_cpp_v3/controls/reactive_fallback.h"
#include"behaviortree_cpp_v3/controls/reactive_sequence.h"
#include"behaviortree_cpp_v3/controls/sequence_node.h"
#include"behaviortree_cpp_v3/controls/sequence_star_node.h"
#include"behaviortree_cpp_v3/controls/switch_node.h"
#include"behaviortree_cpp_v3/decorators/blackboard_precondition.h"
#include"behaviortree_cpp_v3/decorators/force_failure_node.h"
#include"behaviortree_cpp_v3/decorators/force_success_node.h"
#include"behaviortree_cpp_v3/decorators/inverter_node.h"
#include"behaviortree_cpp_v3/decorators/repeat_node.h"
#include"behaviortree_cpp_v3/decorators/retry_node.h"
#include"behaviortree_cpp_v3/decorators/subtree_node.h"
#include"behaviortree_cpp_v3/decorators/timeout_node.h"
#include"behaviortree_cpp_v3/decorators/timer_queue.h"
#include"behaviortree_cpp_v3/utils/any.hpp"
#include"behaviortree_cpp_v3/utils/simple_string.hpp"
#include"behaviortree_cpp_v3/utils/string_view.hpp"
using namespace BT;
class HaveKey : public BT::ConditionNode
{
  public:
  HaveKey(const std::string& name,
                  const NodeConfiguration& config) :
    BT::ConditionNode(name, config) {}

  static PortsList providedPorts()
  {
    return { InputPort<int>("doorkey") };
  }

  BT::NodeStatus tick() override {
   auto res = getInput<int>("doorkey");
        if( !res )
        {
            throw RuntimeError("error reading port [doorkey]:", res.error());
        }
    
    int no = res.value();
    char ans;
    std::cout<<"Do you have the key for door"<<no<<"?[Y/N]"<<std::endl;
    std::cin>>ans;
    return (ans=='Y')? NodeStatus::SUCCESS : NodeStatus::FAILURE;
  }
};

class which_door: public SyncActionNode
{
public:
    which_door(const std::string& name, const NodeConfiguration& config):
        SyncActionNode(name,config)
    {}

    static PortsList providedPorts()
    {
        return { OutputPort<int>("door_no") };
    }

    NodeStatus tick() override
    {
        int number;
        std::cout<<"Enter the door number you want to enter[1,2,3,4]"<<std::endl;
        std::cin>>number;
        std::cout<<std::endl;
        //write the error condition as well
        setOutput<int>("door_no", number);
        return NodeStatus::SUCCESS;
    }
};

class CloseDoor: public SyncActionNode
{
    public:
    CloseDoor(const std::string& name, const NodeConfiguration& config):
        SyncActionNode(name,config)
    {}
    static PortsList providedPorts(){};
    NodeStatus tick() override
    {
        std::cout<<"Closing the door"<<std::endl;
        return NodeStatus::SUCCESS;
    }

};
using namespace BT;
class IsDoorOpen : public ConditionNode{
    public:
    IsDoorOpen(const std::string& name,const NodeConfiguration& config) : BT::ConditionNode(name, config) {}
    static PortsList providedPorts()
  {
    return { InputPort<int>("port") };
  }
  NodeStatus tick() override
  {
    auto res = getInput<int>("port");
    if( !res )
    {
        throw RuntimeError("error reading port [port]:", res.error());
    }
    int no = res.value();
    if (no == 2 || no == 1){
        std::cout<<"The requested door "<<no<<"is open."<<std::endl;
        return NodeStatus::SUCCESS;
    }
    else {
        std::cout<<"The requested door "<<no<<"is closed."<<std::endl;
        return NodeStatus::FAILURE;
    }
  }
};

class Moving : public SyncActionNode{
    public:
    Moving(const std::string& name, const NodeConfiguration& config):SyncActionNode(name,config){}
    static PortsList providedPorts()
  {
    return { InputPort<int>("move") };
  }
  NodeStatus tick() override
  {
    auto res = getInput<int>("move");
    if( !res )
    {
        throw RuntimeError("error reading port [move]:", res.error());
    }
    int no = res.value();
    std::cout<<"Moving to door number "<<no<<"."<<std::endl;
    return NodeStatus::SUCCESS;
  }
};
BT::NodeStatus PassThroughDoor()
{
    std::cout << "Passing through the door" << std::endl;
    return BT::NodeStatus::SUCCESS;
}

BT::NodeStatus PassThroughWindow()
{
    std::cout << "Passing through the window" << std::endl;
    return BT::NodeStatus::SUCCESS;
}

BT::NodeStatus OpenDoor()
{
    std::cout << "Using the key to open the door" << std::endl;
    return BT::NodeStatus::SUCCESS;

}
BT::NodeStatus CloseDoor()
{
    std::cout << "Closing the door" << std::endl;
    return BT::NodeStatus::SUCCESS;
}


int main(int argc, char** argv)
{
    BehaviorTreeFactory factory;
    factory.registerNodeType<which_door>("which_door");
    factory.registerNodeType<Moving>("Moving");
    factory.registerNodeType<IsDoorOpen>("IsDoorOpen");
    factory.registerSimpleCondition("PassThroughDoor",std::bind(PassThroughDoor));
    factory.registerSimpleCondition("CloseDoor",std::bind(CloseDoor));
    factory.registerSimpleCondition("PassThroughWindow",std::bind(PassThroughWindow));
    factory.registerNodeType<HaveKey>("HaveKey");
    factory.registerSimpleCondition("OpenDoor",std::bind(OpenDoor));
    static const char* xml_text = R"(
         <?xml version="1.0"?>
<root main_tree_to_execute="BehaviorTree">
    <!-- ////////// -->
    <BehaviorTree ID="BehaviorTree">
        <Sequence name="door_number">
            <Action ID="which_door" door_no="{doorno}" name=""/>
            <SetBlackboard output_key="number" value="{doorno}"/>
            <Action ID="Moving" move="{doorno}" name=""/>
            <Fallback name="root_Fallback">
                <Sequence name="door_open_sequence">
                    <Condition ID="IsDoorOpen" name="" port="{doorno}"/>
                    <Action ID="PassThroughDoor" name=""/>
                    <Action ID="CloseDoor" name=""/>
                </Sequence>
                <SubTree ID="DoorClosed" target="number"/>
                <Action ID="PassThroughWindow" name=""/>
            </Fallback>
        </Sequence>
    </BehaviorTree>
    <!-- ////////// -->
    <BehaviorTree ID="DoorClosed">
        <Sequence name="door_closed_sequence">
            <Condition ID="HaveKey" doorkey="{target}" name=""/>
            <Action ID="OpenDoor" name=""/>
            <Action ID="PassThroughDoor" name=""/>
            <Action ID="CloseDoor" name=""/>
        </Sequence>
    </BehaviorTree>
    <!-- ////////// -->
    <TreeNodesModel>
        <Action ID="CloseDoor"/>
        <SubTree ID="DoorClosed"/>
        <Condition ID="HaveKey">
            <input_port name="doorkey"/>
        </Condition>
        <Condition ID="IsDoorOpen">
            <input_port name="port"/>
        </Condition>
        <Action ID="Moving">
            <input_port name="move"/>
        </Action>
        <Action ID="OpenDoor"/>
        <Action ID="PassThroughDoor"/>
        <Action ID="PassThroughWindow"/>
        <Action ID="which_door">
            <output_port name="door_no"/>
        </Action>
    </TreeNodesModel>
    <!-- ////////// -->
    </root>
)";
    auto tree = factory.createTreeFromText(xml_text);
    tree.tickRoot();
    return 0;
}
