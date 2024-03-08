/* Program entry. Init the application then start the run function. */
#include "headers/Application.h"

using namespace Simplex;
int main(void)
{
	Application* pApp = new Application();
	pApp->Init("", RES_C_1280x720_16x9_HD, false, false);
	pApp->Run();
	SafeDelete(pApp);
	return 0;
}
