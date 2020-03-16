# script om de file om te schrijven
import re
from argparse import ArgumentParser

parser = ArgumentParser()
parser.add_argument("file", type=str, help="name of file to write results to")
args = parser.parse_args()
filename = args.file + ".rs"

rs = open(filename, "r")

rslines = rs.readlines()

index = rslines.index("    let mem_init = MemInitial::default()\n") + 1
numbers = re.findall(r'\d+', rslines[index])

Tword = "T" + str(numbers[0])
mword = "m" + str(numbers[1])

rslines.remove('use reo_rs::building::{*, NameDef::*};\n')
rslines.remove("use reo_rs::{Instruction::*, Term::*};\n")

rslines.insert(7, "#[allow(unused_imports)]\n")
rslines.insert(8, "use reo_rs::{\n")
rslines.insert(9, "    building::{NameDef::*, *},\n")
rslines.insert(10, "    Instruction::*,\n")
rslines.insert(11, "    Term::*,\n")
rslines.insert(12, "};\n")

rslines[14] = "pub extern \"C\" fn reors_generated_proto_create() -> CProtoHandle {\n"

line = "    " + Tword + ": 'static + Send + Sync + Sized,\n"
line2 = "    " + Tword + ": 'static + Send + Sync + Sized + core::str::FromStr,\n"
rslines[rslines.index(line)] = line2


line = "        .with<" + Tword + ">(\"" + mword +"\", \"\"0\"\".into());\n"
line2 = "        .with(\"" + mword + "\", " + Tword + "::from_str_expect(\"0\"));\n"
rslines[rslines.index(line)] = line2

rs.close()

rsnew = open(filename, "w")

rsnew.writelines(rslines)

rsnew.close()
