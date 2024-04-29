Our project is made to use Visual Studio 2022 on Windows.

Set include and library directories in 'VC++ directories' in the solution's 'property pages' to the 'include' and 'lib' folders respectively.

('Property pages' can be accessed by right-clicking 'Breakout' in the solution explorer window and selecting 'property pages')

(The folders' contents can be copied to the user's include and library directories specified in their VS settings)

Add 'glfw3.lib' 'opengl32.lib' and 'freetype.lib' to 'addtional dependencies' under the 'input' subheadder in 'linker' settings in 'property pages'.

Make sure to include glad.c in the solution.

Make sure 'freetype.dll' is included in Windows' 'System32' folder, if not, it is provided in the repo.

Dependencies:

glfw3.lib

freetype.lib

freetype.dll

glad
