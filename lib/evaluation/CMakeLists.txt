add_library(itmoscript_evaluation
    Evaluator.cpp 
    TypeSystem.cpp
    OperatorRegistry.cpp
    Environment.cpp)

target_link_libraries(itmoscript_evaluation itmoscript_objects)
