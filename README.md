-This code accepts the files in the order of input file first and the s file second, Meaning argv[1] = input file argv[2] = s file
-This code runs based on the fact the variables are only one letter, for example "x" and not "xz"
-This code runs based on the fact the processes are only one number following the p, for example "p0" and not "p10"
-This code runs based on the fact the processes start with the letter 'p' and not some other letter
-The code runs based on the fact the instruction file looks like:
        input_var x,y,z;
        internal_var p0,p1,p2;
           x -> p0;
           y -> p1;
         + z -> p1;
           p1 -> p2;
         - p0 -> p2;
        write(x, y, z, p0, p1, p2).
-Spaces in the instruction file do not matter but there should not be random characters before or after the lines
  For example it should NOT be like this:
        {) input_var x,y,z;
        &@ internal_var p0,p1,p2;
           x -> p0;
           y -> p1 #@!;
         + z -> p1;
           p1 -> p2 &@!;
         - p0 -> p2;
        write(x, y, z, p0, p1, p2).
-Same goes for the input file, spaces do not matter but it should look like this and has to have commas: 
        12,5,7  
  NOT like this:
        *& 12 5 7
If there is something wrong with the files there will be infinite running code, out of range errors, cannot read from file errors, etc. This code should work since it is based on all of the provided input files.
When finished the code output the final values of the processes to the screen with a cout
