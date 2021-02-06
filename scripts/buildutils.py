import os
import sys
import time
from subprocess import Popen, PIPE
import signal
import shutil

defaultconfig = {
    'ccompiler': '/usr/bin/gcc',
    'cppcompiler': '/usr/bin/g++',
    'cflags': '-Wall -Wextra -g -fPIC',
    'cppflags': '-std=c++17 -Wall -Wextra -g -fPIC'
}

exampleproject = {
    'name': 'project',
    'path': 'project',
    'source': 'src',
    'build': 'build',
    'include': [
        '/usr/include',
        'include',
    ],
    'format': 'executable', # or shared
    'linkerargs': '-lpthread',
    'copy': [
        {
            'filetypes': [ '.glsl' ],
            'source': 'src/shaders',
            'dest': 'shaders'
        }
    ]
}

colors = {
    'none': '\033[0m',
    'red': '\033[0;31m',
    'green': '\033[0;32m',
    'blue': '\033[0;34m'
}

def build(projectconfig: dict, cconfig: dict = defaultconfig) -> bool:
    """Main function of this library, compiles to objects, creates executable and generates compile_commands.json"""
    # handle KeyboardInterrupt
    signal.signal(signal.SIGINT, interrupthandler)

    # start message
    print('Building', colors['blue'] + projectconfig['name'] + colors['none'])

    # change cwd
    workingDirectory = os.getcwd()
    if not os.getcwd().endswith('/' + projectconfig['path']):
        os.chdir(os.path.dirname(os.path.abspath(sys.argv[0])) + '/' + projectconfig['path'])
    
    # Compile
    objectPath = cleanPath(projectconfig['build']) + '/obj'
    genCompileCommandsJson(cconfig, projectconfig['include'], cleanPath(projectconfig['source']), objectPath)
    changedFiles = getChangedFiles(cleanPath(projectconfig['source']), objectPath, projectconfig['include'])
    if not compile(cconfig, projectconfig['include'], cleanPath(projectconfig['source']), changedFiles, objectPath):
        print('Stopping due to compile error.')
        return False

    # Link the binary
    objectFiles = getObjectFiles(objectPath)
    objectNames = []
    for file in objectFiles:
        objectNames.append(objectPath + '/' + file['path'])
    binPath = projectconfig['build'] + '/bin'
    if not os.path.exists(binPath):
        os.makedirs(binPath)
    if (projectconfig['format'] == 'executable'):
        if not makeExecutable(cconfig, projectconfig['name'], objectNames, cleanPath(projectconfig['build']) + '/bin', projectconfig['linkerargs']):
            return False
    elif projectconfig['format'] == 'shared':
        if not makeShared(cconfig, projectconfig['name'], objectNames, cleanPath(projectconfig['build']) + '/bin', projectconfig['linkerargs']):
            return False
        copyFiles(cleanPath(projectconfig['source']), cleanPath(projectconfig['build']) + '/include', [ 'hpp', 'h' ])

    # Copy files
    if 'copy' in projectconfig:
        for copy in projectconfig['copy']:
            copyFiles(cleanPath(copy['source']), cleanPath(projectconfig['build']) + '/' + cleanPath(copy['dest']), copy['filetypes'])
    
    # Change cwd back
    os.chdir(workingDirectory)
    return True

def compile(cconfig: dict, includeDirs: list[str], sourceDir: str, sources: list[str], objectPath: str) -> bool:
    """Compile the source files into the output directory. Only output the text if a command failes. Show a loading animation instead."""
    if len(sources) == 0:
        print('Nothing to do.')
        return True
    i = 1
    startTime = time.time()
    for file in sources:
        progress('Compiling ' + file, i, len(sources))
        command = getCompileCommand(cconfig, includeDirs, file, sourceDir, objectPath)
        process = Popen(command.split(' '), stdout=PIPE, stdin=PIPE, stderr=PIPE)
        stdout, stderr = process.communicate()
        exit_code = process.wait()
        if exit_code != 0:
            progress('Failed to compile ' + file, i, len(sources), False)
            print('\n', stdout.decode('utf-8'), '\n', stderr.decode('utf-8'))
            return False
        i+=1
        elapsedTime = time.time() - startTime
        minutes, seconds = divmod(elapsedTime, 60)
        print('\r', colors['blue'], '{:02.0f}m {:02.0f}s'.format(minutes, seconds), colors['none'], ' ', file, '     ')
    print('Done compiling.\n')
    return True

