import json
import argparse
from pathlib import Path

root = Path(__file__).parent

parser = argparse.ArgumentParser()
parser.add_argument("targetPath", help="The directory to your target object files.")

def configure(desiredTargetPath):
    targetPath = root / desiredTargetPath
    basePath = root / "Client/App/obj/ReleaseAssert"

    if not targetPath.is_dir():
        print("Specified target directory does not exist.")
        return
    
    # TODO: categorize objects by which folder theyre in
    config = {
        "$schema": "https://raw.githubusercontent.com/encounter/objdiff/main/config.schema.json",
        "build_base": False,
        "units": [],
    }

    objects = list(f for f in targetPath.iterdir() if f.is_file())
    
    with open("objdiff.json", "w", encoding='utf-8') as file:
        for i, targetObj in enumerate(objects):
            objName = targetObj.name

            config["units"].append({
                "name": targetObj.stem,
                "target_path": str(targetPath / objName)
            })

            if basePath.joinpath(objName).is_file():
                config["units"][i].update({
                    "base_path": str(basePath / objName)
                })

        json.dump(config, file, indent=4)

if __name__ == "__main__":
    args = parser.parse_args()
    configure(args.targetPath)