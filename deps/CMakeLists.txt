cmake_minimum_required(VERSION 2.8)

# Dependencies will be included as submodules if
# their target is not defined
#
# If you include your own version of a dependency,
# make sure the target has the correct include directories
# configured with target_include_directories(target **PUBLIC** include)
#
# TODO: check with find_package if the dependency is already installed

# targets expected: yogacore freetype

########
# yoga #
########
if(NOT TARGET yogacore)
	# We can't use the CMakeLists.txt present in yoga/
	# because they link to "android log" by default
	# and don't expose an option to turn it off :(
	file(GLOB_RECURSE yogacore_SRC yoga/yoga/*.cpp)
	add_library(yogacore STATIC ${yogacore_SRC})
	target_include_directories(yogacore PUBLIC yoga)
endif()

#############
# freetype2 #
#############
if(NOT TARGET freetype)
	set(SKIP_INSTALL_ALL ON)
	
	add_subdirectory(freetype2)
endif()


