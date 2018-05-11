package cplusplus_test;

import java.io.BufferedReader;
import java.io.File;
import java.io.IOException;
import java.io.InputStreamReader;
import java.util.ArrayList;
import java.util.List;

public class Main
{
	public static void main(String[] args) throws IOException
	{
	    Process p;     // Process tracks one external native process
	    BufferedReader is;  // reader for output of process
	    String line;
	    
	    List<String> command = new ArrayList<String>();
	    command.add("./simple_ray_tracer");
	    command.add("../render_input_test/");
	    command.add("0");
	    command.add("200");
	    
	    ProcessBuilder pb = new ProcessBuilder(command);
	    pb.directory(new File("/home/tomislav/Desktop/build-simple-ray-tracer-Desktop-Default"));
	    p = pb.start();
	
	    // getInputStream gives an Input stream connected to
	    // the process p's standard output. Just use it to make
	    // a BufferedReader to readLine() what the program writes out.
	    is = new BufferedReader(new InputStreamReader(p.getInputStream()));
	
	    while ((line = is.readLine()) != null)
	      System.out.println(line);
	  
	    System.out.flush();
	    try { p.waitFor();}
	    catch (InterruptedException e)
	    {
	      System.err.println(e);  // "Can'tHappen"
	      return;
	    };
	   
	    System.out.println("Finished!");
	    return;
	}
}
