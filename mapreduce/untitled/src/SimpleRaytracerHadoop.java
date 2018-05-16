import org.apache.hadoop.fs.FileStatus;
import org.apache.hadoop.fs.FileSystem;
import org.apache.hadoop.fs.Path;
import org.apache.hadoop.io.IntWritable;
import org.apache.hadoop.io.NullWritable;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mapred.*;

import java.io.BufferedReader;
import java.io.File;
import java.io.IOException;
import java.io.InputStreamReader;
import java.util.ArrayList;
import java.util.Iterator;
import java.util.List;

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

            Process p;     // Process tracks one external native process
            BufferedReader is;  // reader for output of process
            String line;

            List<String> command = new ArrayList<String>();
            command.add("./simple_ray_tracer");
            command.add("../mapreduce/render_input_test/");
            command.add(String.valueOf(start));
            command.add(String.valueOf(end));

            ProcessBuilder pb = new ProcessBuilder(command);
            pb.directory(new File("/home/tomislav/Desktop/simple-ray-tracer/cmake-build-debug"));
            p = pb.start();

            // getInputStream gives an Input stream connected to
            // the process p's standard output. Just use it to make
            // a BufferedReader to readLine() what the program writes out.
            is = new BufferedReader(new InputStreamReader(p.getInputStream()));
            String result = "";
            boolean _switch = false;

            while ((line = is.readLine()) != null)
            {
                if(line.equals("COLOR LIST END"))
                    _switch = false;
                if (_switch)
                    result = result.concat(line + "\n");
                if(line.equals("COLOR LIST START"))
                    _switch = true;
            }

            System.out.flush();
            try { p.waitFor();}
            catch (InterruptedException e)
            {
                System.err.println(e);  // "Can'tHappen"
                return;
            }

            output.collect(new IntWritable(start), new Text(result));
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
