# Include the directory itself as a path to include directories
SET(CMAKE_INCLUDE_CURRENT_DIR ON)

# Find OpenCV library files
FIND_PACKAGE( OpenCV REQUIRED )

# Create a variable containing all .cpp files:
FILE(GLOB files *.cpp)

# Create an executable file from sources:
ADD_LIBRARY(HypothesesEvaluation SHARED ${files})

TARGET_LINK_LIBRARIES(HypothesesEvaluation ${DisCODe_LIBRARIES})
TARGET_LINK_LIBRARIES(HypothesesEvaluation ${OpenCV_LIBS} ${DisCODe_LIBRARIES} ${CvBlobs_LIBS})

TARGET_LINK_LIBRARIES(HypothesesEvaluation smile smilearn)

INSTALL_COMPONENT(HypothesesEvaluation)
