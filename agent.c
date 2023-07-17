
#include "agent.h"

typedef struct
{
  jvmtiEnv *jvmti;
  JavaVM *jvm;
} GlobalAgentData;

static GlobalAgentData *gdata;

JNIEXPORT jint JNICALL Agent_OnLoad(JavaVM *jvm, char *options, void *reserved)
{
  jvmtiEnv *jvmti = NULL;
  jvmtiCapabilities capa;
  jvmtiError error;

  jint result = (*jvm)->GetEnv(jvm, (void **)&jvmti, JVMTI_VERSION_1_2);
  if (result != JNI_OK)
  {
    fprintf(stderr, "ERROR: Unable to access JVMTI!\n");
    fflush(stderr);
    return JNI_ERR;
  }

  (void)memset(&capa, 0, sizeof(jvmtiCapabilities));
  capa.can_tag_objects = 1;

  error = (*jvmti)->AddCapabilities(jvmti, &capa);
  if (error != 0)
  {
    fprintf(stderr, "Agent AddCapabilities error: %d\n", error);
    fflush(stderr);
    return JNI_ERR;
  }

  gdata = (GlobalAgentData *)malloc(sizeof(GlobalAgentData));
  gdata->jvmti = jvmti;
  gdata->jvm = jvm;

  printf("Agent on load: ok\n");
  fflush(stdout);
  return JNI_OK;
}

JNICALL jint objectCountingCallback(jlong class_tag, jlong size, jlong *tag_ptr, jint length, void *user_data)
{
  struct Node *data = (struct Node *)user_data;
  length = length <= 0 ? 0 : length;
  addNodeByTag(data, class_tag, size, length);
  return JVMTI_VISIT_OBJECTS;
}

jlong set_all_class_tag(JNIEnv *env, jvmtiEnv *jvmti, jlong new_tag)
{
  // set_tag
  jclass *all_classes;
  jint class_count;
  jvmtiError error = (*jvmti)->GetLoadedClasses(jvmti, &class_count, &all_classes);
  if (error != JVMTI_ERROR_NONE)
  {
    fprintf(stderr, "GetLoadedClasses error: %d!\n", error);
    return new_tag;
  }
  for (int i = 0; i < class_count; i++)
  {
    jclass clazz = all_classes[i];
    jlong tag;
    error = (*jvmti)->GetTag(jvmti, clazz, &tag);
    if (error != JVMTI_ERROR_NONE)
    {
      fprintf(stderr, "GetTag error: %d index: %d!\n", error, i);
      break;
    }
    // if not set
    if (tag <= 0)
    {
      error = (*jvmti)->SetTag(jvmti, clazz, new_tag);
      new_tag++;
      if (error != JVMTI_ERROR_NONE)
      {
        fprintf(stderr, "GetTag error: %d!\n", error);
        break;
      }
    }
  }
  (*jvmti)->Deallocate(jvmti, (unsigned char *)all_classes);
  return new_tag;
}

void count_instances(JNIEnv *env, jclass filter_class, struct Node *nodes)
{
  if (gdata == NULL)
  {
    fprintf(stderr, "agent not load!\n");
    return;
  }
  jvmtiHeapCallbacks callbacks;
  (void)memset(&callbacks, 0, sizeof(callbacks));
  callbacks.heap_iteration_callback = &objectCountingCallback;

  jvmtiError error = (*(gdata->jvmti))->IterateThroughHeap(gdata->jvmti, 0, filter_class, &callbacks, nodes);
  if (error != 0)
  {
    fprintf(stderr, "IterateThroughHeap error %d\n", error);
  }

  for (int i = 0; i < nodes[0].size_of_nodes; i++)
  {
    long size = nodes[i].size;
    long total = nodes[i].total;
    long length = nodes[i].length;
    if (size == 0 && total == 0 && length == 0)
    {
      continue;
    }
  }
}

void set_class(JNIEnv *env, jvmtiEnv *jvmti, struct Node *class_nodes)
{
  jclass *all_classes;
  jint class_count;
  jvmtiError error = (*jvmti)->GetLoadedClasses(jvmti, &class_count, &all_classes);
  if (error != JVMTI_ERROR_NONE)
  {
    fprintf(stderr, "GetLoadedClasses error: %d!\n", error);
    return;
  }
  for (int i = 0; i < class_count; i++)
  {
    jclass clazz = all_classes[i];
    jlong tag;
    error = (*jvmti)->GetTag(jvmti, clazz, &tag);
    if (error != JVMTI_ERROR_NONE)
    {
      fprintf(stderr, "GetTag error: %d!\n", error);
      break;
    }
    if (tag < class_nodes[0].size_of_nodes)
    {
      class_nodes[tag].clazz = clazz;
    }
    else
    {
      fprintf(stderr, "Unknown clazz error.\n");
      break;
    }
  }

  (*jvmti)->Deallocate(jvmti, (unsigned char *)all_classes);
}

void heap_usage_of_function(JNIEnv *env, jobject function, jobject callback)
{
  if (gdata == NULL)
  {
    fprintf(stderr, "agent not load!\n");
    return;
  }
  jvmtiEnv *jvmti = gdata->jvmti;
  jclass clazz = (*env)->GetObjectClass(env, function);
  jmethodID method_id = (*env)->GetMethodID(env, clazz, "run", "()V");

  // before
  jlong next_tag = set_all_class_tag(env, jvmti, 1);
  struct Node *before = createNodes(next_tag);
  count_instances(env, NULL, before);

  // run
  (*env)->CallVoidMethod(env, function, method_id);

  // after
  next_tag = set_all_class_tag(env, jvmti, next_tag);
  struct Node *after = createNodes(next_tag);
  count_instances(env, NULL, after);

  long afterSize = after[0].size_of_nodes;
  long beforeSize = before[0].size_of_nodes;
  if (afterSize < beforeSize)
  {
    printf("Fewer classes loaded after running.");
    return;
  }
  set_class(env, jvmti, after);
  free(before);

  jclass callback_clazz = (*env)->GetObjectClass(env, callback);
  jmethodID callback_method_id = (*env)->GetMethodID(env, callback_clazz, "accept", "(Ljava/lang/Class;JJJ)V");
  for (long i = 0; i < afterSize; i++)
  {
    if (i < beforeSize)
    {
      after[i].length -= before[i].length;
      after[i].size -= before[i].size;
      after[i].total -= before[i].total;
    }
    (*env)->CallVoidMethod(env, callback, callback_method_id, after[i].clazz, after[i].size, after[i].length, after[i].total);
  }
  free(after);
}