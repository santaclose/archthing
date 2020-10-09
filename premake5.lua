workspace "archthing"
	architecture "x64"
	startproject "archthing"

	configurations
	{
		"Debug",
		"Release"
	}
	
	flags
	{
		"MultiProcessorCompile"
	}

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

-- Projects
group "Dependencies"
	include "rofv/vendor/GLFW"
	include "rofv/vendor/Glad"
	include "rofv/vendor/imgui"
group ""

project "archthing"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++17"
	staticruntime "on"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	files
	{
		"src/**.h",
		"src/**.cpp",
		"rofv/src/**.h",
		"rofv/src/**.cpp",
		"rofv/vendor/stb_image/**.h",
		"rofv/vendor/stb_image/**.cpp",
		"rofv/vendor/glm/glm/**.hpp",
		"rofv/vendor/glm/glm/**.inl",
		"rofv/vendor/baseGL/src/*.cpp"
	}

	defines
	{
		"_CRT_SECURE_NO_WARNINGS"
	}

	includedirs
	{
		"rofv/src",
		"rofv/vendor/spdlog/include",
		"rofv/vendor/GLFW/include",
		"rofv/vendor/Glad/include",
		"rofv/vendor/imgui",
		"rofv/vendor/glm",
		"rofv/vendor/stb_image",
		"rofv/vendor/baseGL/src"
	}

	links 
	{ 
		"GLFW",
		"Glad",
		"ImGui",
		"opengl32.lib"
	}

	filter "system:windows"
		systemversion "latest"

		defines
		{
			"ARCHTHING_PLATFORM_WINDOWS",
			"GLFW_INCLUDE_NONE"
		}

	filter "configurations:Debug"
		defines "ARCHTHING_DEBUG"
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		defines "ARCHTHING_RELEASE"
		runtime "Release"
		optimize "on"
