# path_finder
Pathfinding demonstration utilizing the A* algorithm in an OpenGL-rendered environment. Watch Steve from Minecraft get chased by a Creeper until the Creeper blows up!

## Screenshot
![Creeper chasing a very concerned Steve.](https://jackwatson06.github.io/path_finder/240313_Minecraft_AStar_Screenshot.png)

## Technical Details
The project consists entirely of C++ code (besides a few shaders). It leans on the OpenGL library to render the simulation state to the screen. Several libraries (see **Dependencies**) were utilized to create windows, GUI menus, and handle input. Overall, it demonstrates a mixture of graphical programming alongside game development. The project only works for Windows-based operating systems.

## Development Guide
The following headers describe the project setup, design methodology, and any potential "gotchas".

### Installation
1. Clone this repository locally.
2. Launch the project in Visual Studio.
3. Build and run. All dependencies are included and there should be no issues.

### Architecture
This project maintains a slim directory structure. Since the project will remain small we store all .h files in the _src/headers_ directory and .cpp files in the _src_ directory. The code within each .cpp file should all be on a similar "layer". We define a layer as a level of abstraction above our infrastructure services (file reading, graphics rendering, etc.).

Assets are added within the _dependencies/assets_ folder. You can add shaders and additional objects here.

### Troubleshooting

#### Post Build Issue
When setting up the project we had an issue with Visual Studio not running our _post-build_ events. When project files don't change Visual Studio will not run the _post-build_ events. So we set _DisableFastUpToDateCheck_ to true to make sure Visual Studio will re-run our _post-build_ events.

#### RC Files
Microsoft uses RC files to embed resources into a .exe. You can then load resources using the _FindResource_ function provided by the [_libloaderapi_ header](https://learn.microsoft.com/en-us/windows/win32/api/libloaderapi/). Note that this function uses wchar pointers as input. Our project uses Unicdoe, so interactions with the Windows API must use wchar's. The documentation below links to these concepts.
- [FindResourceW](https://learn.microsoft.com/en-us/windows/win32/api/libloaderapi/nf-libloaderapi-findresourcew)
- [Resource Files]( https://learn.microsoft.com/en-us/cpp/windows/resource-files-visual-studio?view=msvc-170)
- [WChar](https://learn.microsoft.com/en-us/windows/win32/extensible-storage-engine/wchar)

We ran into an issue when loading embedded _images_ using a string to identify a resource (instead of an ID provided in the resource.h file). You MUST create a resource using the following steps to identify a resource using a string (or when using the _LoadImageFromResource_ in our _FileSystem_ class).
1. Right-click on a folder. Then add a new resource.
2. Choose the icon option and click import. The import your image.
3. In your _Resource View_ navigate to the newly created ICON resource name which should be IDI_ICON1 or similar. Click on this row in the resource view.
4. In the _Properties_ window change the ID to "{resource_name}". The quotes are required to make the resource name a string.
5. Open the .rc file recently created. You have to close the _Resource View_ to edit the .rc file.
6. Scroll down to the resource you just added it should look something like this:
```
{resource_name}               ICON                    "minecraft.png"
```
7. Change ICON to RCDATA. Save the file.
8. You should now be able to refer to resources as strings.

## Dependencies
- Simplex (authored by RIT professor Alberto Bobadilla ALL credit goes to him).
- BasicX (authored by RIT professor Alberto Bobadilla ALL credit goes to him).
- [FreeImage](https://freeimage.sourceforge.io/)
- [GLEW](https://www.opengl.org/sdk/libs/GLEW/)
- [GLUT](https://www.opengl.org/resources/libraries/glut/glut_downloads.php)
- [GLFW](https://www.glfw.org/)
- [GLM](https://github.com/g-truc/glm)
- [ImGui](https://github.com/ocornut/imgui)
- [SFML](https://www.sfml-dev.org/)
