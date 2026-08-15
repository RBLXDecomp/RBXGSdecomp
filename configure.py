import json
import argparse
from pathlib import Path
import urllib.request
import subprocess
import os

root = Path(__file__).parent

delink_release_link = "https://github.com/HaydnTrigg/delink/releases/download/v0.15.0/delink-windows-x86_64.exe"
delink_local_path = root / "tools/delink.exe"
webservice_dll_location = root / "orig/WebService.dll"
webservice_pdb_location = root / "orig/WebService.pdb"
out_dir = root / "obj"

objs = [
    [
        out_dir / "0210_Ball.obj",
        root / "Client/App/obj/ReleaseAssert/Ball.obj",
        "./obj/ReleaseAssert/Ball.obj"
    ],
    [
        out_dir / "0211_Block.obj",
        root / "Client/App/obj/ReleaseAssert/Block.obj",
        "./obj/ReleaseAssert/Block.obj"
    ]
]


def delinkObjs():
    if (Path(out_dir).is_dir()):
        return

    if not Path(webservice_dll_location).is_file() or not Path(webservice_pdb_location).is_file():
        raise Exception("Please put WebService.dll and WebService.pdb in the orig folder!")

    if not Path(root / "tools").is_dir():
        os.makedirs("tools")

    if not Path(delink_local_path).is_file():
        urllib.request.urlretrieve(delink_release_link, delink_local_path)

    command = "{delink} pe-split {WebServiceLocation} --pdb {PDBLocation} --outdir {out}".format(
        delink=delink_local_path,
        WebServiceLocation=webservice_dll_location,
        PDBLocation=webservice_pdb_location,
        out=out_dir
    )
    p = subprocess.Popen(command)
    p.wait()


def configure():
    basePath = root / "Client/App/obj/ReleaseAssert"
    
    # TODO: categorize objects by which folder theyre in
    config = {
        "$schema": "https://raw.githubusercontent.com/encounter/objdiff/main/config.schema.json",
        "build_base": False,
        "units": [],
    }
    
    with open("objdiff.json", "w", encoding='utf-8') as file:
        for i, object in enumerate(objs):

            print(str(objs[i][0]))

            config["units"].append({
                "name": str(objs[i][2]),
                "target_path": str(objs[i][0])
            })

            if objs[i][1].is_file():
                config["units"][i].update({
                    "base_path": str(objs[i][1])
                })

        json.dump(config, file, indent=4)

if __name__ == "__main__":
    delinkObjs()
    configure()