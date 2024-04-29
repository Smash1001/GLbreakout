Our project is built to use Visual Studio 2022 on Windows.

Set include and library directories in 'VC++ directories' in the solution's 'property pages' to the 'include' and 'lib' folders respectively.

(Alternatively, include folder contents to user's include and library directories)

Add 'glfw3.lib' 'opengl32.lib' and 'freetype.lib' to 'addtional dependencies' under the 'linker' settings.

Make sure to include glad.c in the solution.

Make sure 'freetype.dll' is included in Windows' 'System32' folder, if not, it is provided in the repo.

Dependencies:

glfw3.lib

freetype.lib

freetype.dll

glad
