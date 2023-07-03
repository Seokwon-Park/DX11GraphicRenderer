#include "D3D11Renderer.h"

int main()
{
	graphics::D3D11Renderer renderer;

	// Initialize Renderer
	if (!renderer.Initialize())
	{
		// If failed, print message and return -1
		std::cout << "Initialization failed.\n";
		return -1;
	}

	// Running Renderer
	return renderer.Run();
}