def makeShared(cconfig: dict, name: str, objects: list[str], targetDir: str, linkerargs: str) -> bool:
    name = 'lib' + name + '.so'
    print('Linking shared library ' + name + '... ', end='')
    command = [ cconfig['cppcompiler'], '-shared' ] + linkerargs.split(' ') + objects + [ '-o', targetDir + '/' + name ]
    process = Popen(command, stdout=PIPE, stdin=PIPE, stderr=PIPE)
    stdout, stderr = process.communicate()
    exit_code = process.wait()
    if exit_code != 0:
        print(colors['red'], 'Failed', colors['none'])
        print(stdout.decode('utf-8'), '\n', stderr.decode('utf-8'))
        return False
    print(colors['green'], 'Done', colors['none'])
    return True

def makeExecutable(cconfig: dict, name: str, objects: list[str], targetDir: str, linkerargs: str) -> bool:
    print('Linking executable ' + name + '... ', end='')
    command = [ cconfig['cppcompiler'] ] + linkerargs.split(' ') + objects + [ '-o', targetDir + '/' + name ]
    process = Popen(command, stdout=PIPE, stdin=PIPE, stderr=PIPE)
    stdout, stderr = process.communicate()
    exit_code = process.wait()
    if exit_code != 0:
        print(colors['red'], 'Failed', colors['none'])
        print(stdout.decode('utf-8'), '\n', stderr.decode('utf-8'))
        return False
    print(colors['green'], 'Done', colors['none'])
    return True

def genCompileCommandsJson(cconfig: dict, includeDirs: list[str], source: str, build: str):
    """Generate the compile_commands.json file for language servers"""
    print('Generating compile_commands.json... ', end='')
    output = '[\n'
    for root, _, files in os.walk(source):
        for file in files:
            if file.endswith('.cpp') or file.endswith('.c'):
                path = root + '/' + file
                output += '{\n'
                output += f'"directory": "{os.getcwd()}",\n'
                output += f'"command": "{getCompileCommand(cconfig, includeDirs, path, source, build)}",\n'
                output += f'"file": "{path}"\n'
                output += '},\n'
    output = output[:-2] + '\n]'
    compile_commands_file = open('compile_commands.json', 'w')
    compile_commands_file.write(output)
    print(colors['green'] + 'Done' + colors['none'])

def getCompileCommand(cconfig: dict, includeDirs: list[str], file: str, source: str, build: str) -> str:
    includeText = ''
    for includeDir in includeDirs:
        includeText += ' -I' + includeDir
    fileDir, _ = os.path.split(build + '/' + file)
    if not os.path.exists(fileDir):
        os.makedirs(fileDir)
    fileRoot, _ = os.path.splitext(file)
    command = ''
    if file.endswith('.cpp'):
        command = f'{cconfig["cppcompiler"]} {cconfig["cppflags"]} -fdiagnostics-color=always {includeText} -o {build}/{fileRoot}.o -c {file}'
    elif file.endswith('.c'):
        command = f'{cconfig["ccompiler"]} {cconfig["cflags"]} -fdiagnostics-color=always {includeText} -o {build}/{fileRoot}.o -c {file}'
    else:
        raise Exception("Invalid File: " + file)
    return ' '.join(command.split())

