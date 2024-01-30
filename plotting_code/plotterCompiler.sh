
sourceCode="stackedHistogramsPlotter.cpp"
exeName="plotter.exe"

g++ `root-config --cflags` ${sourceCode} -o ${exeName} `root-config --libs`
