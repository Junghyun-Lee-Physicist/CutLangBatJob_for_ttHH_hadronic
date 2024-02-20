
sourceCode="plotStackedHistogramsV9.cpp"
exeName="plotter.exe"

g++ `root-config --cflags` ${sourceCode} -o ${exeName} `root-config --libs`

./plotter.exe

rm -rf plotter.exe

echo "Making plotter execution is done!!"
