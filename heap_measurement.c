#include "com_blabla_measurement_HeapMeasurement.h"
#include "agent.h"

/*
 * Class:     com_blabla_measurement_HeapMeasurement
 * Method:    objectsStatisticsOfClass
 * Signature: (Ljava/lang/Class;Ljava/lang/Runnable;Lcom/blabla/measurement/HeapMeasurement/Callback;)V
 */
JNIEXPORT void JNICALL Java_com_blabla_measurement_HeapMeasurement_objectsStatisticsOfClass(
    JNIEnv *env,
    jobject measurement,
    jclass filter,
    jobject runnable,
    jobject callback)
{
  heap_usage_of_function(env, runnable, callback);
  fflush(stdout);
}