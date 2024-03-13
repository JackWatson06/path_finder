# path_finder
 A* implemented in C++ using a fun Minecraft based models.


 ## Post Build Issue
 We had an issue with vscode not running post build events if the project did not change. We fixed this by adding the following property to the project.
 DisableFastUpToDateCheck to true

 ## RC Files
 RC Files are Microsofts way of incorporating data into a .exe. Essentially when you create a resource it will create a .rc file with some information which tells the compiler how to
 find the resource or include the resource in your .exe. Then to load this resource into the application you use the FindResource function provided in the libloaderapi headers. This documentation
 covers some of these elements.

 https://learn.microsoft.com/en-us/windows/win32/api/libloaderapi/nf-libloaderapi-findresourcew
 https://learn.microsoft.com/en-us/cpp/windows/resource-files-visual-studio?view=msvc-170

 There are a couple gotcahs.
 - In order to refer to a resource by a name you need to edit the .rc file so that you do not use the same ID for the resource as defined in the resource.h file. We use the string name for the LoadImageFromResource fuction.
 If you do then preprocessing of the .rc file will occur replacing that resource locator with an ID instead of a string name. Replace that ID reference with the string you want to use to define your resource.
 Like this:
 minecraft               RCDATA                    "minecraft.ico"
	- You can also change this by going into the linked .ico resource and configure the ID to be a string in the properties window. So you would put in "minecraft" for the ID which will make the ID a string.

1. Right click on a folder > add resource.
2. Choose the icon option and click import > navigate to your .ico image.
3. In your resource view navigate to the newly created ICON resource name which should be IDI_ICON1 or something similar. Click on this line item.
4. In your properties window change the ID to "minecraft". The quotes are required to make it a string.
5. Open the actual .rc file in a text editor or unmount the .rc file in Visual Studio and open it their.
6. Scroll down to the resource you added and change ICON to RCDATA. Save the file.
7. You can now refer to the resrouce as a string rather than a integer id.


- Since we are importing a .png we have to use RCDATA.