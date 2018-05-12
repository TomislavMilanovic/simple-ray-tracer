import java.io.DataInput;
import java.io.DataOutput;
import java.io.IOException;

public class RowPairWritable implements org.apache.hadoop.io.WritableComparable<RowPairWritable>
{
    public int firstRow, secondRow;

    public RowPairWritable(int firstRow, int secondRow)
    {
        this.firstRow = firstRow;
        this.secondRow = secondRow;
    }

    public RowPairWritable()
    {
        this.firstRow = 0;
        this.secondRow = 0;
    }

    @Override
    public void readFields(DataInput in) throws IOException
    {
        firstRow = in.readInt();
        secondRow = in.readInt();
    }

    @Override
    public void write(DataOutput out) throws IOException
    {
        out.writeInt(firstRow);
        out.writeInt(secondRow);
    }

    @Override
    public int compareTo(RowPairWritable o)
    {
        if (o instanceof RowPairWritable)
        {
            int thisValue = this.firstRow;
            int thatValue = o.firstRow;
            if(thisValue < thatValue) return -1;
            else if (thisValue > thatValue) return 1;
            else return 0;
        }
        return 0;
    }
}
