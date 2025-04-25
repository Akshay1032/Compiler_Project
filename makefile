#Group 10
#Akshay Shukla 2022A7PS0087P
#Gobind Singh 2022A7PS0083P
#Siddhartha Gotur 2022A7PS0070P
#Sriram Sudheer Hebbale 2022A7PS0147P
#Granth Jain 2022A7PS0172P



# Compiler configuration
CC = gcc -w         # Use GCC as the compiler, suppressing all warnings (-w)
CFLAGS = -Wall -Wextra -g  # Enable additional warnings and debugging information

# Object files and executable name
OBJ = driver.o lexer.o parser.o  # List of object files required for compilation
EXEC = stage1exe                 # Name of the final executable

# Default target: Builds the executable
all: $(EXEC)

# Rule to link object files and create the executable
$(EXEC): $(OBJ)
	$(CC) $(CFLAGS) -o $(EXEC) $(OBJ) -lm  
# Compile and link object files, linking with math library (-lm)

# Rule to compile driver.c into driver.o
driver.o: driver.c
	$(CC) $(CFLAGS) -c driver.c 
# Compile driver.c into an object file

# Rule to compile lexer.c into lexer.o
lexer.o: lexer.c
	$(CC) $(CFLAGS) -c lexer.c  
# Compile lexer.c into an object file

# Rule to compile parser.c into parser.o
parser.o: parser.c
	$(CC) $(CFLAGS) -c parser.c  
# Compile parser.c into an object file

# Rule to clean up generated files (object files and the executable)
clean:
	rm -f $(OBJ) $(EXEC) parsetreeOutFile.txt preprocessedFileForLexer.txt cleaned_tc.txt 
# Remove all compiled object files, intermediate text files and the executable

# Rule to run the compiled executable with input and output files
run: $(EXEC)
	./$(EXEC) testcase.txt parsetreeOutFile.txt  
# Execute the program with input and output files
