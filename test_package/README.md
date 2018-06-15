After exporting the conan package but before uploading it, the new package has to be verified.
Run ```$ conan test . Flow/1.6@spiessensm/stable``` in the current directory to test the package with the specified version.
This will create a copy of the eclipse project with Flow test sources but without the Flow sources itself.
The Flow sources will be imported by means of the newly exported conan package, thus verifying the package.
