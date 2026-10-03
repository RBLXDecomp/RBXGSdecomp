# RBXGS Decompilation
A decompilation of [RBXGS (Roblox Grid Service) version 0.3.634.0](https://archive.org/download/rbxgssetup/S3FileHandler_RBXGSSetup_0.3.634.0.msi), currently focusing on specific critical components (v8kernel, v8world, etc.) for now, and will later expand out to other classes.

You need to use Microsoft Visual Studio 2005 with [SP1 Update](https://web.archive.org/web/20200801000000id_/download.microsoft.com/download/6/3/c/63c69e5d-74c9-48ea-b905-30ac3831f288/VS80sp1-KB926601-X86-ENU.exe) for matching. (Visual C/C++(14.00.50727)[C++]) Additionally, since parts of the code use newer Windows API utilities only introduced in Windows Vista, a [newer version of the Windows SDK](https://web.archive.org/web/20210505032618/https://download.microsoft.com/download/4/2/6/42684501-9ec5-43dd-9dfe-c8c9dfa6a66f/6.1.6000.16384.10.WindowsSDK_Vista_Feb2007Update_rtm.DVD.Rel.iso) is required. 

You have to select ReleaseAssert configuration when compiling the code otherwise it won't produce matching code. To test linking you can select ReleaseAssertDLL configuration (Note: this configuration did not exist in the original Roblox source code and is only present for link testing purposes only)

You can create the [objdiff](https://github.com/encounter/objdiff) project by exporting target objects with the [Object file exporter extension for Ghidra](https://github.com/boricj/ghidra-delinker-extension) and then running `configure.py` with the target directory you put your objects in. Whenever you export new objects you should run the script again for the project to update.

# Dependencies
* [boost 1.34.1](https://www.boost.org/users/history/version_1_34_1.html)
* [SDL 1.2.6](https://www.libsdl.org/release/SDL-1.2.6.zip)
* [Graphics3D 6.09](https://sourceforge.net/projects/g3d/files/g3d-cpp/6.09/)
