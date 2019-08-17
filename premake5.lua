dependencies = {
	basePath = "./deps"
}

function dependencies.load()
	dir = path.join(dependencies.basePath, "premake/*.lua")
	deps = os.matchfiles(dir)

	for i, dep in pairs(deps) do
		dep = dep:gsub(".lua", "")
		require(dep)
	end
end

function dependencies.imports()
	for i, proj in pairs(dependencies) do
		if type(i) == 'number' then
			proj.import()
		end
	end
end

function dependencies.projects()
	for i, proj in pairs(dependencies) do
		if type(i) == 'number' then
			proj.project()
		end
	end
end

dependencies.load()

workspace "ifs-tool"
	startproject "ifs-tool"
	location "./build"
	objdir "%{wks.location}/obj"
	targetdir "%{wks.location}/bin/%{cfg.platform}/%{cfg.buildcfg}"
	
	configurations {
		"Debug",
		"Release",
	}
	platforms { "x32" }

	buildoptions "/std:c++latest"
	systemversion "latest"
	symbols "On"
	staticruntime "On"
	editandcontinue "Off"
	warnings "Extra"
	characterset "ASCII"

	flags {
		"NoIncrementalLink",
		"NoMinimalRebuild",
		"MultiProcessorCompile",
		"No64BitChecks"
	}

	configuration "windows"
		defines {
			"_WINDOWS",
			"WIN32",
		}

	configuration "Release"
		optimize "Full"

		defines {
			"NDEBUG",
		}

		flags {
			"FatalCompileWarnings",
		}

	configuration "Debug"
		optimize "Debug"

		defines {
			"DEBUG",
			"_DEBUG",
		}

	configuration {}

	project "ifs-tool"
		kind "ConsoleApp"
		language "C++"
		
		files {
			"./src/**.rc",
			"./src/**.hpp",
			"./src/**.cpp",
		}
		
		includedirs {
			"./src"
		}
		
		resincludedirs {
			"$(ProjectDir)src"
		}

		pchheader "std_include.hpp"
		pchsource "src/std_include.cpp"
		
		dependencies.imports()

	group "Dependencies"
		dependencies.projects()