def getChangedFiles(source: str, build: str, includeDirs: list[str]) -> list[str]:
    """Get all source and header files that are newer than their object file"""
    if build[0:1] == './':
        build = build[2:]
    if source[0:1] == './':
        source = source[2:]

    # Get source, header and object files
    sourceFiles = []
    headerFiles = []
    for root, _, files in os.walk(source):
        if build in root:
            continue
        root = cleanPath(root)
        for file in files:
            path = root + '/' + file
            if file.endswith('.cpp') or file.endswith('.c'):
                sourceFiles.append({ 'path': path, 'modified': os.path.getmtime(path) })
            elif file.endswith('.hpp') or file.endswith('.h'):
                headerFiles.append({ 'path': path, 'modified': os.path.getmtime(path) })

    objectFiles = getObjectFiles(build)

    # Update modified time of source files dependent on the used headers
    for sourceFile in sourceFiles:
        header_candidates = []
        for headerFile in headerFiles:
            for includeDir in includeDirs:
                if headerFile['path'].startswith(includeDir):
                    header_candidates.append({
                        'name': headerFile['path'][len(includeDir) + 1:],
                        'path': headerFile['path'],
                        'modified': headerFile['modified']
                    })
        dependencies = getDependencies(sourceFile['path'], header_candidates)
        for dependency in dependencies:
            if dependency['modified'] > sourceFile['modified']:
                sourceFile['modified'] = dependency['modified']

    # Compare source and object times
    changedSourceFiles = []
    for sourceFile in sourceFiles:
        objectExists = False
        for objectFile in objectFiles:
            sourceRoot, _ = os.path.splitext(sourceFile['path'])
            objectRoot, _ = os.path.splitext(objectFile['path'])
            if (sourceRoot == objectRoot):
                objectExists = True
                if (sourceFile['modified'] > objectFile['modified']):
                    changedSourceFiles.append(sourceFile['path'])
        if not objectExists:
            changedSourceFiles.append(sourceFile['path'])
    return changedSourceFiles

def getDependencies(file: str, candidates: list[dict]) -> list[dict]:
    dependencies = []
    file_content = open(file, 'r').read()
    for candidate in candidates:
        if (candidate['name'] in file_content):
            dependencies.append(candidate)
            candidates.remove(candidate)
    if any(candidates):
        for dependency in dependencies:
            dependencies += getDependencies(dependency['path'], candidates)
    return dependencies

def getObjectFiles(build: str) -> list[dict]:
    objectFiles = []
    for root, _, files in os.walk(build):
        relativeroot = root[len(build) + 1:]
        for file in files:
            relfilepath = relativeroot + '/' + file
            absfilepath = root + '/' + file
            if file.endswith('.o'):
                objectFiles.append({ 'path': relfilepath, 'modified': os.path.getmtime(absfilepath) })
    return objectFiles

def copyFiles(sourceDir: str, destDir: str, filetypes: list[str]):
    sourceDir = cleanPath(sourceDir)
    destDir = cleanPath(destDir)
    print('copy filetypes', filetypes, 'from', sourceDir, 'to', destDir + '... ', end='')
    for root, _, files in os.walk(sourceDir):
        if root.startswith(destDir):
            continue
        root = cleanPath(root)
        for file in files:
            for type in filetypes:
                if file.endswith(type):
                    filepath = root + '/' + file
                    shortroot = root
                    if shortroot.startswith(sourceDir):
                        shortroot = shortroot[len(sourceDir)+1:]
                    destRoot = cleanPath(destDir + '/' + shortroot)
                    if not os.path.exists(destRoot):
                        os.makedirs(destRoot)
                    shutil.copyfile(filepath, destRoot + '/' + file)
                    break
    print(colors['green'] + 'Done' + colors['none'])

def progress(text: str, current_progress: int, max_progress: int, success: bool = True):
    color = colors['red']
    if success:
        color = colors['green']

    print(f'\r{color}[{current_progress}/{max_progress}] {colors["none"]}{text}', end='');

def cleanPath(path: str) -> str:
    if path.startswith('./'):
        path = path[2:]
    if path.endswith('/'):
        path = path[:-1]
    return path

def interrupthandler(signal, frame):
    print('\nInterrupted by user. Stopping...')
    exit(1)
