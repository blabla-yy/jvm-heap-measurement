package com.blabla.measurement;

public class ClassInfo {
    private final long tag;
    private final Class<?> clazz;
    private long total;
    private long size;
    private long length;

    public ClassInfo(Class<?> clazz, long size, long length, long total) {
        // this.tag = tag;
        this.tag = 0;
        this.clazz = clazz;
        this.total = total;
        this.size = size;
        this.length = length;
    }

    public long getTag() {
        return tag;
    }

    public Class<?> getClazz() {
        return clazz;
    }

    public long getTotal() {
        return total;
    }

    public long getSize() {
        return size;
    }

    public long getLength() {
        return length;
    }

    public void add(long size, long length, long total) {
        this.total += total;
        this.size += size;
        this.length += length;
    }

    public void compareTo(ClassInfo another) {
        if (another == null) {
            return;
        }

        this.total = this.getTotal() - another.getTotal();
        this.size = this.getSize() - another.getSize();
        this.length = this.getLength() - another.getLength();
    }

    @Override
    public String toString() {
        return "ClassInfo{" +
                "class=" + clazz +
                ", total=" + total +
                ", size=" + size +
                ", length=" + length +
                '}';
    }
}
