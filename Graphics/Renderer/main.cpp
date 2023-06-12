#include "D3D11Renderer.h"

int main()
{
	graphics::D3D11Renderer renderer;

	// Initialize
	if (!renderer.Initialize())
	{
		std::cout << "Initialization failed.\n";
		return -1;
	}

	return renderer.Run();
}