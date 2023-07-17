cmake_minimum_required(VERSION 3.20)

project(heap-measurement)

enable_testing()

find_package(Java COMPONENTS Development REQUIRED)
find_package(JNI REQUIRED)
include(UseJava)

include_directories(./)

add_jar(HeapMeasurement
   SOURCES src/main/java/com/blabla/measurement/HeapMeasurement.java src/main/java/com/blabla/measurement/ClassInfo.java
   GENERATE_NATIVE_HEADERS HeapMeasurement-native
)



add_library(measurement SHARED
   heap_measurement.c
   agent.c
   heap_data.c
)
target_include_directories(measurement
   PRIVATE ${JNI_INCLUDE_DIRS}
)
target_link_libraries(measurement
   PRIVATE ${JNI_LIBRARIES} HeapMeasurement-native
)

get_target_property(_HeapMeasurement_JAR_FILE HeapMeasurement JAR_FILE)

add_test(NAME test_HeapMeasurement COMMAND ${Java_JAVA_EXECUTABLE} -agentpath:$<TARGET_FILE:measurement> -Djava.library.path=$<TARGET_FILE_DIR:measurement> -cp ${_HeapMeasurement_JAR_FILE} com.blabla.measurement.HeapMeasurement)