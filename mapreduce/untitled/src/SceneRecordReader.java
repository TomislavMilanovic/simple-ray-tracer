import org.apache.hadoop.conf.Configuration;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mapred.RecordReader;

import java.io.IOException;

public class SceneRecordReader implements RecordReader<RowPairWritable, Text>
{
    private boolean processed = false;
    private SceneSplit sceneSplit;

    public SceneRecordReader(SceneSplit sceneSplit, Configuration conf) throws IOException
    {
        this.sceneSplit = sceneSplit;
    }

    @Override
    public void close() throws IOException {}

    @Override
    public RowPairWritable createKey()
    {
        return new RowPairWritable();
    }

    @Override
    public Text createValue() {
        return new Text();
    }

    @Override
    public long getPos() throws IOException { return 0; }

    @Override
    public float getProgress() throws IOException
    {
        return processed?1.f:0.f;
    }

    @Override
    public boolean next(RowPairWritable key, Text value)
            throws IOException
    {
        if (!processed)
        {
            key.firstRow = sceneSplit.start;
            key.secondRow = sceneSplit.end;
            value.set(sceneSplit.sceneJson);
            processed = true;
            return true;
        }
        return false;
    }
}
