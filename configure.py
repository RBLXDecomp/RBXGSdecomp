import json
from pathlib import Path

from tools.project import (
    delinkObjs,
    Object,
    NonMatching,
    Matching,
)

root = Path(__file__).parent
obj_dir = root / "obj"
app_obj_dir = root / "Client/App/obj/ReleaseAssert"
glg3d_obj_dir = root / "Client/Rendering/g3d/obj/GLG3D/Release"


objs = [
    Object(NonMatching, obj_dir / "0018_DDSTexture.obj", glg3d_obj_dir / "DDSTexture.obj", "./obj/glg3d/release/DDSTexture.obj"),
    Object(NonMatching, obj_dir / "0019_Draw.obj", glg3d_obj_dir / "Draw.obj", "./obj/glg3d/release/Draw.obj"),
    Object(NonMatching, obj_dir / "0020_DXCaps.obj", glg3d_obj_dir / "DXCaps.obj", "./obj/glg3d/release/DXCaps.obj"),
    Object(NonMatching, obj_dir / "0021_getOpenGLState.obj", glg3d_obj_dir / "getOpenGLState.obj", "./obj/glg3d/release/getOpenGLState.obj"),
    Object(NonMatching, obj_dir / "0022_GFont.obj", glg3d_obj_dir / "GFont.obj", "./obj/glg3d/release/GFont.obj"),
    Object(NonMatching, obj_dir / "0023_glcalls.obj", glg3d_obj_dir / "glcalls.obj", "./obj/glg3d/release/glcalls.obj"),
    Object(NonMatching, obj_dir / "0024_GLCaps.obj", glg3d_obj_dir / "GLCaps.obj", "./obj/glg3d/release/GLCaps.obj"),
    Object(NonMatching, obj_dir / "0025_glenumtostring.obj", glg3d_obj_dir / "glenumtostring.obj", "./obj/glg3d/release/glenumtostring.obj"),
    Object(NonMatching, obj_dir / "0026_GPUProgram.obj", glg3d_obj_dir / "GPUProgram.obj", "./obj/glg3d/release/GPUProgram.obj"),
    Object(NonMatching, obj_dir / "0027_GWindow.obj", glg3d_obj_dir / "GWindow.obj", "./obj/glg3d/release/GWindow.obj"),
    Object(NonMatching, obj_dir / "0028_LightingParameters.obj", glg3d_obj_dir / "LightingParameters.obj", "./obj/glg3d/release/LightingParameters.obj"),
    Object(NonMatching, obj_dir / "0029_Milestone.obj", glg3d_obj_dir / "Milestone.obj", "./obj/glg3d/release/Milestone.obj"),
    Object(NonMatching, obj_dir / "0030_RenderDevice.obj", glg3d_obj_dir / "RenderDevice.obj", "./obj/glg3d/release/RenderDevice.obj"),
    Object(NonMatching, obj_dir / "0031_Shader.obj", glg3d_obj_dir / "Shader.obj", "./obj/glg3d/release/Shader.obj"),
    Object(NonMatching, obj_dir / "0032_Sky.obj", glg3d_obj_dir / "Sky.obj", "./obj/glg3d/release/Sky.obj"),
    Object(NonMatching, obj_dir / "0033_Texture.obj", glg3d_obj_dir / "Texture.obj", "./obj/glg3d/release/Texture.obj"),
    Object(NonMatching, obj_dir / "0034_TextureFormat.obj", glg3d_obj_dir / "TextureFormat.obj", "./obj/glg3d/release/TextureFormat.obj"),
    Object(NonMatching, obj_dir / "0035_TextureManager.obj", glg3d_obj_dir / "TextureManager.obj", "./obj/glg3d/release/TextureManager.obj"),
    Object(NonMatching, obj_dir / "0036_ToneMap.obj", glg3d_obj_dir / "ToneMap.obj", "./obj/glg3d/release/ToneMap.obj"),
    Object(NonMatching, obj_dir / "0037_VAR.obj", glg3d_obj_dir / "VAR.obj", "./obj/glg3d/release/VAR.obj"),
    Object(NonMatching, obj_dir / "0038_VARArea.obj", glg3d_obj_dir / "VARArea.obj", "./obj/glg3d/release/VARArea.obj"),
    Object(NonMatching, obj_dir / "0039_Win32Window.obj", glg3d_obj_dir / "Win32Window.obj", "./obj/glg3d/release/Win32Window.obj"),

    Object(Matching, obj_dir / "0210_Ball.obj", app_obj_dir / "Ball.obj", "./obj/releaseassert/Ball.obj"),
    Object(NonMatching, obj_dir / "0211_Block.obj", app_obj_dir / "Block.obj", "./obj/releaseassert/Block.obj"),
]


def configure():
    delinkObjs(root, obj_dir)
    config = {
        "$schema": "https://raw.githubusercontent.com/encounter/objdiff/main/config.schema.json",
        "build_base": False,
        "units": [],
    }

    with open("objdiff.json", "w", encoding='utf-8') as file:
        for object in objs:
            config["units"].append(object.toUnitObject())

        json.dump(config, file, indent=4)


if __name__ == "__main__":
    configure()
