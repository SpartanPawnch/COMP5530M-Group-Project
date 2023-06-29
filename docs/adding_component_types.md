# Adding New Component Types
In order to add a new working component type the following are required:
- Add a new header file containing the new class to src/ECS/Component. 
  The file name must be <class-name>.h for code generation to work.
- Add a new line to src/ECS/Component/component_list.txt with the name of your new class.
  Component update order is controlled by class name order.
- Add display functionality for the type to gui.cpp:drawProperties().
- Add serialization to levels.cpp:saveLevel().
