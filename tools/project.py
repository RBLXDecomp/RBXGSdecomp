from pathlib import Path
import urllib.request
import subprocess
import os

Matching = True
Equivalent = False
NonMatching = False


def delinkObjs(root, obj_dir):
    delink_release_link = "https://github.com/HaydnTrigg/delink/releases/download/v0.15.0/delink-windows-x86_64.exe"
    delink_local_path = root / "tools/delink.exe"
    webservice_dll_location = root / "orig/WebService.dll"
    webservice_pdb_location = root / "orig/WebService.pdb"

    if (Path(obj_dir).is_dir()):
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
        out=obj_dir
    )
    subprocess.Popen(command).wait()


class Object:
    def __init__(self, matching, target_path, base_path, pdb_path):
        self.matching = matching
        self.target_path = Path(target_path)
        self.base_path = Path(base_path)
        self.pdb_path = pdb_path

    def toUnitObject(self) -> dict:
        ret = {
            "name": self.pdb_path,
            "target_path": str(self.target_path),
        }

        if self.base_path.is_file():
            ret["base_path"] = str(self.base_path)

        ret["metadata"] = {
            "complete": self.matching
        }
        return ret
