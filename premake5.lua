require "emscripten"

	-- On Linux We have to query the dependencies of gtk+3 for sr_gui, we do this on the host for now.
if os.ishost("linux") then
	listing, code = os.outputof("pkg-config --libs libnotify gtk+-3.0")
	liballLibs = string.explode(string.gsub(listing, "-l", ""), " ")
end


workspace("Calco")
	
	-- Configuration.
	configurations({ "Release", "Dev"})
	location("build")
	targetdir ("build/%{prj.name}/%{cfg.longname}")
	debugdir ("build/%{prj.name}/%{cfg.longname}")
	architecture("x86_64")
	systemversion("latest")

	filter("system:macosx")
		systemversion("10.12:latest")
	filter("system:emscripten")
		architecture("wasm32")
	filter({})

	-- Configuration specific settings.
	filter("configurations:Release")
		defines({ "NDEBUG" })
		optimize("On")

	filter("configurations:Dev")
		defines({ "DEBUG" })
		symbols("On")

	filter({})
	startproject("Calco")

-- Projects	

function CommonFlags()
	language("C++")
	cppdialect("C++17")
	
	-- Compiler flags
	filter("toolset:not msc*")
		buildoptions({ "-Wall", "-Wextra" })
	filter("toolset:msc*")
		buildoptions({ "-W3"})
	filter({})

	-- visual studio filters
	filter("action:vs*")
		defines({ "_CRT_SECURE_NO_WARNINGS" })  
	filter({})

	-- Emscripten
	filter("system:emscripten")
		buildoptions({"-flto", "-O3"})
		linkoptions({"-flto" , "-O3"})
	filter({})

end


group("Libs")

-- Include sr_gui and GLFW premake files.
include("libs/sr_gui/premake5.lua")
include("libs/glfw/premake5.lua")

group("Calco")

project("CalcoTool")
	
	kind("ConsoleApp")
	CommonFlags()

	includedirs({"src/"})
	externalincludedirs({ "libs/", "src/libs" })
	links({"sr_gui"})
	-- common files
	includedirs({ "libs/", "src/libs" })
	files({"src/core/**", "src/libs/glm/**.hpp", "src/libs/glm/*.cpp", "src/libs/glm/**.h", "src/libs/glm/*.c", "src/tool/**", "premake5.lua"})
	removefiles({"**.DS_STORE", "**.thumbs"})
	

project("Calco")
	
	kind("WindowedApp")
	CommonFlags()

	includedirs({"src/"})
	includedirs({ "libs/", "src/libs" })
	
	filter("system: not emscripten")
		includedirs({ "libs/glfw/include/" })
	filter({})

	-- common files
	files({"src/core/**", "src/libs/**.hpp", "src/libs/*/*.cpp", "src/libs/**.h", "src/libs/*/*.c", "src/app/**", "premake5.lua"})
	removefiles({"**.DS_STORE", "**.thumbs"})

	-- per platform files
	filter("action:vs*")
		files({"resources/windows/*"})

	filter("action:xcode*")
		files({"resources/macos/*"})
		
   filter({})
	
	filter("system:emscripten")
		links({"sr_gui", "glfw3"})
	filter("system:not emscripten")
		links({"sr_gui", "glfw3in"})

	-- Libraries for each platform.
	filter("system:macosx")
		links({"OpenGL.framework", "Cocoa.framework", "IOKit.framework", "CoreVideo.framework", "AppKit.framework"})

	filter("system:windows")
		links({"opengl32", "User32", "Comdlg32", "Comctl32", "Shell32", "runtimeobject"})

	filter("system:linux")
		links({"GL", "X11", "Xi", "Xrandr", "Xxf86vm", "Xinerama", "Xcursor", "Xext", "Xrender", "Xfixes", "xcb", "Xau", "Xdmcp", "rt", "m", "pthread", "dl", liballLibs})
	
	filter("system:emscripten")
		linkoptions({"--sUSE_GLFW=3", "--sMAX_WEBGL_VERSION=2",  "--sMIN_WEBGL_VERSION=2", "-lidbfs.js", "--sFORCE_FILESYSTEM=1", "--shell-file ../resources/emscripten/shell_minimal.html" })
		targetextension(".html")
	filter({})


newaction {
   trigger     = "clean",
   description = "Clean the build directory",
   execute     = function ()
      print("Cleaning...")
      os.rmdir("./build")
      print("Done.")
   end
}
