#!/usr/bin/python
import re

def ReadFile(file_path):
    f = open(file_path, 'rb')
    content = f.read()
    f.close()
    return content

data = ReadFile('data')
print ' << data from https://c9x.me/x86/html/file_module_x86_id_146.html >>'
print data

parsed = re.findall('([A-Z]*) rel', data)
print '\n << parsed data >>'
print parsed

print '\n << code generated >>'
for insn in parsed:
    print 'branch_insn.insert(\"' + insn.lower() + '\");'

