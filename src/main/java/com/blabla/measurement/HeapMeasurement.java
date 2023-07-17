package com.blabla.measurement;

import java.util.LinkedList;
import java.util.List;

public class HeapMeasurement {
    static {
        System.loadLibrary("measurement");
    }

    private native void objectsStatisticsOfClass(Class<?> klass, Runnable runnable, Callback func);

    interface Callback {
        void accept(Class clazz, long size, long length, long total);
    }

    public List<ClassInfo> functionCreatedObjects(Runnable function) {
        if (function == null) {
            throw new NullPointerException();
        }
        List<ClassInfo> list = new LinkedList<>();
        objectsStatisticsOfClass(null, function, (clazz, size, length, total) -> {
            if (size == 0 && length == 0 && total == 0) {
                return;
            }
            list.add(new ClassInfo(clazz, size, length, total));
        });
        return list;
    }

    public static void main(String[] args) throws InterruptedException {
        HeapMeasurement self = new HeapMeasurement();
        Runnable testFunc = () -> {
            char[] array1 = new char[1];
            char[] array2 = new char[1];
        };
        testFunc.run();// warm up
        List<ClassInfo> classInfos = self.functionCreatedObjects(testFunc);
        classInfos.forEach(System.out::println);
    }
}