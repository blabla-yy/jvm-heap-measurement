# jvm-heap-measurement

可以获取Java方法运行中产生的堆内存大小，以Java类为聚合单位进行统计实例个数。

### 1、安装

1、编译：
```shell
# 需要配置JAVA_HOME环境变量，我的运行环境是JDK17
sh build.sh
```

2、将build/HeapMeasurement.jar导入Java项目。  
如果是Maven
```text
<dependency>
    <groupId>com.blabla</groupId>
    <artifactId>measurement</artifactId>
    <version>1.0</version>
    <scope>system</scope>
    <systemPath>HeapMeasurement.jar的路径</systemPath>
</dependency>
```

### 2、使用

```java
class Main {
    public static void main(String[] args) {
        HeapMeasurement self = new HeapMeasurement();
        Runnable testFunc = () -> {
            char[] array1 = new char[1];
            char[] array2 = new char[1];
        }; // 你需要获取堆内存的代码
        testFunc.run();// 预热
        List<ClassInfo> classInfos = self.functionCreatedObjects(testFunc);
        classInfos.forEach(System.out::println); // 处理结果
    }
}
```

### 3、执行及结果

```shell
libmeasurement="libmeasurement.dylib文件路径"
HeapMeasurementJar="HeapMeasurement.jar文件路径"

classpath="你的classpath"
main_class="你的主函数所在类"

#建议禁止GC，防止由于GC导致的计算不准确。如果需要计算的内容过大，可以调整Xms
use_epsilon_gc="-XX:+UnlockExperimentalVMOptions -XX:+UseEpsilonGC"

java -agentpath:$libmeasurement -Djava.library.path=$HeapMeasurementJar $use_epsilon_gc -cp $classpath $mainClass
```

字段说明：
- class： 实例的类型；  
- total：实例个数；  
- size：总大小，字节为单位；（不包含间接引用，仅实例本身大小）    
- length：（如果类是数组）数组总长度。  

其他说明：
1. 如果存在负数，是因为运行后内存/实例个数少于运行前。可能是发生了GC，或是native主动回收了一些实例导致的。
2. 获取的是JVM内的数据变化，目前没有绑定线程，所以会捕获多线程的全部数据，包括JDK/JVM内部产生的堆内存。
   1. 可以在结果中根据Class信息，筛选自己想要计算的类。
3. 目前不支持一个进程内重复执行functionCreatedObjects函数。

```text
ClassInfo{class=class [C, total=2, size=48, length=2}
```