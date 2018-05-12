import org.apache.hadoop.io.Text;

import java.io.DataInput;
import java.io.DataOutput;
import java.io.IOException;

public class SceneSplit implements org.apache.hadoop.mapred.InputSplit
{
    public int start, end;
    public Text sceneJson;

    public SceneSplit() {}

    public SceneSplit(int start, int end, Text sceneJson)
    {
        this.start = start;
        this.end = end;
        this.sceneJson = sceneJson;
    }

    @Override
    public void readFields(DataInput in) throws IOException
    {
        start = in.readInt();
        end = in.readInt();
        sceneJson = new Text(in.readUTF());
    }

    @Override
    public void write(DataOutput out) throws IOException
    {
        out.writeInt(start);
        out.writeInt(end);
        out.writeUTF(sceneJson.toString());
    }

    @Override
    public long getLength() throws IOException {
        return 12;
    }

    @Override
    public String[] getLocations() throws IOException
    {
        String[] result = new String[1];
        result[0] = "";
        return result;
    }
}
