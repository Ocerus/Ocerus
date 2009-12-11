--
-- CEGUI premake script
--

-- omit some things if we are part of a single solution along with the core libs
if not CEGUI_CORE_LIBRARY_SOLUTION then
    require("helpers")
    require("config")

    --
    -- project setup
    --

    project.name = "CEGUISamples"
    project.bindir = rootdir.."bin"
    project.libdir = rootdir.."lib"
    project.configs =
    {
    	"Debug",
    	"Debug_Static",
    	"ReleaseWithSymbols",
    	"Release",
    	"Release_Static"
    }
end

local pkg_table =
{
    ["Samples"] =
    {
        "common",
        "FirstWindow",
        "TextDemo",
        "Demo6",
        "Demo7",
        "DragDropDemo",
        "FalagardDemo1",
        "FontDemo",
        "Minesweeper",
        "ScrollablePaneDemo",
        "TabControlDemo",
        "TreeDemo"
    }
}

local lua_samples =
{
    "Demo8",
}

if LUA_SCRIPT_MODULE then
    for _,v in ipairs(lua_samples) do
        tinsert(pkg_table.Samples, v)
    end
end

createpackages(pkg_table)

-- the end
