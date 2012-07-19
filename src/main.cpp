#include "testApp.h"
#include "ofAppGlutWindow.h"

int main() {
	ofAppGlutWindow window;
	ofSetupOpenGL(&window, 1920, 1080, OF_WINDOW);
	window.setGlutDisplayString("rgba double depth samples>=4"); 
	window.setGlutDisplayString("rgba double depth alpha samples>=4");
	ofRunApp(new testApp());
}
