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
	architecture("x64")
	systemversion("latest")

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
	sysincludedirs({ "libs/", "src/libs" })

	-- common files
	files({"src/core/**", "src/tool/**", "premake5.lua"})
	removefiles({"**.DS_STORE", "**.thumbs"})
	

project("Calco")
	
	kind("WindowedApp")
	CommonFlags()

	includedirs({"src/"})
	sysincludedirs({ "libs/", "src/libs", "libs/glfw/include/" })

	-- common files
	files({"src/core/**", "src/libs/**.hpp", "src/libs/*/*.cpp", "src/libs/**.h", "src/libs/*/*.c", "src/app/**", "premake5.lua"})
	removefiles({"**.DS_STORE", "**.thumbs"})

	-- per platform files
	filter("action:vs*")
		files({"resources/windows/*"})

	filter("action:xcode*")
		files({"resources/macos/*"})
		
   filter({})
	
	links({"sr_gui", "glfw3"})

	-- Libraries for each platform.
	filter("system:macosx")
		links({"OpenGL.framework", "Cocoa.framework", "IOKit.framework", "CoreVideo.framework", "AppKit.framework"})

	filter("system:windows")
		links({"opengl32", "User32", "Comdlg32", "Comctl32", "Shell32", "runtimeobject"})

	filter("system:linux")
		links({"GL", "X11", "Xi", "Xrandr", "Xxf86vm", "Xinerama", "Xcursor", "Xext", "Xrender", "Xfixes", "xcb", "Xau", "Xdmcp", "rt", "m", "pthread", "dl", liballLibs})
	
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
