package site.ycsb.generator;

public abstract class NumberGenerator {

    protected long lastValue = 0;

    public abstract Long nextValue();

    public long lastValue() {
        return lastValue;
    }

    public void setLastValue(long val) {
        this.lastValue = val;
    }

    public double mean() {
        throw new UnsupportedOperationException("mean() not implemented");
    }
}
