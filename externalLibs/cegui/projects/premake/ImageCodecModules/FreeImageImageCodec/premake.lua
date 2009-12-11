--
-- CEGUIFreeImageCodec premake script
--

cegui_dynamic("CEGUIFreeImageImageCodec")

package.files =
{
    matchfiles(rootdir.."cegui/src/"..pkgdir.."*.cpp"),
    matchfiles(rootdir.."cegui/include/"..pkgdir.."*.h"),
}

include(rootdir.."cegui/include/"..pkgdir)

library("FreeImage", "d")

dependency("CEGUIBase")
define("CEGUIFREEIMAGEIMAGECODEC_EXPORTS")
define("FREEIMAGE_LIB", "Debug_Static")
define("FREEIMAGE_LIB", "Release_Static")
