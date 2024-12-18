all:
	javac -d bin/ -sourcepath ./src src/org/rail5/shellwatch/Main.java
	cd bin/ && jar cfe shellwatch.jar org.rail5.shellwatch.Main org/
	mv bin/shellwatch.jar .

clean:
	rm -rf bin/*
	rm -f shellwatch.jar