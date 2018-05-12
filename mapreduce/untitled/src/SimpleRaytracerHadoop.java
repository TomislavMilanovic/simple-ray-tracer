import org.apache.hadoop.fs.Path;
import org.apache.hadoop.io.IntWritable;
import org.apache.hadoop.io.NullWritable;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mapred.*;

import java.io.IOException;
import java.util.Iterator;

public class SimpleRaytracerHadoop
{
    public static class Map extends MapReduceBase implements
            Mapper<RowPairWritable, Text, IntWritable, Text>
    {
        public void map(RowPairWritable keyrow, Text sceneJson,
                        OutputCollector<IntWritable, Text> output,
                        Reporter reporter) throws IOException
        {
            int start = keyrow.firstRow;
            int end = keyrow.secondRow;

            output.collect(new IntWritable(start), new Text("LOL"));
        }
    }

    public static class Reduce extends MapReduceBase implements
            Reducer<IntWritable, Text, NullWritable, Text>
    {
        public void reduce(IntWritable keyrow, Iterator<Text> values,
                           OutputCollector<NullWritable, Text> output,
                           Reporter reporter) throws IOException
        {
            output.collect(null, values.next());
        }
    }

    public static void main(String[] args) throws Exception
    {
        JobConf conf = new JobConf();
        conf.setJobName("Hadoop Raytracer");
        conf.setJarByClass(SimpleRaytracerHadoop.class);
        
        conf.setOutputKeyClass(NullWritable.class);
        conf.setOutputValueClass(Text.class);

        conf.setMapperClass(Map.class);
        conf.setMapOutputKeyClass(IntWritable.class);
        conf.setMapOutputValueClass(Text.class);
        conf.setReducerClass(Reduce.class);

        conf.setInputFormat(SceneFileInputFormat.class);

        conf.setNumReduceTasks(1);
        
        String inpath = args[0];
        String outpath = args[1];

        FileInputFormat.setInputPaths(conf, new Path(inpath));
        FileOutputFormat.setOutputPath(conf, new Path(outpath));
        JobClient.runJob(conf);
    }
}
